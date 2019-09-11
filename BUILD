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

Flash DFU (on Linux) with

sudo dfu-util -D /tmp/main-stripped.bin -s 0x08000000 -a 0

Problems you might encounter when flashing with dfu-util:

On some systems, sudo replaces the PATH environment variable so dfu-util might no longer be found (say, if it was in /usr/local/). To get around this you can do
$ su
(enter password)
# dfu-util -D /tmp/main-stripped.bin -s 0x08000000 -a 0

Note that if you use 'make dfu_flash' instead, you might also have to export the
PATH to your version of arm-none-eabi-gcc and related binaries

e.g.,
# export PATH="/path/to/them/gcc-arm-none-eabi-8-2019-q3-update/bin/:${PATH}"

Note that in this case $HOME might not be set to the $HOME path of your user, so
you might have to explictly type /home/username/ instead of $HOME if that is
usually in the path to the binaries.
