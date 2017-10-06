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

Notes for BOARD_V2 05-Oct-17
build with the following command (remove AUDIO_HW_TEST_OUTPUT if you don't just
want to pass a bitstream of 0xff000000,0xff000000 to the codec)
 CFLAGS=-DBOARD_V2\ -DCODEC_WM8778\ -DAUDIO_HW_TEST_OUTPUT\
-DCODEC_ANALOG_DIGITAL_MIX make
