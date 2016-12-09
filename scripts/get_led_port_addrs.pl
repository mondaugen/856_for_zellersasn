BEGIN{ $l1=""; $l2="";}

($port,$pin) = ($_ =~ m/    (\(\(GPIO_.*ODR) \|= 0x1 << ([0-9]+);/);
if ($port){ 
    ($tmp) = ($l2 =~ m/void (.*)\_set.*/);
    if ($tmp =~ m/led[0-9]+/) {
        print "set \$tmp = &".$port."\n";
        print "printf \"".$tmp."_reg {%#x}\\n\", \$tmp \n";
        print "printf \"".$tmp."_pin {".$pin."}\\n\"\n";
    }
}
$l2=$l1;
$l1=$_;

