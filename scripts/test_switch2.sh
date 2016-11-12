#!/bin/bash -v
OCD='sudo openocd \
    -f /usr/local/share/openocd/scripts/board/stm32f429discovery.cfg \
    -f interface/stlink-v2-1.cfg';

eval "$OCD" "-f scripts/test_switch2.tcl"
