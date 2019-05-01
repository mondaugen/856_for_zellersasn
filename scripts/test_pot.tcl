init
signal handle SIGINT
# send interrupt or SIGUSR{1,2} to interrupt

# get adc addresses and sizes from file
set f [open "/tmp/adc_vals_addr"]
set a1 [gets $f]
set a2 [gets $f]
puts "board_version:"
puts $board_version
set nchans1 0
set nchans2 0
if {([string compare $board_version "BOARD_V1"] == 0) \
    || ([string compare $board_version "BOARD_V1.1"] == 0)} {
    set nchans1 [gets $f]
    set nchans2 $nchans1
} elseif {[string compare $board_version "BOARD_V2"] == 0} {
    set nchans1 [gets $f]
    set nchans2 [gets $f]
} else {
    puts "Must specify BOARD_VERSION as BOARD_V1, BOARD_V1.1 or BOARD_V2"
}
puts "nchans1:"
puts $nchans1
puts "nchans2:"
puts $nchans2

close $f

while {[catch -signal {
        # for ADC1

        # dsize_ is size in bytes
        set dsize_ [lindex $a1 1]
        set len_ [expr [lindex $a1 2] / $dsize_]

        # mem2arry wants datum size in bits, therefore x8
        mem2array x [expr $dsize_ * 8] [lindex $a1 0] $len_

        set z [list]

        # compute average
        foreach i [range 0 $nchans1 1] {
            set z_ 0
            foreach j [range $i $len_ $nchans1] {
                set z_ [expr $z_ + $x($j)]
            }
            lappend z [expr $z_ / ($len_. / $nchans1)]
        }

        # clear screen
        puts -nonewline "\033\x5b2J"
        # move cursor to 1,1 position (top left-hand corner)
        puts -nonewline "\033\x5b1;1H"

        puts "ADC1"
        foreach i [range 0 $nchans1 1] {
            puts -nonewline [format "%4.0f " [lindex $z $i]]
        }
        puts ""

        # for ADC2

        set dsize_ [lindex $a2 1]
        set len_ [expr [lindex $a2 2] / $dsize_]

        mem2array x [expr $dsize_ * 8] [lindex $a2 0] $len_

        set z [list]

        foreach i [range 0 $nchans2 1] {
            set z_ 0
            foreach j [range $i $len_ $nchans2] {
                set z_ [expr $z_ + $x($j)]
            }
            lappend z [expr $z_ / ($len_. / $nchans2)]
        }

        puts "ADC2"
        foreach i [range 0 $nchans2 1] {
            puts -nonewline [format "%4.0f " [lindex $z $i]]
        }

    }] == 0} {
    # move cursor to 1,1 position (top left-hand corner)
    puts -nonewline "\033\x5b1;1H"
    after 500
}


shutdown
