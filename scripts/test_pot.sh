#!/bin/bash

[ -z $OPENOCD_BOARD ] && OPENOCD_BOARD=/usr/local/share/openocd/scripts/board/stm32f429discovery.cfg

rm -f /tmp/adc_vals_addr

# Get addresses and sizes of memory regions where ADC values go
# This is formatted addr size_one_item_bytes n_items
arm-none-eabi-gdb --command scripts/get_adc_addrs.gdb

if [[ $BOARD_VERSION = BOARD_V1 || $BOARD_VERSION = "BOARD_V1.1" ]]
then
    # Append to this file the number of channels per ADC which will give the number
    # of entries before getting another reading of the same channel
    cat inc/adc.h| grep "#define *NUM_CHANNELS_PER_ADC" | awk '{ print $3 }' \
        >> /tmp/adc_vals_addr
elif [ $BOARD_VERSION = BOARD_V2 ]
then
    # Append to this file the number of channels per ADC which will give the number
    # of entries before getting another reading of the same channel
    cat inc/adc.h| grep "#define *NUM_CHANNELS_ADC1" | awk '{ print $3 }' \
        >> /tmp/adc_vals_addr
    # Append to this file the number of channels per ADC which will give the number
    # of entries before getting another reading of the same channel
    cat inc/adc.h| grep "#define *NUM_CHANNELS_ADC3" | awk '{ print $3 }' \
        >> /tmp/adc_vals_addr
else
    echo "Please define board version."
    exit -1
fi

OCD='sudo openocd \
    -f '$OPENOCD_BOARD' \
    -f '$OPENOCD_INTERFACE

# board_version set here because I can't seem to read environment variables from
# within openocd scripting
eval "$OCD" "-c 'set board_version $BOARD_VERSION'" "-f scripts/test_pot.tcl"
