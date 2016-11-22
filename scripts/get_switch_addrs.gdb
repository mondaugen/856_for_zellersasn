# Assuming the binary on the device is current, one need only load the symbols
# from the binary and not read them from the running program as they are known
# at load-time
file ./main.elf
set logging file /tmp/switch_vals_addr.XXX
set logging overwrite on
set logging on 
printf "return {"
source /tmp/switch_regchk.XXX
printf "}"
set logging off
quit
