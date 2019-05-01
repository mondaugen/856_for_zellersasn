# set 0 <= led_index < 4 to test particular led, otherwise, will test all leds

puts [format "led_index = %d" $led_index]
puts [format "led_index < 0 ? %d" [expr $led_index < 0]]

signal handle SIGINT

# Initialize openocd
init

# Halt target
halt

array set swreg [source /tmp/leds_vals_addr.XXX]
    
set regnums [list 1 3 5 7]

if {[expr $led_index < 0]} then {
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
} else {
    set i [lindex $regnums $led_index ]
    set reg_ $swreg([format "led%d_reg" $i])
    set pin_ $swreg([format "led%d_pin" $i])
    mem2array x 32 $reg_ 1
    #    puts $x(0)
    set x(0) [expr {($x(0) | (1 << $pin_))}]
    #    puts $x(0)
    array2mem x 32 $reg_ 1

    puts [format "Check if LED %d is illuminated" $led_index]
}

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

