import os
import sys
import shutil

# (filelist, arguments) information for each benchmark
# filelist[0] should always be the binary file
mybench_info = {
  "xapian": (
  ),
  "img-dnn": (
  ),
  "moses": (
  ),
  "specjbb": (
  ),
}

if __name__ == "__main__":
  all_gaps = list(mybench_info.keys())
  for gap in all_gaps:
    tmp_gap = [gap]
    cmd = r'sed -i "25s/\S*\";/{}\";/" /nfs/home/zhangchuanqi/lvna/for_xs/xs-env/sw-sdcard/riscv-pk/dts/noop.dtsi'
    print(cmd.format(gap))
    os.system(cmd.format(gap))
    os.system("make clean -C /nfs/home/zhangchuanqi/lvna/for_xs/xs-env/sw-sdcard/riscv-pk")
    os.system("make -j 16 -C /nfs/home/zhangchuanqi/lvna/for_xs/xs-env/sw-sdcard/riscv-pk")
    os.makedirs(f"/nfs/home/share/zhangchuanqi/cpt_bins/tailbench-withsd/", exist_ok=True)
    shutil.copy("/nfs/home/zhangchuanqi/lvna/for_xs/xs-env/sw-sdcard/riscv-pk/build/bbl.bin",
                 f"/nfs/home/share/zhangchuanqi/cpt_bins/tailbench-withsd/{gap}.bin")
