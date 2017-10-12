#!/bin/bash
make clean
CFLAGS='-DBOARD_V2 -DCODEC_WM8778 -DCODEC_ANALOG_DIGITAL_MIX -DRAM_INTEGRITY_TEST2' make
arm-none-eabi-gdb --command scripts/gdb-load-bin.script --command scripts/dump_ram_test.gdb
python -c 'import numpy as np
import matplotlib.pyplot as plt
x=np.fromfile("/tmp/ram.u32","uint32")
plt.plot(np.arange(x.shape[0]),x)
plt.show()'

