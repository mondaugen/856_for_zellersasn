# Assuming the binary on the device is current, one need only load the symbols
# from the binary and not read them from the running program as they are known
# at load-time
file ./main.elf
set logging file /tmp/adc_vals_addr
set logging overwrite on
set logging on 
printf "%#x %d %d\n", &adc1_values, sizeof(adc1_values[0]), sizeof(adc1_values)
printf "%#x %d %d\n", &adc3_values, sizeof(adc3_values[0]), sizeof(adc3_values)
set logging off
quit
