BEGIN{ $l1=""; $l2="";}

($port,$pin) = ($_ =~ m/    return \((\(\(GPIO_.*IDR) & \(0x1 << ([0-9]+)\)/);
if ($port){ 
    ($tmp) = ($l2 =~ m/uint32_t (.*)\_get_state.*/);
    if ($tmp =~ m/sw[0-9]+/) {
        print "set \$tmp = &".$port."\n";
        print "printf \"".$tmp."_reg {%#x}\\n\", \$tmp \n";
        print "printf \"".$tmp."_pin {".$pin."}\\n\"\n";
    }
}
$l2=$l1;
$l1=$_;

