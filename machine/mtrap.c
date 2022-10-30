// See LICENSE for license details.

#include "mtrap.h"
#include "mcall.h"
#include "htif.h"
#include "atomic.h"
#include "bits.h"
#include "vm.h"
#include "uart.h"
#include "uartlite.h"
#include "uart16550.h"
#include "xuart.h"
#include "finisher.h"
#include "fdt.h"
#include "unprivileged_memory.h"
#include "disabled_hart_mask.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

extern void __am_uartlite_putchar(unsigned char data);
extern unsigned char __am_uartlite_getchar();

void __attribute__((noreturn)) bad_trap(uintptr_t* regs, uintptr_t dummy, uintptr_t mepc)
{
  die("machine mode: unhandlable trap %d @ %p", read_csr(mcause), mepc);
}

static uintptr_t mcall_console_putchar(uint8_t ch)
{
    if (uart) {
        uart_putchar(ch);
    } else if (xuart) {
        xuart_putchar(ch);
    } else if (uartlite) {
        uartlite_putchar(ch);
    } else if (uart16550) {
        uart16550_putchar(ch);
    } else if (htif) {
        htif_console_putchar(ch);
    } else {
        __am_uartlite_putchar(ch);
    }
    return 0;
}

void putstring(const char* s)
{
  while (*s)
    mcall_console_putchar(*s++);
}

void vprintm(const char* s, va_list vl)
{
  char buf[256];
  vsnprintf(buf, sizeof buf, s, vl);
  putstring(buf);
}

void printm(const char* s, ...)
{
  va_list vl;

  va_start(vl, s);
  vprintm(s, vl);
  va_end(vl);
}

static void send_ipi(uintptr_t recipient, int event)
{
  if (((disabled_hart_mask >> recipient) & 1)) return;
  atomic_or(&OTHER_HLS(recipient)->mipi_pending, event);
  mb();
  *OTHER_HLS(recipient)->ipi = 1;
}

static uintptr_t mcall_console_getchar()
{
  if (uart) {
    return uart_getchar();
  } else if (xuart) {
    return xuart_getchar();
  } else if (uartlite) {
    return uartlite_getchar();
  } else if (uart16550) {
    return uart16550_getchar();
  } else if (htif) {
    return htif_console_getchar();
  } else { /* snps */
    return __am_uartlite_getchar(); 
  }
}

static uintptr_t mcall_clear_ipi()
{
  return clear_csr(mip, MIP_SSIP) & MIP_SSIP;
}

static uintptr_t mcall_shutdown()
{
  poweroff(0);
}

static uintptr_t mcall_set_timer(uint64_t when)
{
  *HLS()->timecmp = when;
  clear_csr(mip, MIP_STIP);
  set_csr(mie, MIP_MTIP);
  return 0;
}

static uintptr_t mcall_plic_eoi()
{
  clear_csr(mip, MIP_SEIP);
  set_csr(mie, MIP_MEIP);
  return 0;
}

static void send_ipi_many(uintptr_t* pmask, int event)
{
  _Static_assert(MAX_HARTS <= 8 * sizeof(*pmask), "# harts > uintptr_t bits");
  uintptr_t mask = hart_mask;
  if (pmask)
    mask &= load_uintptr_t(pmask, read_csr(mepc));

  // send IPIs to everyone
  for (uintptr_t i = 0, m = mask; m; i++, m >>= 1)
    if (m & 1)
      send_ipi(i, event);

  if (event == IPI_SOFT)
    return;

  // wait until all events have been handled.
  // prevent deadlock by consuming incoming IPIs.
  uint32_t incoming_ipi = 0;
  for (uintptr_t i = 0, m = mask; m; i++, m >>= 1)
    if (m & 1)
      while (*OTHER_HLS(i)->ipi)
        incoming_ipi |= atomic_swap(HLS()->ipi, 0);

  // if we got an IPI, restore it; it will be taken after returning
  if (incoming_ipi) {
    *HLS()->ipi = incoming_ipi;
    mb();
  }
}

long write_perf_csr(uint64_t csr, uint64_t data);
long read_perf_csr(uint64_t csr);

