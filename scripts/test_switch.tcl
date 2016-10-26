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

foreach i $regnums {

    set top_reg_ $swreg([format "sw%d_top_reg" $i])
    set btm_reg_ $swreg([format "sw%d_btm_reg" $i])
    set top_pin_ $swreg([format "sw%d_top_pin" $i])
    set btm_pin_ $swreg([format "sw%d_btm_pin" $i])

    puts [format "Put switch %d in top position" $i]
    puts "Press enter to test."
    gets stdin

    mem2array x 32 $btm_reg_ 1
    if {[expr ($x(0) & (1 << $btm_pin_)) >> $btm_pin_] == 0} {
        puts "passed"
    } else {
        puts "failed"
    }
    puts [format "%d" [expr ($x(0) & (1 << $btm_pin_)) >> $btm_pin_]]

    puts [format "Put switch %d in bottom position" $i]
    puts "Press enter to test."
    gets stdin

    mem2array y 32 $top_reg_ 1
    if {[expr ($y(0) & (1 << $top_pin_)) >> $top_pin_] == 0} {
        puts "passed"
    } else {
        puts "failed"
    }
    puts [format "%d" [expr ($y(0) & (1 << $top_pin_)) >> $top_pin_]]

}

shutdown
