#!/bin/bash -v

# Get addresses and sizes of memory regions where ADC values go
# This is formatted addr size_one_item_bytes n_items
arm-none-eabi-gdb --command scripts/get_adc_addrs.gdb

# Append to this file the number of channels per ADC which will give the number
# of entries before getting another reading of the same channel
cat inc/adc.h| grep "#define NUM_CHANNELS_PER_ADC" | awk '{ print $3 }' \
    >> /tmp/adc_vals_addr

OCD='sudo openocd \
    -f /usr/local/share/openocd/scripts/board/stm32f429discovery.cfg \
    -f '$OPENOCD_INTERFACE;

eval "$OCD" "-f scripts/test_pot.tcl"