void mcall_trap(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
  write_csr(mepc, mepc + 4);

  uintptr_t n = regs[17], arg0 = regs[10], arg1 = regs[11], retval, ipi_type;

  switch (n)
  {
    case SBI_CONSOLE_PUTCHAR:
      retval = mcall_console_putchar(arg0);
      break;
    case SBI_CONSOLE_GETCHAR:
      retval = mcall_console_getchar();
      break;
    case SBI_SEND_IPI:
      ipi_type = IPI_SOFT;
      goto send_ipi;
    case SBI_REMOTE_SFENCE_VMA:
    case SBI_REMOTE_SFENCE_VMA_ASID:
      ipi_type = IPI_SFENCE_VMA;
      goto send_ipi;
    case SBI_REMOTE_FENCE_I:
      ipi_type = IPI_FENCE_I;
send_ipi:
      send_ipi_many((uintptr_t*)arg0, ipi_type);
      retval = 0;
      break;
    case SBI_CLEAR_IPI:
      retval = mcall_clear_ipi();
      break;
    case SBI_SHUTDOWN:
      retval = mcall_shutdown();
      break;
    case SBI_SET_TIMER:
#if __riscv_xlen == 32
      retval = mcall_set_timer(arg0 + ((uint64_t)arg1 << 32));
#else
      retval = mcall_set_timer(arg0);
#endif
      break;
    case SBI_PLIC_EOI:
      retval = mcall_plic_eoi();
    case SBI_SET_PERF:
      mcall_console_putchar('@');
      retval = write_perf_csr(arg0, arg1);
      break;
    case SBI_GET_PERF:
      mcall_console_putchar('$');
      retval = read_perf_csr(arg0);
      break;
    default:
      retval = -ENOSYS;
      break;
  }
  regs[10] = retval;
}

#define csr_read(csr)						\
({								\
	register unsigned long __v;				\
	__asm__ __volatile__ ("csrr %0, " #csr			\
			      : "=r" (__v) :			\
			      : "memory");			\
	__v;							\
})

#define csr_write(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrw " #csr ", %0"		\
			      : : "rK" (__v)			\
			      : "memory");			\
})

long write_perf_csr(uint64_t csr, uint64_t data) {
  // write according to csr number
  switch (csr) {
    case 0xb00:
      csr_write(0xb00, data);
      break;
    case 0xb01:
      csr_write(0xb01, data);
      break;
    case 0xb02:
      csr_write(0xb02, data);
      break;
    case 0xb03:
      csr_write(0xb03, data);
      break;
    case 0xb04:
      csr_write(0xb04, data);
      break;
    case 0xb05:
      csr_write(0xb05, data);
      break;
    case 0xb06:
      csr_write(0xb06, data);
      break;
    case 0xb07:
      csr_write(0xb07, data);
      break;
    case 0xb08:
      csr_write(0xb08, data);
      break;
    case 0xb09:
      csr_write(0xb09, data);
      break;
    case 0xb0a:
      csr_write(0xb0a, data);
      break;
    case 0xb0b:
      csr_write(0xb0b, data);
      break;
    case 0xb0c:
      csr_write(0xb0c, data);
      break;
    case 0xb0d:
      csr_write(0xb0d, data);
      break;
    case 0xb0e:
      csr_write(0xb0e, data);
      break;
    case 0xb0f:
      csr_write(0xb0f, data);
      break;
    case 0xb10:
      csr_write(0xb10, data);
      break;
    case 0xb11:
      csr_write(0xb11, data);
      break;
    case 0xb12:
      csr_write(0xb12, data);
      break;
    case 0xb13:
      csr_write(0xb13, data);
      break;
    case 0xb14:
      csr_write(0xb14, data);
      break;
    case 0xb15:
      csr_write(0xb15, data);
      break;
    case 0xb16:
      csr_write(0xb16, data);
      break;
    case 0xb17:
      csr_write(0xb17, data);
      break;
    case 0xb18:
      csr_write(0xb18, data);
      break;
    case 0xb19:
      csr_write(0xb19, data);
      break;
    case 0xb1a:
      csr_write(0xb1a, data);
      break;
    case 0xb1b:
      csr_write(0xb1b, data);
      break;
    case 0xb1c:
      csr_write(0xb1c, data);
      break;
    case 0xb1d:
      csr_write(0xb1d, data);
      break;
    case 0xb1e:
      csr_write(0xb1e, data);
      break;
    case 0xb1f:
      csr_write(0xb1f, data);
      break;
    // counter end, event begin
    case 0x320:
      csr_write(0x320, data);
      break;
    case 0x321:
      csr_write(0x321, data);
      break;
    case 0x322:
      csr_write(0x322, data);
      break;
    case 0x323:
      csr_write(0x323, data);
      break;
    case 0x324:
      csr_write(0x324, data);
      break;
    case 0x325:
      csr_write(0x325, data);
      break;
    case 0x326:
      csr_write(0x326, data);
      break;
    case 0x327:
      csr_write(0x327, data);
      break;
    case 0x328:
      csr_write(0x328, data);
      break;
    case 0x329:
      csr_write(0x329, data);
      break;
    case 0x32a:
      csr_write(0x32a, data);
      break;
    case 0x32b:
      csr_write(0x32b, data);
      break;
    case 0x32c:
      csr_write(0x32c, data);
      break;
    case 0x32d:
      csr_write(0x32d, data);
      break;
    case 0x32e:
      csr_write(0x32e, data);
      break;
    case 0x32f:
      csr_write(0x32f, data);
      break;
    case 0x330:
      csr_write(0x330, data);
      break;
    case 0x331:
      csr_write(0x331, data);
      break;
    case 0x332:
      csr_write(0x332, data);
      break;
    case 0x333:
      csr_write(0x333, data);
      break;
    case 0x334:
      csr_write(0x334, data);
      break;
    case 0x335:
      csr_write(0x335, data);
      break;
    case 0x336:
      csr_write(0x336, data);
      break;
    case 0x337:
      csr_write(0x337, data);
      break;
    case 0x338:
      csr_write(0x338, data);
      break;
    case 0x339:
      csr_write(0x339, data);
      break;
    case 0x33a:
      csr_write(0x33a, data);
      break;
    case 0x33b:
      csr_write(0x33b, data);
      break;
    case 0x33c:
      csr_write(0x33c, data);
      break;
    case 0x33d:
      csr_write(0x33d, data);
      break;
    case 0x33e:
      csr_write(0x33e, data);
      break;
    case 0x33f:
      csr_write(0x33f, data);
      break;
    default:
      return -1;
  }
  return 1;
}

