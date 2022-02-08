#include <riscv.h>

#ifndef _UART_REG_H_
#define _UART_REG_H_

#define DDR4_1600

#define           UART0_BASE	  0x310B0000	     //Size=  64K	 Max_offset=  0x00010000
#define           UART_BASE	  UART0_BASE          //Size=  64K	 Max_offset=  0x00010000
/*uart register definitions*/
#define SYNC_ADDR UART_BASE + 0xbc

#define RBR       UART_BASE + 0x00 + 0x10000*UART_NUM  
#define THR       UART_BASE + 0x00 + 0x10000*UART_NUM 
#define DLL       UART_BASE + 0x00 + 0x10000*UART_NUM 
#define DLH       UART_BASE + 0x04 + 0x10000*UART_NUM 
#define IER       UART_BASE + 0x04 + 0x10000*UART_NUM 
#define IIR       UART_BASE + 0x08 + 0x10000*UART_NUM 
#define FCR       UART_BASE + 0x08 + 0x10000*UART_NUM 
#define LCR       UART_BASE + 0x0c + 0x10000*UART_NUM 
#define MCR       UART_BASE + 0x10 + 0x10000*UART_NUM 
#define LSR       UART_BASE + 0x14 + 0x10000*UART_NUM 
#define MSR       UART_BASE + 0x18 + 0x10000*UART_NUM 
#define SCR_UART  UART_BASE + 0x1c + 0x10000*UART_NUM 
#define LPDLL     UART_BASE + 0x20 + 0x10000*UART_NUM 
#define LPDLH     UART_BASE + 0x24 + 0x10000*UART_NUM 
#define USR       UART_BASE + 0x7c + 0x10000*UART_NUM 
#define HTX       UART_BASE + 0xa4 + 0x10000*UART_NUM 
#define DMASA     UART_BASE + 0xa8 + 0x10000*UART_NUM 
#define UCV       UART_BASE + 0xf8 + 0x10000*UART_NUM 
#define CTR       UART_BASE + 0xfc + 0x10000*UART_NUM 

#define RFE  0x80
#define TEMT 0x40
#define THRE 0x20
#define BI   0x10
#define FE   0X08
#define PE   0X04
#define OE   0X02
#define DR   0X01
#define DTR  0X01
#define RTS  0X02
#define AFCE 0X20
#define SIRE 0X40
#define CTS  0X10

#endif

#define UART_NUM 0

#define REG_WRITE(addr, value)  (*((volatile unsigned int*)((unsigned long long)addr))) = (value)
#define REG_READ(addr)  (*((volatile unsigned int*)((unsigned long long)addr)))

//cru define
#define CRU_BASE 0x31120000 //cru base config

void writechar(unsigned char ch);

void initUart(void);

static inline void newline(void)
{
    writechar('\r');
    writechar('\n');
}

static inline void htoa(uint64_t base)
{
    register int i = 0;
    register uint64_t mask = 0xfULL << 60;
    for (i = 0; i < 16; i++) {
        uint64_t c = (base & mask) >> (60 - i * 4);
        if (c < 10) {
            c = '0' + c;
        }
        else {
            c = 'a' + (c - 0xa);
        }
        writechar(c);
        mask = mask >> 4;
    }
}

static inline void writestr(const char *s)
{
    while (*s) {
        writechar(*s++);
    }
}

static inline void hex_log(const char *prefix, uint64_t data)
{
    writestr(prefix);
    htoa(data);
    newline();
}

void cru_clk_change(){
    // no need ?
	//REG_WRITE(CRU_BASE+0x08,0x00);
}


void delay_1(unsigned int loops){
	while(loops--){
        writechar('.');
	}
}

void cru_clk_change_withnocpu(){
    // REG_WRITE(CRU_BASE+0x08,0x02);
    // CPU, DDR to SYS_CLK
    REG_WRITE(CRU_BASE+0x08,0x06);
    // init DDR-1600 (PLL to 400M)
    // REG_WRITE(CRU_BASE+0x50, 0x00864348);
    REG_WRITE(CRU_BASE+0x50, 0x01032148);
    delay_1(10000);
    // DDR to PLL
    REG_WRITE(CRU_BASE+0x08,0x02);
}

typedef union {
    struct {
        uint32_t sd_bclk : 4;
        uint32_t aaa : 4;
        uint32_t bbb : 4;
        uint32_t gmac_tx : 4;
        uint32_t nothing : 16;
    };
    uint32_t data;
}__attribute__((__packed__)) CLK_DIV_2;


void cru_cpu_change()
{
#ifdef DDR4_1600_HP
  #define DDR4_1600
#endif

#ifdef DDR4_2400
  REG_WRITE(CRU_BASE+0x08,0x02);//cpu切换到低频时钟，准备进行pll寄存器的配置。
#endif
#ifdef DDR4_1600
  REG_WRITE(CRU_BASE+0x08,0x06);//cpu,ddr切换到低频时钟，准备进行pll寄存器的配置。
#endif
  // 480M: 24/3*60(0x3c)
	// REG_WRITE(CRU_BASE+0x40,0x0001203c);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x44,0x00000000);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x48,0x00000300);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x4c,0x00000088);//pll0_crt0 config
  // 640M: 24/3*80(0x50)
	// REG_WRITE(CRU_BASE+0x40,0x00012050);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x44,0x00000000);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x48,0x00000300);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x4c,0x00000088);//pll0_crt0 config
  // 800M: 24/3*100(0x64)
	// REG_WRITE(CRU_BASE+0x40,0x00012064);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x44,0x00000000);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x48,0x00000300);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x4c,0x00000088);//pll0_crt0 config
  // 1G
	REG_WRITE(CRU_BASE+0x40,0x0001207d);//pll0_crt0 config
	REG_WRITE(CRU_BASE+0x44,0x00000000);//pll0_crt0 config
	REG_WRITE(CRU_BASE+0x48,0x00000300);//pll0_crt0 config
	REG_WRITE(CRU_BASE+0x4c,0x00000088);//pll0_crt0 config
  // 1.2G: 24/3*150(0x96)
  // REG_WRITE(CRU_BASE+0x40,0x00012096);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x44,0x00000000);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x48,0x00000300);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x4c,0x00000088);//pll0_crt0 config
  // 1.26G: 24/6*315(0x13b)
  // REG_WRITE(CRU_BASE+0x40,0x0001213b);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x44,0x00000000);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x48,0x00000600);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x4c,0x00000088);//pll0_crt0 config
  // while (!(REG_READ(CRU_BASE+0x4c) & 0x10000)) {
  // }

  //DDR-2400: clk to 600MHz
	// REG_WRITE(CRU_BASE+0x50,0x00012064);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x54,0x00000000);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x58,0x00000200);//pll0_crt0 config
	// REG_WRITE(CRU_BASE+0x5c,0x00000080);//pll0_crt0 config
#ifdef DDR4_1600
  // DDR-1600: clk to 400MHz
	REG_WRITE(CRU_BASE+0x50,0x00013064);//pll0_crt0 config
	REG_WRITE(CRU_BASE+0x54,0x00000000);//pll0_crt0 config
	REG_WRITE(CRU_BASE+0x58,0x00000200);//pll0_crt0 config
	REG_WRITE(CRU_BASE+0x5c,0x00000080);//pll0_crt0 config
  while (!(REG_READ(CRU_BASE+0x5c) & 0x10000)) {
  }
#endif
	delay_1(50);
	REG_WRITE(CRU_BASE+0x08,0x00);

#ifdef DDR4_1600_HP
  #undef DDR4_1600
#endif
}

void uart_delay(unsigned int loops)
{
  while(loops--){
    __asm__ volatile ("nop");
  }
}

