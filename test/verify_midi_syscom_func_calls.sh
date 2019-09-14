# This simply checks to see that only the scheduler increment is called when a
# MIDI clock message is received and the other messages do nothing

echo "Testing if only the MIDI clock message is responded to and other system common messages ignored"

STORE_FAILED_FILE=/tmp/verify_midi_func_call_FAILED
[ -z $MIDIDEV ] && MIDIDEV=hw:1,0,0

# fake system common commands
# those that should give response are *
# fake system exclusive
cmds[0]='f0 7a 7b f7'
# fake MIDI time code
cmds[1]='f1 7f'
# fake song position pointer
cmds[2]='f2 7f 7f'
# song select
cmds[3]='f3 7f'
# undefined
cmds[4]='f4'
# undefined
cmds[5]='f5'
# tune request
cmds[6]='f6'
# fake end system exclusive
cmds[7]='f7'
# timing clock*
cmds[8]='f8'
# undefined
cmds[9]='f9'
# start
cmds[10]='fa'
# continue
cmds[11]='fb'
# stop
cmds[12]='fc'
# undefined
cmds[13]='fd'
# active sensing
cmds[14]='fe'
# reset
cmds[15]='ff'
passed=1
for cmd in "${cmds[@]}"
do
    # TODO For some reason you have to restart openocd every time or else gdb
    # gets stuck the second time you hit the synth_midi_control_setup breakpoint
    scripts/start_openocd&
    openocd_proc_id=$(ps aux|grep '\<openocd\>'|grep -v 'grep'|awk '{print $2}')
    sleep 1

    echo "$cmd"
    rm -f "$STORE_FAILED_FILE"
    arm-none-eabi-gdb --silent \
    --command scripts/gdb-load-symbols.script \
    --command test/verify_midi_syscom_func_calls.gdb > /dev/null &
    gdb_proc_id="$!"
    sleep 1
    amidi -p $MIDIDEV -S "$cmd"
    sleep 1
    if [[ ( "$cmd" != 'f8' ) && ( -e "$STORE_FAILED_FILE" ) ]]
    then
        echo "FAILED"
        passed=0
    else
        echo "PASSED"
    fi
    if [[ ( "$cmd" == 'f8' ) && ( ! -e "$STORE_FAILED_FILE" ) ]]
    then
        echo "FAILED"
        passed=0
    else
        echo "PASSED"
    fi
    if [[ "$cmd" == 'f8' ]]
    then
        cat "$STORE_FAILED_FILE"
    fi
    kill -9 "$gdb_proc_id"
    sleep 1

    kill -9 "$openocd_proc_id"
done

if [[ $passed == 0 ]]
then
    echo "FAILED"
else
    echo "PASSED"
fi