long read_perf_csr(uint64_t csr) {
  // read according to csr number
  switch (csr) {
    case 0xb00:
      return csr_read(0xb00);
    case 0xb01:
      return csr_read(0xb01);
    case 0xb02:
      return csr_read(0xb02);
    case 0xb03:
      return csr_read(0xb03);
    case 0xb04:
      return csr_read(0xb04);
    case 0xb05:
      return csr_read(0xb05);
    case 0xb06:
      return csr_read(0xb06);
    case 0xb07:
      return csr_read(0xb07);
    case 0xb08:
      return csr_read(0xb08);
    case 0xb09:
      return csr_read(0xb09);
    case 0xb0a:
      return csr_read(0xb0a);
    case 0xb0b:
      return csr_read(0xb0b);
    case 0xb0c:
      return csr_read(0xb0c);
    case 0xb0d:
      return csr_read(0xb0d);
    case 0xb0e:
      return csr_read(0xb0e);
    case 0xb0f:
      return csr_read(0xb0f);
    case 0xb10:
      return csr_read(0xb10);
    case 0xb11:
      return csr_read(0xb11);
    case 0xb12:
      return csr_read(0xb12);
    case 0xb13:
      return csr_read(0xb13);
    case 0xb14:
      return csr_read(0xb14);
    case 0xb15:
      return csr_read(0xb15);
    case 0xb16:
      return csr_read(0xb16);
    case 0xb17:
      return csr_read(0xb17);
    case 0xb18:
      return csr_read(0xb18);
    case 0xb19:
      return csr_read(0xb19);
    case 0xb1a:
      return csr_read(0xb1a);
    case 0xb1b:
      return csr_read(0xb1b);
    case 0xb1c:
      return csr_read(0xb1c);
    case 0xb1d:
      return csr_read(0xb1d);
    case 0xb1e:
      return csr_read(0xb1e);
    case 0xb1f:
      return csr_read(0xb1f);
    // counter end, event begin
    case 0x320:
      return csr_read(0x320);
    case 0x321:
      return csr_read(0x321);
    case 0x322:
      return csr_read(0x322);
    case 0x323:
      return csr_read(0x323);
    case 0x324:
      return csr_read(0x324);
    case 0x325:
      return csr_read(0x325);
    case 0x326:
      return csr_read(0x326);
    case 0x327:
      return csr_read(0x327);
    case 0x328:
      return csr_read(0x328);
    case 0x329:
      return csr_read(0x329);
    case 0x32a:
      return csr_read(0x32a);
    case 0x32b:
      return csr_read(0x32b);
    case 0x32c:
      return csr_read(0x32c);
    case 0x32d:
      return csr_read(0x32d);
    case 0x32e:
      return csr_read(0x32e);
    case 0x32f:
      return csr_read(0x32f);
    case 0x330:
      return csr_read(0x330);
    case 0x331:
      return csr_read(0x331);
    case 0x332:
      return csr_read(0x332);
    case 0x333:
      return csr_read(0x333);
    case 0x334:
      return csr_read(0x334);
    case 0x335:
      return csr_read(0x335);
    case 0x336:
      return csr_read(0x336);
    case 0x337:
      return csr_read(0x337);
    case 0x338:
      return csr_read(0x338);
    case 0x339:
      return csr_read(0x339);
    case 0x33a:
      return csr_read(0x33a);
    case 0x33b:
      return csr_read(0x33b);
    case 0x33c:
      return csr_read(0x33c);
    case 0x33d:
      return csr_read(0x33d);
    case 0x33e:
      return csr_read(0x33e);
    case 0x33f:
      return csr_read(0x33f);
    default:
      return -1;
  }
}

