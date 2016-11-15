init
array set swreg [source /tmp/switch_vals_addr.XXX]

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

}

puts [format " %d %d %d %d" {*}[lrange $rslt_btm 0 3]]
puts [format " %d %d %d %d" {*}[lrange $rslt_top 0 3]]
puts [format "%d %d %d %d" {*}[lrange $rslt_btm 4 7]]
puts [format "%d %d %d %d" {*}[lrange $rslt_top 4 7]]

shutdown
