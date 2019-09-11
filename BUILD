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
