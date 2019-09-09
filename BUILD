Now all the external libraries are tracked using git-submodules.
When you first clone this repository, you have to do:

git submodule init
git submodule update

The libraries will be cloned and checked out to the correct commits.

Now you can build. You have to specify the version of the board, e.g.,

BOARD_VERSION=BOARD_V2 make

and the firmware should be compiled and linked.

Use

BOARD_VERSION=BOARD_V2 make flash

to flash chips.

To make a DFU, run

BOARD_VERSION=BOARD_V2 make binary_release

Flash DFU (with dfu-util) with

sudo dfu-util -D /tmp/main-stripped.bin -s 0x08000000 -a 0
