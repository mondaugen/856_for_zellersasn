# set a breakpoint at scheduler_incTimeAndDoEvents_midiclock
# this should only be hit when the MIDI clock system common message is sent
# (0xF8)

source test/verify_midi_func_call_common.gdb

set breakpoint always-inserted on

break synth_midi_control_setup
continue
finish
break scheduler_incTimeAndDoEvents_midiclock
commands
set $to_dump="HELLO"
dump binary value /tmp/verify_midi_func_call_FAILED $to_dump
end
continue