void printInfoddrinit(){
  register unsigned int value asm("t0");
  register unsigned int status asm("t1");
  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x44);
  
  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x44);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x52);
}

void printInfo(){
  register unsigned int value asm("t0");
  register unsigned int status asm("t1");
  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x48);
  
  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x65);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x6C);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x6C);
  
  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x6F);
  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x2C);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x58);
 
  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x69);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x61);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x6E);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x67);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x53);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x68);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x61);

  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x6E);
  value = REG_READ(LSR);
  status = !(value & 0x60);
  //while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  while(status) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
    status = !(value & 0x60);
  }
  //REG_WRITE(THR,data);
  REG_WRITE(THR,0x21);
}

inline void writechar(unsigned char data)
{
  unsigned int value;

  value = REG_READ(LSR);
  while(!(value & 0x60)) // transmit empty and transmit holding reg empty
  {
    //uart_delay(100);
    value = REG_READ(LSR);
  }
  REG_WRITE(THR,data);
}

int readchar()
{
  unsigned int value;
  int recv;

  value = REG_READ(LSR);
  if (value & 0x1) {
    recv = REG_READ(RBR);
  }
  else {
    recv = -1;
  }

  return recv;
}

void initUart()
{
	
	REG_WRITE(LCR,0x83); //0x0c
    while((REG_READ(USR)&0x1) == 0x1);//0x7c
    REG_WRITE(DLH,0x0);   //0x04
    //REG_WRITE(DLL,0xa2);   //0x00 25MHz/9600/16
    //REG_WRITE(DLL,0x44);   //0x00 125M/('h44*16) ~= 115200
    REG_WRITE(DLL,0x36);   //0x00 100M/('h36*16) ~= 115200
    //REG_WRITE(DLL,0x82);   //0x00 240M/('h82*16) ~= 115200
    //REG_WRITE(DLL,0x10);   //0x00 2.4M/('h10*16) ~= 9600
    //REG_WRITE(DLL,0x0f);   //0x00 2.4M/('h0f*16) ~= 9600
    //REG_WRITE(DLL,0x4);   //0x00 100M/('h36*16) ~= 1562500
    //REG_WRITE(DLL,0x2);   //0x00 100M/('h36*16) ~=   3125000
    //REG_WRITE(DLL,0xd);   //0x00 25M/('h13*16) ~=   115200
    //REG_WRITE(DLL,0x82);   //0x00 20MHz/9600
    //REG_WRITE(DLL+0x10000*uart_num,0x1);   //0x00 100M/('h36*16) ~= 115200
    REG_WRITE(LCR,0x03);  //0x0c
    REG_WRITE(FCR,0x01);  //0x08  // enable fifo
    REG_WRITE(IER,0x0);  //0x04
    REG_WRITE(MCR,(RTS | DTR) );  //0x10
}

void __am_init_uartlite(void) {
  initUart();
}

void __am_uartlite_putchar(char ch) {
  if (ch == '\n') __am_uartlite_putchar('\r');
  writechar(ch);
}

int __am_uartlite_getchar() {
  return readchar();
}

void copyAndRun(void)
{
  extern uint64_t _start[];
  extern uint64_t _run[];
  extern uint64_t _data[];
  //extern uint64_t _end[];

  #define ROM_START 0x10000000
  #define RAM_START 0x80000000

  register uint64_t *romStart asm("t0") = (uint64_t *)(unsigned long long)ROM_START;
  register uint64_t *ramStart asm("t1") = (uint64_t *)(unsigned long long)RAM_START;
  register uint64_t *start asm("t2") = &_start[0];
  register uint64_t *end asm("t3") = &_data[0];
  //register uint64_t *end asm("t3") = &_end[0];
  uint64_t size = end - start;

  uint64_t i = 0;
  for (i = 0; i < size; i += 16) {
     #define MACRO(j) ramStart[i + j] = romStart[i + j]
     #define MACRO4(j) MACRO(j); MACRO(j + 1); MACRO(j + 2); MACRO(j + 3);
     MACRO4(0); MACRO4(4); MACRO4(8); MACRO4(12);

     if ((i & 0x1ffff) == 0) {  // per 512KB (0x20000 * 4)
         writechar('*');
     }
  }

  register uint64_t *run asm("t4") = &_run[0];
  uint64_t runOffset = run - start;
  register uint64_t *runAddr asm("t5") = ramStart + runOffset;
  __asm__ __volatile__("fence.i");
  (*(void(*) ())runAddr)();
}

void initBSS(void)
{
  #define RAM_START 0x80000000
  uint64_t *ramStart  = (uint64_t *)(unsigned long long)RAM_START;

  extern uint64_t _start[];
  extern uint64_t _bss_start[];
  extern uint64_t _stack_top[];
  uint64_t *bssStart = &_bss_start[0];
  uint64_t *bssEnd   = &_stack_top[0];
  uint64_t bssSize = bssEnd - bssStart;

  uint64_t *codeStart = &_start[0];
  uint64_t codeSize = bssStart - codeStart;
  ramStart += codeSize;
  uint64_t i = 0;
  for (i = 0; i < bssSize; i += 16) {
     #define MEMSET(j) ramStart[i + j] = 0;
     #define MEMSET4(j) MEMSET(j); MEMSET(j + 1); MEMSET(j + 2); MEMSET(j + 3);
     MEMSET4(0); MEMSET4(4); MEMSET4(8); MEMSET4(12);

     if ((i & 0x1ffff) == 0) {  // per 512KB
         writechar('#');
     }
  }
  
}

//-------------------------------------------
#define READ  1
#define WRITE 0
#define MC    0
#define PHY   1

typedef struct {
    uint8_t rw;
    uint8_t mcPhy;
    uint64_t addr;
    uint32_t data;
} DDRCmd;

