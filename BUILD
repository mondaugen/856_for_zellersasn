Requirements:

mmmidi v0.1
mm_dsp v0.4
mm_primitives v0.1
ne_datastructures v0.1
mm_dsp_schablone v0.1
CMSIS v4.00

Put directories of first 5 in directory above this one.
Put CMSIS in directory ../../build

Build all libraries except for CMSIS (doesn't need to be built).
Do

make

in this directory and it should build. Do

make flash

to flash chips.

Probably will have to pass defines like so:
CFLAGS=-DBOARD_V2\ -DCODEC_WM8778\ -DAUDIO_HW_TEST_OUTPUT\ -DCODEC_ANALOG_DIGITAL_MIX make

To make a DFU that will work, you have to strip the debugging symbols:

arm-none-eabi-strip -g -O binary main.elf -o /tmp/main-stripped.bin
