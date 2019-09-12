source test/verify_midi_func_call_common.gdb
printf "testing synth_midi_note_on_control\n"
break synth_midi_control_setup
continue
finish
break synth_midi_note_on_control
commands
set $to_dump="PASSED"
dump binary value /tmp/verify_midi_func_call_PASSED $to_dump
end
continue