//mc  0x00_3106_0000-0x00_3106_FFFF
//phy 0x00_3107_0000-0x00_3107_FFF
#ifdef DDR4_2400
DDRCmd cmds[] = 
{
  // mc config
  {WRITE, MC,  0x31060304, 0x00000001}, // DBG1
  {WRITE, MC,  0x31060030, 0x00000001}, // Self-Ref & Power Down
  {WRITE, MC,  0x31060000, 0x40040610}, // 2T timing
  {WRITE, MC,  0x31060010, 0x00000010}, // MR Control
  {WRITE, MC,  0x31060014, 0x0000465c}, // MR Data
  {WRITE, MC,  0x3106001c, 0xa2e0f445}, // MR PDA
  {WRITE, MC,  0x31060030, 0x00000088}, // Self-Ref & Power Down
  {WRITE, MC,  0x31060034, 0x000f0002}, // Self-Ref & Power Down Timing
  {WRITE, MC,  0x31060038, 0x006a0000}, // Hardware Low Power
  {WRITE, MC,  0x31060050, 0x00001060}, // 
  {WRITE, MC,  0x31060060, 0x00000000},
  // x16
  {WRITE, MC,  0x31060064, 0x0049014a},
  // x8
  // {WRITE, MC,  0x31060064, 0x009200d3},
  {WRITE, MC,  0x310600c0, 0x00000000},
  {WRITE, MC,  0x310600c4, 0x00001010},
  {WRITE, MC,  0x310600d0, 0x00020027},
  {WRITE, MC,  0x310600d4, 0x00270006},
  {WRITE, MC,  0x310600dc, 0x0a310001},
  {WRITE, MC,  0x310600e0, 0x10180200},
  {WRITE, MC,  0x310600e4, 0x00110000},
  {WRITE, MC,  0x310600e8, 0x00000640},
  {WRITE, MC,  0x310600ec, 0x00000800},
  {WRITE, MC,  0x310600f0, 0x00000010},
  // x16
  {WRITE, MC,  0x31060100, 0x11122914},
  // x8
  // {WRITE, MC,  0x31060100, 0x110d2914}, // rw2pre, t_faw, t_rax_max, t_ras_min
  {WRITE, MC,  0x31060104, 0x0004051b}, // null, t_xp, rd2pre, t_rc
  {WRITE, MC,  0x31060108, 0x06080510}, // wl, rl, 
  {WRITE, MC,  0x3106010c, 0x0000f00f},
  // x16
  {WRITE, MC,  0x31060110, 0x08030409},
  // x8
  // {WRITE, MC,  0x31060110, 0x08030309},
  {WRITE, MC,  0x31060114, 0x0f0e0404},
  {WRITE, MC,  0x31060120, 0x06060e08},
  // x16
  {WRITE, MC,  0x31060124, 0x0002040d},
  // x8
  // {WRITE, MC,  0x31060124, 0x0002020d},
  {WRITE, MC,  0x31060128, 0x000f0e05},
  {WRITE, MC,  0x3106012c, 0x14090112},
  {WRITE, MC,  0x31060130, 0x0c00000d},
  {WRITE, MC,  0x3106013c, 0x00000000},
  {WRITE, MC,  0x31060180, 0x51000040},
  {WRITE, MC,  0x31060184, 0x00000070},
  {WRITE, MC,  0x31060190, 0x048b820a},
  {WRITE, MC,  0x31060194, 0x01090606},
  {WRITE, MC,  0x31060198, 0x07008120},
  {WRITE, MC,  0x3106019c, 0x00000061},
  {WRITE, MC,  0x310601a0, 0x20400004},
  {WRITE, MC,  0x310601a4, 0x0003002e},
  {WRITE, MC,  0x310601a8, 0x00000000},
  {WRITE, MC,  0x310601b0, 0x00000041},
  {WRITE, MC,  0x310601b8, 0x00000000},
  {WRITE, MC,  0x310601c0, 0x00000001},
  {WRITE, MC,  0x310601c4, 0x00000000},
  // x16
  {WRITE, MC,  0x31060204, 0x003f1818},
  {WRITE, MC,  0x31060208, 0x00000000},
  {WRITE, MC,  0x3106020c, 0x00000000},
  {WRITE, MC,  0x31060210, 0x00001f1f},
  {WRITE, MC,  0x31060214, 0x04040404},
  {WRITE, MC,  0x31060218, 0x04040404},
  {WRITE, MC,  0x3106021c, 0x00000f0f},
  {WRITE, MC,  0x31060220, 0x00001a1a},
  // x8
/*  {WRITE, MC,  0x31060204, 0x003f0007},
  {WRITE, MC,  0x31060208, 0x03000400},
  {WRITE, MC,  0x3106020c, 0x02020507},
  {WRITE, MC,  0x31060210, 0x00001f1f},
  {WRITE, MC,  0x31060214, 0x0b080709},
  {WRITE, MC,  0x31060218, 0x0406080b},
  {WRITE, MC,  0x3106021c, 0x00000f0f},
  {WRITE, MC,  0x31060220, 0x00000204},*/
  {WRITE, MC,  0x31060224, 0x02020107},
  {WRITE, MC,  0x31060228, 0x0704080a},
  {WRITE, MC,  0x3106022c, 0x00000007},
  // {WRITE, MC,  0x31060240, 0x0f1e0264},
  // {WRITE, MC,  0x31060244, 0x00000001},
  // {WRITE, MC, 0x31060240, 0x0700060c}, // odt 0 change!!!
  {WRITE, MC, 0x31060240, 0x070a0634}, // odt 1 change!!!
  {WRITE, MC, 0x31060244, 0x00000010}, // odt 0 change!!!
  {WRITE, MC,  0x31060250, 0x1aa51704},
  {WRITE, MC,  0x31060254, 0x00000039},
  {WRITE, MC,  0x3106025c, 0x40001e0b},
  {WRITE, MC,  0x31060264, 0x2c006921},
  {WRITE, MC,  0x3106026c, 0x79001d5c},
  {WRITE, MC,  0x31060300, 0x00000001},
  {WRITE, MC,  0x31060304, 0x00000000},
  {WRITE, MC,  0x3106030c, 0x00000000},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060328, 0x00000000},
  {WRITE, MC,  0x3106036c, 0x00000010},
  {WRITE, MC,  0x31060400, 0x00000010},
  {WRITE, MC,  0x31060404, 0x000023e7},
  {WRITE, MC,  0x31060408, 0x0000036e},
  {WRITE, MC,  0x31060490, 0x00000000},
  {WRITE, MC,  0x31060494, 0x00100005},
  // mc reset done
  //{READ ,0x31060ff4, 0x00000000},
  {READ , MC,  0x31060ff4, 0x67612a2a}, //read only
  // mc init
  {WRITE, MC,  0x31060304, 0x00000000},
  {WRITE, MC,  0x31060030, 0x00000088},
  {WRITE, MC,  0x31060030, 0x00000088},
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x310601b0, 0x00000040},
  // phy config
  {WRITE, PHY, 0x31070024, 0x0020403b},
  {WRITE, PHY, 0x31070148, 0x0f000009},
  // {WRITE, PHY, 0x31070148, 0x0f090000},// 电压
  {WRITE, PHY, 0x31070149, 0x00000309},
  {WRITE, PHY, 0x310701c4, 0x0e00003c},
  {WRITE, PHY, 0x31070204, 0x0e00003c},
  {WRITE, PHY, 0x31070244, 0x0e00003c},
  {WRITE, PHY, 0x31070284, 0x0e00003c},
  {WRITE, PHY, 0x310702c4, 0x0e00003c},
  {WRITE, PHY, 0x31070304, 0x0e00003c},
  {WRITE, PHY, 0x31070344, 0x0e00003c},
  {WRITE, PHY, 0x31070384, 0x0e00003c},
  {WRITE, PHY, 0x31070080, 0x3000b0c7},
  {WRITE, PHY, 0x3107014b, 0x0fc00172},
  {WRITE, PHY, 0x31070040, 0x2800040c},
  {WRITE, PHY, 0x31070022, 0x20c01884},
  {WRITE, PHY, 0x31070010, 0x5e001810},
  {WRITE, PHY, 0x31070011, 0x008005f0},
  {WRITE, PHY, 0x31070060, 0x00000a31},
  {WRITE, PHY, 0x31070061, 0x00000001},
  {WRITE, PHY, 0x31070062, 0x00000018},
  {WRITE, PHY, 0x31070063, 0x00000200},
  {WRITE, PHY, 0x31070064, 0x00000000},
  {WRITE, PHY, 0x31070065, 0x00000640},
  {WRITE, PHY, 0x31070080, 0x3000b0c7},
  {WRITE, PHY, 0x31070066, 0x00000800},
  // x16
  {WRITE, PHY, 0x31070044, 0x0827100a},
  {WRITE, PHY, 0x31070045, 0x28240408},
  // x8
  // {WRITE, PHY, 0x31070044, 0x0627100a},
  // {WRITE, PHY, 0x31070045, 0x281a0408},
  {WRITE, PHY, 0x31070046, 0x00070380},
  {WRITE, PHY, 0x31070047, 0x02000101},
  // x16
  {WRITE, PHY, 0x31070048, 0x02940608},
  // x8
  // {WRITE, PHY, 0x31070048, 0x01cd0608},
  {WRITE, PHY, 0x31070049, 0x0036100a},
  {WRITE, PHY, 0x31070005, 0x020046a0},
  {WRITE, PHY, 0x31070006, 0x20006c2b},
  {WRITE, PHY, 0x31070007, 0xc0aa0060},
  {WRITE, PHY, 0x3107000b, 0x80040000},
  {WRITE, PHY, 0x31070001, 0x00000073},
  // phy init
  {READ , PHY, 0x3107000d, 0x8000000f},
  // phy SDRAM config
  {WRITE, PHY, 0x31070001, 0x00040001},
  // phy SDRAM init
  {READ , PHY, 0x3107000d, 0x8000001f},
  // mc SDRAM config
  {WRITE, MC,  0x310601b0, 0x00000041},
  {WRITE, MC,  0x31060320, 0x00000001},
  // mc SDRAM init(!!!!!!)
  {READ , MC,  0x31060004, 0x00000001},
  // mc SDRAM config reset
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x31060060, 0x00000001},
  {WRITE, MC,  0x310601b0, 0x00000040},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060010, 0x00001010},
  {WRITE, MC,  0x31060014, 0x00000181},
  {WRITE, MC,  0x31060010, 0x80001010},
  // write leveling config
  {WRITE, PHY, 0x310700c5, 0x000000ff},
  {WRITE, PHY, 0x31070137, 0x00000000},
  {WRITE, PHY, 0x31070026, 0x00010000},
  {WRITE, PHY, 0x31070001, 0x00000201},
 // write leveling training
  {READ , PHY, 0x3107000d, 0x8000003f},
  // write leveling config reset
  {WRITE, PHY, 0x31070137, 0x00000000}, // change!!!
  {WRITE, PHY, 0x31070026, 0x00000001}, // change!!!
  {WRITE, MC,  0x31060010, 0x00001010},
  {WRITE, MC,  0x31060014, 0x00000101},
  {WRITE, MC,  0x31060010, 0x80001010},
  // read & write training config
  {WRITE, PHY, 0x31070001, 0x0002fc01},
  // read & write training
  {READ , PHY, 0x3107000d, 0x80004fff},
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x31060060, 0x00000000},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060490, 0x00000001}
};
#endif

