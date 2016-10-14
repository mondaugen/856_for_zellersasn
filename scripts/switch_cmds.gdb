set $n_switches = 8

define print_3way_switch_states
    set $i = 1
    while $i <= $n_switches
        eval "set $top_state = sw%d_top_get_state()", $i
        eval "set $btm_state = sw%d_btm_get_state()", $i
        printf "%d %d ", $top_state, $btm_state
        set $i = $i + 1
    end
    printf "\n"
end

define get_3way_switch_states_hex
    set $retval = 0x0
    set $i = 0
    while $i < $n_switches
        eval "set $retval = $retval | (sw%d_top_get_state() << %d)", \
            $i + 1, 2*$i
        eval "set $retval = $retval | (sw%d_btm_get_state() << %d)", \
            $i + 1, 2*$i + 1
        set $i = $i + 1
    end
end