void redirect_trap(uintptr_t epc, uintptr_t mstatus, uintptr_t badaddr)
{
  write_csr(sbadaddr, badaddr);
  write_csr(sepc, epc);
  write_csr(scause, read_csr(mcause));
  write_csr(mepc, read_csr(stvec));

  uintptr_t new_mstatus = mstatus & ~(MSTATUS_SPP | MSTATUS_SPIE | MSTATUS_SIE);
  uintptr_t mpp_s = MSTATUS_MPP & (MSTATUS_MPP >> 1);
  new_mstatus |= (mstatus * (MSTATUS_SPIE / MSTATUS_SIE)) & MSTATUS_SPIE;
  new_mstatus |= (mstatus / (mpp_s / MSTATUS_SPP)) & MSTATUS_SPP;
  new_mstatus |= mpp_s;
  write_csr(mstatus, new_mstatus);

  extern void __redirect_trap();
  return __redirect_trap();
}

void pmp_trap(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
  redirect_trap(mepc, read_csr(mstatus), read_csr(mbadaddr));
}

static void machine_page_fault(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
  // MPRV=1 iff this trap occurred while emulating an instruction on behalf
  // of a lower privilege level. In that case, a2=epc and a3=mstatus.
  // a1 holds MPRV if emulating a load or store, or MPRV | MXR if loading
  // an instruction from memory.  In the latter case, we should report an
  // instruction fault instead of a load fault.
  if (read_csr(mstatus) & MSTATUS_MPRV) {
    if (regs[11] == (MSTATUS_MPRV | MSTATUS_MXR)) {
      if (mcause == CAUSE_LOAD_PAGE_FAULT)
        write_csr(mcause, CAUSE_FETCH_PAGE_FAULT);
      else if (mcause == CAUSE_LOAD_ACCESS)
        write_csr(mcause, CAUSE_FETCH_ACCESS);
      else
        goto fail;
    } else if (regs[11] != MSTATUS_MPRV) {
      goto fail;
    }

    return redirect_trap(regs[12], regs[13], read_csr(mbadaddr));
  }

fail:
  bad_trap(regs, mcause, mepc);
}

void trap_from_machine_mode(uintptr_t* regs, uintptr_t dummy, uintptr_t mepc)
{
  uintptr_t mcause = read_csr(mcause);

  switch (mcause)
  {
    case CAUSE_LOAD_PAGE_FAULT:
    case CAUSE_STORE_PAGE_FAULT:
    case CAUSE_FETCH_ACCESS:
    case CAUSE_LOAD_ACCESS:
    case CAUSE_STORE_ACCESS:
      return machine_page_fault(regs, mcause, mepc);
    default:
      bad_trap(regs, dummy, mepc);
  }
}

void poweroff(uint16_t code)
{
  printm("Power off\r\n");
  finisher_exit(code);
  if (htif) {
    htif_poweroff();
  } else {
    send_ipi_many(0, IPI_HALT);
    while (1) { asm volatile ("wfi\n"); }
  }
}