#ifdef DDR4_1600_HP
DDRCmd cmds[] =
{
  // mc config
  {WRITE, MC,  0x31060304, 0x00000001},
  {WRITE, MC,  0x31060000, 0x40040410},
  {WRITE, MC,  0x31060010, 0x00000010},
  {WRITE, MC,  0x31060014, 0x00000000},
  {WRITE, MC,  0x3106001c, 0x00000000},
  {WRITE, MC,  0x31060030, 0x00000080},
  {WRITE, MC,  0x31060038, 0x006a0000},
  {WRITE, MC,  0x31060050, 0x00001070},
  {WRITE, MC,  0x31060060, 0x00000000},
  {WRITE, MC,  0x31060064, 0x0061008C}, // RFSHTMG, tREFI, tRFC
  {WRITE, MC,  0x310600c0, 0x00000000},
  {WRITE, MC,  0x310600c4, 0x00001000},
  {WRITE, MC,  0x310600d0, 0x00020125},
  {WRITE, MC,  0x310600d4, 0x00760000},
  {WRITE, MC,  0x310600dc, 0x02050001}, // MR0, MR1, CL, tWR, tRTP
  {WRITE, MC,  0x310600e0, 0x00000000}, // MR2, MR3, CWL
  {WRITE, MC,  0x310600e4, 0x00100000}, // MR4
  {WRITE, MC,  0x310600e8, 0x00000440}, // MR5
  {WRITE, MC,  0x310600ec, 0x00000400}, // MR6, tCCD_L
  {WRITE, MC,  0x310600f0, 0x00000010},
  {WRITE, MC,  0x31060100, 0x0D0A1A0E}, // DRAMTMG0, CWL, tWR, tFAW, tREFI, tRASmin
  {WRITE, MC,  0x31060104, 0x00030313}, // DRAMTMG1, tXP, CL, tRASmin
  {WRITE, MC,  0x31060108, 0x0505040A}, // DRAMTMG2, CL, CWL, tWTR_L
  {WRITE, MC,  0x3106010c, 0x0000400C}, // DRAMTMG3, tMRD, tMOD
  {WRITE, MC,  0x31060110, 0x05030306}, // DRAMTMG4, CL, tCCD_L, tRRD_L
  {WRITE, MC,  0x31060114, 0x04040302}, // DRAMTMG5, tCKSRX, tCKSRE,  tCKE
  {WRITE, MC,  0x31060120, 0x04040A07}, // DRAMTMG8, tRFC4, tRFC, tDLLK
  {WRITE, MC,  0x31060124, 0x00020208}, // DRAMTMG9, tCCD_S, tRRD_S, CWL, tWTR_S
  {WRITE, MC,  0x31060128, 0x000E0C05}, // DRAMTMG10, tMOD
  {WRITE, MC,  0x3106012c, 0x1107010E}, // DRAMTMG11, tXS, tXSDLL, tMOD, tCPDED
  {WRITE, MC,  0x31060130, 0x0C000008}, // DRAMTMG12, tMOD, tMRD_PDA
  {WRITE, MC,  0x3106013c, 0x00000000},
  {WRITE, MC,  0x31060180, 0x51000040},
  {WRITE, MC,  0x31060184, 0x00000070},
  {WRITE, MC,  0x31060190, 0x04868207}, // DFITMG0, CL, CWL
  {WRITE, MC,  0x31060194, 0x01090606},
  {WRITE, MC,  0x31060198, 0x07008120},
  {WRITE, MC,  0x3106019c, 0x00000061},
  {WRITE, MC,  0x310601a0, 0x20400004},
  {WRITE, MC,  0x310601a4, 0x0003002e},
  {WRITE, MC,  0x310601a8, 0x00000000},
  {WRITE, MC,  0x310601b0, 0x00000041},
  {WRITE, MC,  0x310601b8, 0x00000000},
  {WRITE, MC,  0x310601c0, 0x00000001},
  {WRITE, MC,  0x310601c4, 0x00000000},
  {WRITE, MC,  0x31060204, 0x003f0c0c}, // addr
  {WRITE, MC,  0x31060208, 0x01010100},
  {WRITE, MC,  0x3106020c, 0x01010101},
  {WRITE, MC,  0x31060210, 0x00001f1f},
  {WRITE, MC,  0x31060214, 0x08080505},
  {WRITE, MC,  0x31060218, 0x08080808},
  {WRITE, MC,  0x3106021c, 0x00000f0f},
  {WRITE, MC,  0x31060220, 0x00000a01},
  {WRITE, MC,  0x31060224, 0x02020107},
  {WRITE, MC,  0x31060228, 0x0704080a},
  {WRITE, MC,  0x3106022c, 0x00000007},
  {WRITE, MC, 0x31060240, 0x0608061C},
  {WRITE, MC, 0x31060244, 0x00000010},
  {WRITE, MC,  0x31060250, 0x1aa51704}, // SCHED, w-CAM, r/w sw, page open/close
  {WRITE, MC,  0x31060254, 0x00000039}, // SCHED1, page policy
  {WRITE, MC,  0x3106025c, 0x40001e0b}, // PERFHPR1, hprd starve & length
  {WRITE, MC,  0x31060264, 0x2c006921}, // PERFLPR1, lprd starve & length
  {WRITE, MC,  0x3106026c, 0x79001d5c}, // PERFWR1, wr starve & length
  {WRITE, MC,  0x31060300, 0x00000001}, // DBG0, write combine
  {WRITE, MC,  0x31060304, 0x00000000},
  {WRITE, MC,  0x3106030c, 0x00000000},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060328, 0x00000000},
  {WRITE, MC,  0x3106036c, 0x00000010},
  {WRITE, MC,  0x31060400, 0x00000010},
  {WRITE, MC,  0x31060404, 0x000023e7},
  {WRITE, MC,  0x31060408, 0x0000036e},
  {WRITE, MC,  0x31060490, 0x00000000},
  {WRITE, MC,  0x31060494, 0x00100005},
  // mc reset done
  {READ , MC,  0x31060ff4, 0x67612a2a}, //read only
  // mc init
  {WRITE, MC,  0x31060304, 0x00000000},
  {WRITE, MC,  0x31060030, 0x00000088},
  {WRITE, MC,  0x31060030, 0x00000088},
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x310601b0, 0x00000040},
  // phy config
  {WRITE, PHY, 0x31070024, 0x0020403b},
  {WRITE, PHY, 0x31070148, 0x0f000009},
  // {WRITE, PHY, 0x31070148, 0x0f090000},// 电压
  {WRITE, PHY, 0x31070149, 0x00000309},
  {WRITE, PHY, 0x310701c4, 0x0e00003c},
  {WRITE, PHY, 0x31070204, 0x0e00003c},
  {WRITE, PHY, 0x31070244, 0x0e00003c},
  {WRITE, PHY, 0x31070284, 0x0e00003c},
  {WRITE, PHY, 0x310702c4, 0x0e00003c},
  {WRITE, PHY, 0x31070304, 0x0e00003c},
  {WRITE, PHY, 0x31070344, 0x0e00003c},
  {WRITE, PHY, 0x31070384, 0x0e00003c},
  {WRITE, PHY, 0x31070080, 0x8000b0cf},
  {WRITE, PHY, 0x3107014b, 0x0fc00172},
  {WRITE, PHY, 0x31070040, 0x2800040c},
  {WRITE, PHY, 0x31070022, 0x20c01004},
  {WRITE, PHY, 0x31070010, 0x5e001810},
  {WRITE, PHY, 0x31070011, 0x008005f0},
  {WRITE, PHY, 0x31070060, 0x00000205}, // MR0
  {WRITE, PHY, 0x31070061, 0x00000001}, // MR1
  {WRITE, PHY, 0x31070062, 0x00000000}, // MR2
  {WRITE, PHY, 0x31070063, 0x00000000}, // MR3
  {WRITE, PHY, 0x31070064, 0x00000000}, // MR4
  {WRITE, PHY, 0x31070065, 0x00000440}, // MR5
  {WRITE, PHY, 0x31070066, 0x00000400}, // MR6
  {WRITE, PHY, 0x31070044, 0x051C0A06}, // tRRD, tRAS, tRP, tRTP
  {WRITE, PHY, 0x31070045, 0x28141808}, // tWLMRD, tFAW, tMOD, tMRD
  {WRITE, PHY, 0x31070046, 0x000401B0}, // tCKE, tXS
  {WRITE, PHY, 0x31070047, 0x02550101}, // tDLLK
  {WRITE, PHY, 0x31070048, 0x01A40605}, // tRFC, tXP
  {WRITE, PHY, 0x31070049, 0x00260A06}, // tRC, tRCD, tWTR
  {WRITE, PHY, 0x31070005, 0x020046a0},
  {WRITE, PHY, 0x31070006, 0x20006c2b},
  {WRITE, PHY, 0x31070007, 0xc0aa0060},
  {WRITE, PHY, 0x3107000b, 0x80040000},
  {WRITE, PHY, 0x31070001, 0x00000073},
  // phy init
  {READ , PHY, 0x3107000d, 0x8000000f},
  // phy SDRAM config
  {WRITE, PHY, 0x31070001, 0x00040001},
  // phy SDRAM init
  {READ , PHY, 0x3107000d, 0x8000001f},
  // mc SDRAM config
  {WRITE, MC,  0x310601b0, 0x00000041},
  {WRITE, MC,  0x31060320, 0x00000001},
  // mc SDRAM init(!!!!!!)
  {READ , MC,  0x31060004, 0x00000001},
  // mc SDRAM config reset
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x31060060, 0x00000001},
  {WRITE, MC,  0x310601b0, 0x00000040},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060010, 0x00001010},
  {WRITE, MC,  0x31060014, 0x00000181},
  {WRITE, MC,  0x31060010, 0x80001010},
  // write leveling config
  {WRITE, PHY, 0x310700c5, 0x000000ff},
  {WRITE, PHY, 0x31070137, 0x00000000},
  {WRITE, PHY, 0x31070026, 0x00010000},
  {WRITE, PHY, 0x31070001, 0x00000201},
 // write leveling training
  {READ , PHY, 0x3107000d, 0x8000003f},
  // write leveling config reset
  {WRITE, PHY, 0x31070137, 0x00000000},
  {WRITE, PHY, 0x31070026, 0x00000001},
  {WRITE, MC,  0x31060010, 0x00001010},
  {WRITE, MC,  0x31060014, 0x00000101},
  {WRITE, MC,  0x31060010, 0x80001010},
  // read & write training config
  {WRITE, PHY, 0x31070001, 0x0002fc01},
  // read & write training
  {READ , PHY, 0x3107000d, 0x80004fff},
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x31060060, 0x00000000},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060490, 0x00000001}
};
#endif

