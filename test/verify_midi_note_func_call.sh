echo "Testing if MIDI note on function is indeed called"
passed=1

rm -f "$STORE_PASSED_FILE"
touch "$STORE_PASSED_FILE"
arm-none-eabi-gdb --silent \
--command scripts/gdb-load-symbols.script \
--command test/verify_midi_note_func_call.gdb > /dev/null &
gdb_proc_id="$!"
sleep 1
bash scripts/send_midi_note_on
sleep 1
if [[ PASSED != "$(<$STORE_PASSED_FILE)" ]]
then
    echo "Failed sending MIDI note on"
    passed=0
fi
kill -9 "$gdb_proc_id"
[[ $passed == 1 ]] && echo "Sending MIDI note on...
PASSED" || echo "FAILED"

