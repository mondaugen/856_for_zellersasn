#! /bin/bash
# call from root of repository using sh scripts/test_switch.sh

if [ -z "$1" ]
then
    LED_INDEX=-1
else
    LED_INDEX=$1
fi

echo "LED_INDEX = $LED_INDEX"


# make code to source in gdb to get switch register addresses and pin numbers
arm-none-eabi-gcc -D${BOARD_VERSION} -DSTM32F429_439xx -Iinc -I../../archives/CMSIS/Include/ -E \
    src/leds.c  | perl -n scripts/get_led_port_addrs.pl   > \
    /tmp/switch_regchk_leds.XXX;

# get register addresses by looking at symbol addresses with gdb
arm-none-eabi-gdb --command scripts/get_led_addrs.gdb;

# check switch states using calculated addresses and openocd
OCD='sudo openocd \
    -f /usr/local/share/openocd/scripts/board/stm32f429discovery.cfg \
    -f '$OPENOCD_INTERFACE;

eval "$OCD" "-c 'set led_index $LED_INDEX'" "-f scripts/test_led.tcl";