#ifdef DDR4_1600
DDRCmd cmds[] =
{
  // mc config
  {WRITE, MC,  0x31060304, 0x00000001},
  {WRITE, MC,  0x31060000, 0x40040410},
  {WRITE, MC,  0x31060010, 0x00000010},
  {WRITE, MC,  0x31060014, 0x00000000},
  {WRITE, MC,  0x3106001c, 0x00000000},
  {WRITE, MC,  0x31060030, 0x00000080},
  {WRITE, MC,  0x31060038, 0x006a0000},
  {WRITE, MC,  0x31060050, 0x00001070},
  {WRITE, MC,  0x31060060, 0x00000000},
  {WRITE, MC,  0x31060064, 0x003000dc},
  {WRITE, MC,  0x310600c0, 0x00000000},
  {WRITE, MC,  0x310600c4, 0x00001010},
  {WRITE, MC,  0x310600d0, 0x000200c4},
  {WRITE, MC,  0x310600d4, 0x004f0007},
  {WRITE, MC,  0x310600dc, 0x02150001},
  {WRITE, MC,  0x310600e0, 0x10100000},
  {WRITE, MC,  0x310600e4, 0x00100000},
  {WRITE, MC,  0x310600e8, 0x00000640},
  {WRITE, MC,  0x310600ec, 0x00000400},
  {WRITE, MC,  0x310600f0, 0x00000010},
  {WRITE, MC,  0x31060100, 0x0e0e0d0e},
  {WRITE, MC,  0x31060104, 0x00030314},
  {WRITE, MC,  0x31060108, 0x0606040b},
  {WRITE, MC,  0x3106010c, 0x0000400c},
  {WRITE, MC,  0x31060110, 0x06030307},
  {WRITE, MC,  0x31060114, 0x04040302},
  {WRITE, MC,  0x31060120, 0x04040a07},
  {WRITE, MC,  0x31060124, 0x00020309},
  {WRITE, MC,  0x31060128, 0x000e0c05},
  {WRITE, MC,  0x3106012c, 0x1106010e},
  {WRITE, MC,  0x31060130, 0x0c000008},
  {WRITE, MC,  0x3106013c, 0x00000000},
  {WRITE, MC,  0x31060180, 0x51000040},
  {WRITE, MC,  0x31060184, 0x00000070},
  // {WRITE, MC,  0x31060190, 0x048b820a},
  {WRITE, MC,  0x31060190, 0x04888209},//20220208
  {WRITE, MC,  0x31060194, 0x01090606},
  {WRITE, MC,  0x31060198, 0x07008120},
  {WRITE, MC,  0x3106019c, 0x00000061},
  {WRITE, MC,  0x310601a0, 0x20400004},
  {WRITE, MC,  0x310601a4, 0x0003002e},
  {WRITE, MC,  0x310601a8, 0x00000000},
  {WRITE, MC,  0x310601b0, 0x00000041},
  {WRITE, MC,  0x310601b8, 0x00000000},
  {WRITE, MC,  0x310601c0, 0x00000001},
  {WRITE, MC,  0x310601c4, 0x00000000},
  {WRITE, MC,  0x31060204, 0x003f1818},
  {WRITE, MC,  0x31060208, 0x00000000},
  {WRITE, MC,  0x3106020c, 0x00000000},
  {WRITE, MC,  0x31060210, 0x00001f1f},
  {WRITE, MC,  0x31060214, 0x04040404},
  {WRITE, MC,  0x31060218, 0x04040404},
  {WRITE, MC,  0x3106021c, 0x00000f0f},
  {WRITE, MC,  0x31060220, 0x00001a1a},
  {WRITE, MC,  0x31060224, 0x02020107},
  {WRITE, MC,  0x31060228, 0x0704080a},
  {WRITE, MC,  0x3106022c, 0x00000007},
  // {WRITE, MC,  0x31060240, 0x0f1e0264},
  // {WRITE, MC,  0x31060244, 0x00000001},

  {WRITE, MC, 0x31060240, 0x060a061c}, // odt 1 change!!!
  {WRITE, MC, 0x31060244, 0x00000010}, // odt 0 change!!!

  {WRITE, MC,  0x31060250, 0x1aa51704},
  {WRITE, MC,  0x31060254, 0x00000039},
  {WRITE, MC,  0x3106025c, 0x40001e0b},
  {WRITE, MC,  0x31060264, 0x2c006921},
  {WRITE, MC,  0x3106026c, 0x79001d5c},
  {WRITE, MC,  0x31060300, 0x00000001},
  {WRITE, MC,  0x31060304, 0x00000000},
  {WRITE, MC,  0x3106030c, 0x00000000},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060328, 0x00000000},
  {WRITE, MC,  0x3106036c, 0x00000010},
  {WRITE, MC,  0x31060400, 0x00000010},
  {WRITE, MC,  0x31060404, 0x000023e7},
  {WRITE, MC,  0x31060408, 0x0000036e},
  {WRITE, MC,  0x31060490, 0x00000000},
  {WRITE, MC,  0x31060494, 0x00100005},
  // mc reset done
  //{READ ,0x31060ff4, 0x00000000},
  {READ , MC,  0x31060ff4, 0x67612a2a}, //read only
  // mc init
  {WRITE, MC,  0x31060304, 0x00000000},
  {WRITE, MC,  0x31060030, 0x00000088},
  {WRITE, MC,  0x31060030, 0x00000088},
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x310601b0, 0x00000040},
  // phy config
  {WRITE, PHY,  0x31070024, 0x0020403b},
  {WRITE, PHY,  0x31070148, 0x0f000009},
  {WRITE, PHY,  0x31070149, 0x00000309},
  {WRITE, PHY,  0x310701C4, 0x0e00003c},
  {WRITE, PHY,  0x31070204, 0x0e00003c},
  {WRITE, PHY,  0x31070244, 0x0e00003c},
  {WRITE, PHY,  0x31070284, 0x0e00003c},
  {WRITE, PHY,  0x310702C4, 0x0e00003c},
  {WRITE, PHY,  0x31070304, 0x0e00003c},
  {WRITE, PHY,  0x31070344, 0x0e00003c},
  {WRITE, PHY,  0x31070384, 0x0e00003c},
  // {WRITE, PHY,  0x31070080, 0x3000b0c7},
  {WRITE, PHY,  0x31070080, 0x8000b0c7}, // 20220208
  {WRITE, PHY,  0x3107014B, 0x0fc00172},
  {WRITE, PHY,  0x31070040, 0x2800040c},
  // {WRITE, PHY,  0x31070022, 0x20c01884},
  {WRITE, PHY, 0x31070022, 0x20c00004}, // dqs resistor !!!
  {WRITE, PHY,  0x31070010, 0x5e001810},
  {WRITE, PHY,  0x31070011, 0x008005f0},
  {WRITE, PHY,  0x31070060, 0x00000215},
  {WRITE, PHY,  0x31070061, 0x00000001},
  {WRITE, PHY,  0x31070062, 0x00000010},
  {WRITE, PHY,  0x31070063, 0x00000000},
  {WRITE, PHY,  0x31070064, 0x00000000},
  {WRITE, PHY,  0x31070065, 0x00000640},
  // {WRITE, PHY,  0x31070080, 0x3000b0c7},
  {WRITE, PHY,  0x31070080, 0x8000b0c7}, // 20220208
  {WRITE, PHY,  0x31070066, 0x00000400},
  {WRITE, PHY,  0x31070044, 0x061c0c06},
  {WRITE, PHY,  0x31070045, 0x281c0008},
  {WRITE, PHY,  0x31070046, 0x00040380},
  {WRITE, PHY,  0x31070047, 0x02000101},
  {WRITE, PHY,  0x31070048, 0x01cd0608},
  {WRITE, PHY,  0x31070049, 0x00280c06},
  {WRITE, PHY,  0x31070005, 0x020046a0},
  {WRITE, PHY,  0x31070006, 0x20006c2b},
  {WRITE, PHY,  0x31070007, 0xc0aa0060},
  {WRITE, PHY,  0x3107000B, 0x80040000},
  {WRITE, PHY,  0x31070001, 0x00000073},
  // phy init
  {READ , PHY, 0x3107000d, 0x8000000f},
  // phy SDRAM config
  {WRITE, PHY, 0x31070001, 0x00040001},
  // phy SDRAM init
  {READ , PHY, 0x3107000d, 0x8000001f},
  // mc SDRAM config
  {WRITE, MC,  0x310601b0, 0x00000041},
  {WRITE, MC,  0x31060320, 0x00000001},
  // mc SDRAM init(!!!!!!)
  {READ , MC,  0x31060004, 0x00000001},
  // mc SDRAM config reset
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x31060060, 0x00000001},
  {WRITE, MC,  0x310601b0, 0x00000040},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060010, 0x00001010},
  {WRITE, MC,  0x31060014, 0x00000181},
  {WRITE, MC,  0x31060010, 0x80001010},
  // write leveling config
  {WRITE, PHY, 0x310700c5, 0x000000ff},
  {WRITE, PHY, 0x31070137, 0x00000000},
  {WRITE, PHY, 0x31070026, 0x00010000},
  {WRITE, PHY, 0x31070001, 0x00000201},
 // write leveling training
  {READ , PHY, 0x3107000d, 0x8000003f},
  // write leveling config reset
  {WRITE, PHY, 0x31070137, 0x00000000}, // change!!!
  {WRITE, PHY, 0x31070026, 0x00000001}, // change!!!
  {WRITE, MC,  0x31060010, 0x00001010},
  {WRITE, MC,  0x31060014, 0x00000101},
  {WRITE, MC,  0x31060010, 0x80001010},
  // read & write training config
  {WRITE, PHY, 0x31070001, 0x0002fc01},
  // read & write training
  {READ , PHY, 0x3107000d, 0x80004fff},
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x31060060, 0x00000000},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060490, 0x00000001}
};
#endif

