[ -z $1 ] && echo "Specify binary to flash" && exit 1
[ -z $OPENOCD_BOARD ] && OPENOCD_BOARD=board/stm32f429discovery.cfg
[ -z $OPENOCD_INTERFACE ] && OPENOCD_INTERFACE=interface/stlink-v2.cfg
sudo openocd -f $OPENOCD_BOARD -f $OPENOCD_INTERFACE \
-c init \
		-c "reset halt" \
	    -c "flash write_image erase $1 $ADDRESS" \
		-c "reset run" \
	    -c shutdown
