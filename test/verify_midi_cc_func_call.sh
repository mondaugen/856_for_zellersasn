# This requires an 856 be plugged in with the debugging dongle and powered

[ -z $N_CC_FUNS ] && N_CC_FUNS=67

echo "Testing if MIDI control change functions are indeed called"
passed=1

n=0
while [[ $n -lt $N_CC_FUNS ]]
do
    rm -f "$STORE_PASSED_FILE"
    touch "$STORE_PASSED_FILE"
    arm-none-eabi-gdb --silent \
    --command scripts/gdb-load-symbols.script \
    --eval-command 'set $GDB_cc_index='$n \
    --command test/verify_midi_cc_func_call.gdb > /dev/null &
    gdb_proc_id="$!"
    sleep 1
    scripts/send_midi_cc "$n"
    sleep 1
    if [[ PASSED != "$(<$STORE_PASSED_FILE)" ]]
    then
        echo "Failed at CC=$n"
        n=$N_CC_FUNS
        passed=0
    else
        n=$(($n+1))
    fi
    kill -9 "$gdb_proc_id"
done
    
[[ $passed == 1 ]] && echo "Tested $n functions
PASSED" || echo "FAILED"