#if 0
DDRCmd cmds[] = 
{
  // mc config
  {WRITE, MC,  0x31060304, 0x00000001}, // DBG1
  {WRITE, MC,  0x31060030, 0x00000001}, // Self-Ref & Power Down
  {WRITE, MC,  0x31060000, 0x40040610}, // 2T timing
  {WRITE, MC,  0x31060010, 0x00000010}, // MR Control
  {WRITE, MC,  0x31060014, 0x0000465c}, // MR Data
  {WRITE, MC,  0x3106001c, 0xa2e0f445}, // MR PDA
  {WRITE, MC,  0x31060030, 0x00000088}, // Self-Ref & Power Down
  {WRITE, MC,  0x31060034, 0x000f0002}, // Self-Ref & Power Down Timing
  {WRITE, MC,  0x31060038, 0x006a0000}, // Hardware Low Power
  {WRITE, MC,  0x31060050, 0x00001060}, // 
  {WRITE, MC,  0x31060060, 0x00000000},
  {WRITE, MC,  0x31060064, 0x0049014a},
  {WRITE, MC,  0x310600c0, 0x00000000},
  {WRITE, MC,  0x310600c4, 0x00001010},
  {WRITE, MC,  0x310600d0, 0x00020027},
  {WRITE, MC,  0x310600d4, 0x00270006},
  {WRITE, MC,  0x310600dc, 0x0a310001},
  {WRITE, MC,  0x310600e0, 0x10180200},
  {WRITE, MC,  0x310600e4, 0x00110000},
  {WRITE, MC,  0x310600e8, 0x00000640},
  {WRITE, MC,  0x310600ec, 0x00000800},
  {WRITE, MC,  0x310600f0, 0x00000010},

    {WRITE, MC,  0x31060100, 0x11122914},
  //{WRITE, MC,  0x31060100, 0x110d2914}, // rw2pre, t_faw, t_rax_max, t_ras_min

  {WRITE, MC,  0x31060104, 0x0004051b}, // null, t_xp, rd2pre, t_rc
  {WRITE, MC,  0x31060108, 0x06080510}, // wl, rl, 
  {WRITE, MC,  0x3106010c, 0x0000f00f},

    {WRITE, MC,  0x31060110, 0x08030409},
  //{WRITE, MC,  0x31060110, 0x08030309},

  {WRITE, MC,  0x31060114, 0x0f0e0404},
  {WRITE, MC,  0x31060120, 0x06060e08},

    {WRITE, MC,  0x31060124, 0x0002040d},
  //{WRITE, MC,  0x31060124, 0x0002020d},

  {WRITE, MC,  0x31060128, 0x000f0e05},
  {WRITE, MC,  0x3106012c, 0x14090112},
  {WRITE, MC,  0x31060130, 0x0c00000d},
  {WRITE, MC,  0x3106013c, 0x00000000},
  {WRITE, MC,  0x31060180, 0x51000040},
  {WRITE, MC,  0x31060184, 0x00000070},
  {WRITE, MC,  0x31060190, 0x048b820a},
  {WRITE, MC,  0x31060194, 0x01090606},
  {WRITE, MC,  0x31060198, 0x07008120},
  {WRITE, MC,  0x3106019c, 0x00000061},
  {WRITE, MC,  0x310601a0, 0x20400004},
  {WRITE, MC,  0x310601a4, 0x0003002e},
  {WRITE, MC,  0x310601a8, 0x00000000},
  {WRITE, MC,  0x310601b0, 0x00000041},
  {WRITE, MC,  0x310601b8, 0x00000000},
  {WRITE, MC,  0x310601c0, 0x00000001},
  {WRITE, MC,  0x310601c4, 0x00000000},

  {WRITE, MC,  0x31060204, 0x003f1818},
  {WRITE, MC,  0x31060208, 0x00000000},
  {WRITE, MC,  0x3106020c, 0x00000000},
  {WRITE, MC,  0x31060210, 0x00001f1f},
  {WRITE, MC,  0x31060214, 0x04040404},
  {WRITE, MC,  0x31060218, 0x04040404},
  {WRITE, MC,  0x3106021c, 0x00000f0f},
  {WRITE, MC,  0x31060220, 0x00001a1a},

  {WRITE, MC,  0x31060224, 0x02020107},
  {WRITE, MC,  0x31060228, 0x0704080a},
  {WRITE, MC,  0x3106022c, 0x00000007},
  {WRITE, MC,  0x31060240, 0x0f1e0264},
  {WRITE, MC,  0x31060244, 0x00000001},
  {WRITE, MC,  0x31060250, 0x1aa51704},
  {WRITE, MC,  0x31060254, 0x00000039},
  {WRITE, MC,  0x3106025c, 0x40001e0b},
  {WRITE, MC,  0x31060264, 0x2c006921},
  {WRITE, MC,  0x3106026c, 0x79001d5c},
  {WRITE, MC,  0x31060300, 0x00000001},
  {WRITE, MC,  0x31060304, 0x00000000},
  {WRITE, MC,  0x3106030c, 0x00000000},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060328, 0x00000000},
  {WRITE, MC,  0x3106036c, 0x00000010},
  {WRITE, MC,  0x31060400, 0x00000010},
  {WRITE, MC,  0x31060404, 0x000023e7},
  {WRITE, MC,  0x31060408, 0x0000036e},
  {WRITE, MC,  0x31060490, 0x00000000},
  {WRITE, MC,  0x31060494, 0x00100005},
  // mc reset done
  //{READ ,0x31060ff4, 0x00000000},
  {READ , MC,  0x31060ff4, 0x67612a2a}, //read only
  // mc init
  {WRITE, MC,  0x31060304, 0x00000000},
  {WRITE, MC,  0x31060030, 0x00000088},
  {WRITE, MC,  0x31060030, 0x00000088},
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x310601b0, 0x00000040},
  // phy config
  {WRITE, PHY, 0x31070024, 0x0020403b},
  {WRITE, PHY, 0x31070148, 0x0f000009},
  {WRITE, PHY, 0x31070149, 0x00000309},
  {WRITE, PHY, 0x310701c4, 0x0e00003c},
  {WRITE, PHY, 0x31070204, 0x0e00003c},
  {WRITE, PHY, 0x31070244, 0x0e00003c},
  {WRITE, PHY, 0x31070284, 0x0e00003c},
  {WRITE, PHY, 0x310702c4, 0x0e00003c},
  {WRITE, PHY, 0x31070304, 0x0e00003c},
  {WRITE, PHY, 0x31070344, 0x0e00003c},
  {WRITE, PHY, 0x31070384, 0x0e00003c},
  {WRITE, PHY, 0x31070080, 0x3000b0c7},
  {WRITE, PHY, 0x3107014b, 0x0fc00172},
  {WRITE, PHY, 0x31070040, 0x2800040c},
  {WRITE, PHY, 0x31070022, 0x20c01884},
  {WRITE, PHY, 0x31070010, 0x5e001810},
  {WRITE, PHY, 0x31070011, 0x008005f0},
  {WRITE, PHY, 0x31070060, 0x00000a31},
  {WRITE, PHY, 0x31070061, 0x00000001},
  {WRITE, PHY, 0x31070062, 0x00000018},
  {WRITE, PHY, 0x31070063, 0x00000200},
  {WRITE, PHY, 0x31070064, 0x00000000},
  {WRITE, PHY, 0x31070065, 0x00000640},
  {WRITE, PHY, 0x31070080, 0x3000b0c7},
  {WRITE, PHY, 0x31070066, 0x00000800},
  {WRITE, PHY, 0x31070044, 0x0827100a},
  {WRITE, PHY, 0x31070045, 0x28240408},
  {WRITE, PHY, 0x31070046, 0x00070380},
  {WRITE, PHY, 0x31070047, 0x02000101},
  {WRITE, PHY, 0x31070048, 0x02940608},
  {WRITE, PHY, 0x31070049, 0x0036100a},
  {WRITE, PHY, 0x31070005, 0x020046a0},
  {WRITE, PHY, 0x31070006, 0x20006c2b},
  {WRITE, PHY, 0x31070007, 0xc0aa0060},
  {WRITE, PHY, 0x3107000b, 0x80040000},
  {WRITE, PHY, 0x31070001, 0x00000073},
  // phy init
  {READ , PHY, 0x3107000d, 0x8000000f},
  // phy SDRAM config
  {WRITE, PHY, 0x31070001, 0x00040001},
  // phy SDRAM init
  {READ , PHY, 0x3107000d, 0x8000001f},
  // mc SDRAM config
  {WRITE, MC,  0x310601b0, 0x00000041},
  {WRITE, MC,  0x31060320, 0x00000001},
  // mc SDRAM init(!!!!!!)
  {READ , MC,  0x31060004, 0x00000001},
  // mc SDRAM config reset
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x31060060, 0x00000001},
  {WRITE, MC,  0x310601b0, 0x00000040},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060010, 0x00001010},
  {WRITE, MC,  0x31060014, 0x00000181},
  {WRITE, MC,  0x31060010, 0x80001010},
  // write leveling config
  {WRITE, PHY, 0x310700c5, 0x000000ff},
  {WRITE, PHY, 0x31070137, 0x00000000},
  {WRITE, PHY, 0x31070026, 0x00010000},
  {WRITE, PHY, 0x31070001, 0x00000201},
 // write leveling training
  {READ , PHY, 0x3107000d, 0x8000003f},
  // write leveling config reset
  {WRITE, MC,  0x31060010, 0x00001010},
  {WRITE, MC,  0x31060014, 0x00000101},
  {WRITE, MC,  0x31060010, 0x80001010},
  // read & write training config
  {WRITE, PHY, 0x31070001, 0x0002fc01},
  // read & write training
  {READ , PHY, 0x3107000d, 0x80004fff},
  {WRITE, MC,  0x31060320, 0x00000000},
  {WRITE, MC,  0x31060060, 0x00000000},
  {WRITE, MC,  0x31060320, 0x00000001},
  {WRITE, MC,  0x31060490, 0x00000001}
};
#endif

