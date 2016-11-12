# display and check status of all switches each iteration
# failure happens when any other knob has been switched other than the desired
# knob.
init
array set swreg [list \
    {sw1_btm_reg} {0x40021810} \
    {sw2_btm_reg} {0x40020c10} \
    {sw3_btm_reg} {0x40020410} \
    {sw4_btm_reg} {0x40020410} \
    {sw5_btm_reg} {0x40020c10} \
    {sw6_btm_reg} {0x40020c10} \
    {sw7_btm_reg} {0x40020810} \
    {sw8_btm_reg} {0x40020810} \
    \
    {sw1_top_reg} {0x40021810} \
    {sw2_top_reg} {0x40020c10} \
    {sw3_top_reg} {0x40020c10} \
    {sw4_top_reg} {0x40020410} \
    {sw5_top_reg} {0x40020410} \
    {sw6_top_reg} {0x40020c10} \
    {sw7_top_reg} {0x40020c10} \
    {sw8_top_reg} {0x40021810} \
    \
    {sw1_btm_pin} {0x3} \
    {sw2_btm_pin} {0xd} \
    {sw3_btm_pin} {0x0} \
    {sw4_btm_pin} {0x7} \
    {sw5_btm_pin} {0x7} \
    {sw6_btm_pin} {0x2} \
    {sw7_btm_pin} {0x8} \
    {sw8_btm_pin} {0x6} \
    \
    {sw1_top_pin} {0x2} \
    {sw2_top_pin} {0xc} \
    {sw3_top_pin} {0xb} \
    {sw4_top_pin} {0x1} \
    {sw5_top_pin} {0x8} \
    {sw6_top_pin} {0x4} \
    {sw7_top_pin} {0x3} \
    {sw8_top_pin} {0x9} \
    ]

set regnums [range 1 9 1]
set reg_phys_map [list 4 0 6 1 5 2 7 3]
set rslt_btm [list 1 2 3 4 5 6 7 8]
set rslt_top [list 1 2 3 4 5 6 7 8]

foreach i $regnums {
    
    set top_reg_ $swreg([format "sw%d_top_reg" $i])
    set btm_reg_ $swreg([format "sw%d_btm_reg" $i])
    set top_pin_ $swreg([format "sw%d_top_pin" $i])
    set btm_pin_ $swreg([format "sw%d_btm_pin" $i])
    set phs_map [lindex $reg_phys_map [expr {$i - 1}]]
    puts [format "%d" $phs_map]
    mem2array x 32 $btm_reg_ 1
    lset rslt_btm $phs_map [expr {($x(0) & (1 << $btm_pin_)) >> $btm_pin_}]
    mem2array x 32 $top_reg_ 1
    lset rslt_top $phs_map [expr {($x(0) & (1 << $top_pin_)) >> $top_pin_}]
    
    #set top_reg_ $swreg([format "sw%d_top_reg" $i])
    #set btm_reg_ $swreg([format "sw%d_btm_reg" $i])
    #set top_pin_ $swreg([format "sw%d_top_pin" $i])
    #set btm_pin_ $swreg([format "sw%d_btm_pin" $i])

    #puts [format "Put switch %d in top position" $i]
    #puts "Press enter to test."
    #gets stdin

    #set passed 1

    #mem2array x 32 $btm_reg_ 1
    #if {[expr ($x(0) & (1 << $btm_pin_)) >> $btm_pin_] != 0} {
    #    set passed 0
    #}

    #foreach j $regnums {
    #    set top_reg_ $swreg([format "sw%d_top_reg" $j])
    #    set top_pin_ $swreg([format "sw%d_top_pin" $j])
    #    mem2array x 32 $top_reg_ 1
    #    if {[expr ($x(0) & (1 << $top_pin_)) >> $top_pin_] == 0} {
    #        set passed 0
    #    }
    #    if ([expr $j != $i ]) {
    #        set btm_reg_ $swreg([format "sw%d_btm_reg" $j])
    #        set btm_pin_ $swreg([format "sw%d_btm_pin" $j])
    #        mem2array x 32 $btm_reg_ 1
    #        if {[expr ($x(0) & (1 << $btm_pin_)) >> $btm_pin_] == 0} {
    #            set passed 0
    #        }
    #    }
    #}
    #
    #if ([expr $passed != 1]) {
    #    puts "Failed"
    #} else {
    #    puts "Passed"
    #}

    #puts [format "Put switch %d in bottom position" $i]
    #puts "Press enter to test."
    #gets stdin

    #set passed 1

    #mem2array x 32 $top_reg_ 1
    #if {[expr ($x(0) & (1 << $top_pin_)) >> $top_pin_] != 0} {
    #    set passed 0
    #}

    #foreach j $regnums {
    #    if ([expr $j != $i ]) {
    #        set top_reg_ $swreg([format "sw%d_top_reg" $j])
    #        set top_pin_ $swreg([format "sw%d_top_pin" $j])
    #        mem2array x 32 $top_reg_ 1
    #        if {[expr ($x(0) & (1 << $top_pin_)) >> $top_pin_] == 0} {
    #            set passed 0
    #        }
    #    }
    #    set btm_reg_ $swreg([format "sw%d_btm_reg" $j])
    #    set btm_pin_ $swreg([format "sw%d_btm_pin" $j])
    #    mem2array x 32 $btm_reg_ 1
    #    if {[expr ($x(0) & (1 << $btm_pin_)) >> $btm_pin_] == 0} {
    #        set passed 0
    #    }
    #}

    #if ([expr $passed != 1]) {
    #    puts "Failed"
    #} else {
    #    puts "Passed"
    #}

}
puts [format " %d %d %d %d" {*}[lrange $rslt_btm 0 3]]
puts [format " %d %d %d %d" {*}[lrange $rslt_top 0 3]]
puts [format "%d %d %d %d" {*}[lrange $rslt_btm 4 7]]
puts [format "%d %d %d %d" {*}[lrange $rslt_top 4 7]]

shutdown
