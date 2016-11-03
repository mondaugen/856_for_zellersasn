init
signal handle SIGINT
# send interrupt or SIGUSR{1,2} to interrupt

# get adc addresses and sizes from file
set f [open "/tmp/adc_vals_addr"]
set a1 [gets $f]
set a2 [gets $f]
set nchans [gets $f]

close $f

while {[catch -signal {
        set dsize_ [lindex $a1 1]
        set len_ [expr [lindex $a1 2] / $dsize_]

        mem2array x [expr $dsize_ * 8] [lindex $a1 0] $len_

        set z [list]

        foreach i [range 0 $nchans 1] {
            set z_ 0
            foreach j [range $i $len_ $nchans] {
                set z_ [expr $z_ + $x($j)]
            }
            lappend z [expr $z_ / ($len_. / $nchans)]
        }

        # clear screen
        puts -nonewline "\033\x5b2J"
        # move cursor to 1,1 position (top left-hand corner)
        puts -nonewline "\033\x5b1;1H"

        puts "ADC1"
        foreach i [range 0 $nchans 1] {
            puts -nonewline [format "%4.0f " [lindex $z $i]]
        }
        puts ""

        set dsize_ [lindex $a2 1]
        set len_ [expr [lindex $a2 2] / $dsize_]

        mem2array x [expr $dsize_ * 8] [lindex $a2 0] $len_

        set z [list]

        foreach i [range 0 $nchans 1] {
            set z_ 0
            foreach j [range $i $len_ $nchans] {
                set z_ [expr $z_ + $x($j)]
            }
            lappend z [expr $z_ / ($len_. / $nchans)]
        }

        puts "ADC2"
        foreach i [range 0 $nchans 1] {
            puts -nonewline [format "%4.0f " [lindex $z $i]]
        }

    }] == 0} {
    # move cursor to 1,1 position (top left-hand corner)
    puts -nonewline "\033\x5b1;1H"
    after 500
}


shutdown