void initDDR(void)
{
    register int i asm ("t0");
    register int j;
    uint32_t rdata;
    for (i = 0; i < sizeof(cmds) / sizeof(DDRCmd); i++) {
        register DDRCmd *p asm ("t1") = &cmds[i];
        uint64_t addrValue = p->addr;
        if(p->mcPhy==PHY)
          addrValue = (addrValue & 0xffff0000) | (((addrValue & 0x0000ffff) << 2) & 0x0000ffff);
        volatile uint32_t *addr = (uint32_t *)(addrValue);
        if (p->rw == READ) {
            rdata = *addr;
            while(rdata != p->data) // LSR = 0x1, data ready
            {
              rdata = *addr;
              // if (p->addr = 0x3107000d) {
              //     PGSR0 *p = (PGSR0 *)&rdata;
              //     if (p->WEERR) {
              //         hex_log("pgsr0 werr\n\r");
              //     }
              // }
            }
        }
        else {
            *addr = p->data;
        }
    }

    //write_csr(0x800, 1);
}


/**
 * Test data rw on ddr region
 */
#define DDR_START 0x80008000ULL
// region size = 2GB / ???
#define DDR_SIZE  0x80000000ULL
// unit size defined by MB
#define UNIT_SIZE 128

#define xxx(x) (x | (x & 0xffffffffULL) << 32ULL)

