signal handle SIGINT

# Initialize openocd
init

# Halt target
halt

array set swreg [source /tmp/leds_vals_addr.XXX]
    
set regnums [list 1 3 5 7]

#Set leds
foreach i $regnums {

    set reg_ $swreg([format "led%d_reg" $i])
    set pin_ $swreg([format "led%d_pin" $i])
    mem2array x 32 $reg_ 1
#    puts $x(0)
    set x(0) [expr {($x(0) | (1 << $pin_))}]
#    puts $x(0)
    array2mem x 32 $reg_ 1

}

# Wait for user to acknowledge LEDs

puts "Check if LEDs are illuminated."

while {[catch -signal { }] == 0} {
    sleep 1
}

#reset leds
foreach i $regnums {

    set reg_ $swreg([format "led%d_reg" $i])
    set pin_ $swreg([format "led%d_pin" $i])
    mem2array x 32 $reg_ 1
#    puts $x(0)
    set x(0) [expr {($x(0) & ~(1 << $pin_))}]
#    puts $x(0)
    array2mem x 32 $reg_ 1

}

# Resume target
resume

shutdown

