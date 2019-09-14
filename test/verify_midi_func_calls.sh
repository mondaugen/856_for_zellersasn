# It is recommended to pipe the output through less so you can easily see the results of the test
# Also there are many environment flags available
# e.g.,
# BOARD_VERSION=BOARD_V2 OPENOCD_INTERFACE=interface/stlink-v2-1.cfg OPENOCD_BOARD=board/stm32f429discovery.cfg bash test/verify_midi_func_calls.sh  |less

[ -z $SKIP_FLASH ] && SKIP_FLASH=0
export STORE_PASSED_FILE=/tmp/verify_midi_func_call_PASSED
[[ "$SKIP_FLASH" != 1 ]] && make flash || exit -1

scripts/start_openocd&
openocd_proc_id=$(ps aux|grep '\<openocd\>'|grep -v 'grep'|awk '{print $2}')

bash test/verify_midi_cc_func_call.sh
bash test/verify_midi_note_func_call.sh

kill -9 "$openocd_proc_id"

# This script launches and kills openocd on its own
bash test/verify_midi_syscom_func_calls.sh