uint64_t hash(uint64_t next){
  next=xxx(next)*1103515245+12345;
  return ((uint64_t)(next/239453));
}

static inline void partial_test_DDR(uint64_t base, uint64_t size)
{
    // size should be larger than 512B (0x200)
    register uint64_t *start = (uint64_t *)base;
    register uint64_t *end = (uint64_t *)(base + size);
    while (start < end) {
        // ver 1
        //*start = (uint64_t)start;
        // ver 2
        *start = hash((uint64_t)start);
        start++;
        // print step per 512B (8 iterations)
        if ((((uint64_t)start) & 0x1fffff) == 0) {
            writechar('*');
        }
    }

    newline();
    writechar('-');
    newline();

    start = (uint64_t *)base;
    while (start < end) {
        uint64_t value = *start;
        uint64_t ref = hash((uint64_t)start);
        if (value != ref) {
            writechar('x');
            newline();
            htoa(ref);
            newline();
            htoa(value);
            newline();
        }

        if ((((uint64_t)start) & 0x1fffff) == 0) {
            writechar('#');
        }

        start++;
    }

    newline();
    writechar('=');
    newline();
}

void testDDR(void)
{
    register const uint64_t unit_size = UNIT_SIZE * 1024 * 1024;  // `UNIT_SIZE` MB
    register uint64_t base = DDR_START;

    newline();

    while (base < (DDR_START + DDR_SIZE)) {
        partial_test_DDR(base, unit_size);
        base += unit_size;
    }

    newline();
    writechar('f');
}

void report_alive(void)
{
    writechar('*');
}

#define CMD_READ 0
#define CMD_WRITE 1
#define CMD_QUIT 2
#define CMD_NOTHING 3

static inline uint64_t atoh(const char *p, int len, int *err)
{
    int cnt = 0;
    uint64_t value = 0;

    while (cnt < len) {
        char ch = *p++;
        uint64_t digit = 0;
        if (ch >= '0' && ch <= '9') {
            digit = ch - '0';
        }
        else if (ch >= 'a' && ch <= 'f') {
            digit = (ch - 'a') + 10;
        }
        else {
            *err = -1;
            return 0;
        }
        value = (value << 4) | digit;
        cnt++;
    }

    *err = 0;

    return value;
}


static char pll_cfg_cmd[128] = {};

static inline int handle_cmd()
{
    char *p = &pll_cfg_cmd[0];

    // r,w,q
    char cmd = *p++;

    writestr("cmd_s: ");
    writestr(pll_cfg_cmd);
    writestr("\r\n");

    if (cmd == 'q') {
        return CMD_QUIT;
    }

    if (cmd != 'r' && cmd != 'w') {
        htoa(*(uint64_t *)pll_cfg_cmd);
        return CMD_NOTHING;
    }

    // delimiter
    if (*p++ != ' ') {
        writestr("bad cmd format\r\n");
        htoa(*(uint64_t *)pll_cfg_cmd);
        return CMD_NOTHING;
    }

    // addr
    int err;
    uint64_t addr = atoh(p, 8, &err);
    if (err != 0) {
        writestr("bad addr format\r\n");
        htoa(*(uint64_t *)(pll_cfg_cmd + 2));
    }
    p += 8;

    uint32_t data;
    // data (optional)
    if (cmd == 'w') {
        if (*p++ != ' ') {
            writestr("bad cmd format 2\r\n");
            htoa(*(uint64_t *)(pll_cfg_cmd + 6));
            return CMD_NOTHING;
        }


        data = (uint32_t)atoh(p, 8, &err);
        if (err != 0) {
            writestr("bad data format\r\n");
            htoa(*(uint64_t *)(pll_cfg_cmd + 10));
            return CMD_NOTHING;
        }
    }

    // Exec
    switch (cmd) {
    case 'w':
        *(uint32_t *)addr = data;
        // fall to read back
    case 'r':
        data  = *(uint32_t *)addr;
        htoa(data);
        newline();
        break;
    }
    
    return CMD_NOTHING;
}


void pll_cfg_console(void)
{
    newline();
    int i = 0;
    char ch;

    writestr("pll> ");
    
    while (1) {
       ch = readchar();
       if (ch == 0xff) {
           continue;
       }

       if (ch == '\r') {
           writestr("\r\n");
           int ret = handle_cmd();
           i = 0;
           for (int j = 0; j < sizeof(pll_cfg_cmd); j++)
               pll_cfg_cmd[j] = '\0';
           if (ret == CMD_QUIT) {
               return;
           }
           writestr("pll> ");
       }
       else {
           writechar(ch);
           pll_cfg_cmd[i++] = ch;
       }
    }
}
