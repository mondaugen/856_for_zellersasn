init
signal handle SIGINT

# clear screen
puts -nonewline "\033\x5b2J"

while {[catch -signal {
    array set swreg [source /tmp/switch_vals_addr.XXX]
    
    set regnums [range 1 11 1]
    set reg_phys_map [list 4 0 6 1 5 2 7 3]
    set rslt_btm [list 1 2 3 4 5 6 7 8]
    set rslt_top [list 1 2 3 4 5 6 7 8]
    set fsw_regnums [range 1 3]
    set rslt_fsw [range 1 3]
    
    foreach i [lrange $regnums 0 7] {
    
        set top_reg_ $swreg([format "sw%d_top_reg" $i])
        set btm_reg_ $swreg([format "sw%d_btm_reg" $i])
        set top_pin_ $swreg([format "sw%d_top_pin" $i])
        set btm_pin_ $swreg([format "sw%d_btm_pin" $i])
        set phs_map [lindex $reg_phys_map [expr {$i - 1}]]
        #    puts [format "%d" $phs_map]
        mem2array x 32 $btm_reg_ 1
        lset rslt_btm $phs_map [expr {($x(0) & (1 << $btm_pin_)) >> $btm_pin_}]
        mem2array x 32 $top_reg_ 1
        lset rslt_top $phs_map [expr {($x(0) & (1 << $top_pin_)) >> $top_pin_}]
    
    }
    
    foreach i $fsw_regnums {
        set top_reg_ $swreg([format "fsw%d_reg" $i])
        set top_pin_ $swreg([format "fsw%d_pin" $i])
        mem2array x 32 $top_reg_ 1
        lset rslt_fsw [expr {$i-1}] \
        [expr {($x(0) & (1 << $top_pin_)) >> $top_pin_}]
    
    }

    # move cursor to 1,1 position (top left-hand corner)
    puts -nonewline "\033\x5b1;1H"
    
    set outstr ""
    
    append outstr [format " %d %d %d %d\n" {*}[lrange $rslt_btm 0 3]]
    append outstr [format " %d %d %d %d\n" {*}[lrange $rslt_top 0 3]]
    append outstr [format "%d %d %d %d\n" {*}[lrange $rslt_btm 4 7]]
    append outstr [format "%d %d %d %d\n" {*}[lrange $rslt_top 4 7]]
    append outstr [format "%d      %d"    {*}$rslt_fsw]
    
    puts $outstr

    }] == 0} {
    # move cursor to 1,1 position (top left-hand corner)
    puts -nonewline "\033\x5b1;1H"
    after 500
}


shutdown
