#!/bin/bash

[ -z $OPENOCD_BOARD ] && OPENOCD_BOARD=/usr/local/share/openocd/scripts/board/stm32f429discovery.cfg
[ -z $CMSIS_INC ] && CMSIS_INC=../../archives/CMSIS/Include/

# call from root of repository using sh scripts/test_switch3.sh
# make code to source in gdb to get switch register addresses and pin numbers
arm-none-eabi-gcc -D${BOARD_VERSION} -DSTM32F429_439xx -Iinc "-I$CMSIS_INC" -E \
    src/switches.c  | perl -n scripts/get_switch_port_addrs.pl   > \
    /tmp/switch_regchk.XXX;

# get register addresses by looking at symbol addresses with gdb
arm-none-eabi-gdb --command scripts/get_switch_addrs.gdb;

# check switch states using calculated addresses and openocd
OCD='sudo openocd \
    -f '$OPENOCD_BOARD' \
    -f '$OPENOCD_INTERFACE

eval "$OCD" "-f scripts/test_switch.tcl"
