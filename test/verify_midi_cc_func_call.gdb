# run this script after loading debugging symbols
# inform the script of the index of the control change function you want to
# break at by passing the arguments --eval-command 'set $GDB_cc_index=<index>' before
# passing --command test/verify_midi_cc_func_call.gdb

source test/verify_midi_func_call_common.gdb

printf "testing CC %d\n", $GDB_cc_index
# set break at function, we have to add an offset or else GDB gets stuck (it's
# like it breaks on the instruction that jumps to the function or something, the
# function's arguments are garbage, and you cannot continue)
break synth_midi_control_setup
continue
finish
set $GDB_cc_func_start=synth_midi_control_get_midi_cc_controls()
break *($GDB_cc_func_start[$GDB_cc_index]->func+9)
commands
set $to_dump="PASSED"
dump binary value /tmp/verify_midi_func_call_PASSED $to_dump
end
continue
