[ -z $SKIP_FLASH ] && SKIP_FLASH=0
export STORE_PASSED_FILE=/tmp/verify_midi_func_call_PASSED
[[ "$SKIP_FLASH" != 1 ]] && make flash

scripts/start_openocd&
openocd_proc_id=$(ps aux|grep '\<openocd\>'|grep -v 'grep'|awk '{print $2}')

bash test/verify_midi_cc_func_call.sh
bash test/verify_midi_note_func_call.sh

kill -9 "$openocd_proc_id"
