ifeq ($(filter /tmp/manual.zip /tmp/manual.html /tmp/env_ramp.png,$(MAKECMDGOALS)),)
 # Defines conditional on board version
 CODEC=
 ifeq ($(BOARD_VERSION),BOARD_V1)
 CODEC=WM8778
 CFLAGS+=-DBOARD_V1
 endif
 ifeq ($(BOARD_VERSION),BOARD_V1.1)
 CODEC=WM8778
 CFLAGS+=-DCODEC_ANALOG_DIGITAL_MIX
 CFLAGS+=-DBOARD_V1
 endif
 ifeq ($(BOARD_VERSION),BOARD_V2)
 CODEC=CS4270
 CFLAGS+=-DBOARD_V2
 endif
 ifeq ($(CODEC),)
 $(error CODEC not set, did you not specify BOARD_VERSION correctly?)
 endif
endif

OPENOCD_INTERFACE		?= interface/stlink-v2.cfg
OPENOCD_BOARD			?= /usr/local/share/openocd/scripts/board/stm32f429discovery.cfg#board/stm32f429discovery.cfg
OPTIMIZE				 ?= -O0
BIN 					 = main.elf
BIN_STRIPPED			 = main-stripped.bin
MMMIDI_PATH				 = libs/mmmidi
MM_DSP_PATH				 = libs/mm_dsp
MM_PRIMITIVES_PATH		 = libs/mm_primitives
NE_DATASTRUCTURES_PATH   = libs/ne_datastructures
MM_DSP_SCHABLONE_PATH    = libs/mm_dsp_schablone
LIMITER_IR_AF_PATH       = libs/audio_limiter
SRC					     = $(notdir $(wildcard $(MM_DSP_SCHABLONE_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard $(MMMIDI_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard src/*.c))
HW_SRC=
ifeq ($(CODEC),WM8778)
HW_SRC+=hw/wm8778.c
endif
ifeq ($(CODEC),CS4270)
HW_SRC+=hw/cs4270.c
endif
LIB						 = $(MM_DSP_PATH)/lib
LIB						+= $(MM_PRIMITIVES_PATH)/lib
LIB						+= $(NE_DATASTRUCTURES_PATH)/lib
LIB						+= $(LIMITER_IR_AF_PATH)
CMSIS_INCLUDES           = libs/CMSIS_5/CMSIS/Core/Include/ libs/CMSIS_5/CMSIS/DSP/Include/
INC 				     = $(MM_DSP_SCHABLONE_PATH)/inc
INC					    += $(MMMIDI_PATH)/inc
INC					    += $(MM_DSP_PATH)/inc
INC					    += $(MM_PRIMITIVES_PATH)/inc
INC					    += $(NE_DATASTRUCTURES_PATH)/inc
INC						+= $(LIMITER_IR_AF_PATH)
INC						+= inc
INC						+= $(CMSIS_INCLUDES)
INC						+= constants
VPATH				     = $(MMMIDI_PATH)/src:src
VPATH				    += :$(MM_DSP_SCHABLONE_PATH)/src
VPATH					+= :$(foreach path, $(INC), :$(path))
VPATH					+= :$(foreach path, $(LIB), :$(path))
DEP						 = $(foreach inc, $(INC), $(notdir $(wildcard $(inc)/*.h)))
DEP						+= constants/tables.h
LIBDEP					 = $(foreach lib, $(LIB), $(notdir $(wildcard $(lib)/*.a)))
CFLAGS					+= $(foreach inc,$(INC),-I$(inc))
CFLAGS					+= -ggdb3 $(OPTIMIZE) -DSTM32F429_439xx \
						   -ffunction-sections -fdata-sections -Wall \
						   -mlittle-endian -mthumb -mcpu=cortex-m4 \
						   -mthumb-interwork -mfloat-abi=hard \
						   -mfpu=fpv4-sp-d16 -dD \
						   --specs=nano.specs -Wno-unused-function

LDFLAGS					 = $(foreach lib,$(LIB),-L$(lib))
LDFLAGS				     += -lm -lmm_dsp -lmm_primitives \
							-lne_datastructures -llimiter_ir_af -Tstm32f429.ld
OBJSDIR					 = objs
OBJS					 = $(addprefix $(OBJSDIR)/,\
						   	$(addsuffix .o, $(basename $(SRC))))
TESTDIR					 = test
TESTSRC					 = $(notdir $(wildcard $(TESTDIR)/*.c))
TESTS					 = $(addprefix $(TESTDIR)/,\
						    $(addsuffix .o, $(basename $(TESTSRC))))
VPATH				    += :test
CC 						 = arm-none-eabi-gcc
STRIP					 = arm-none-eabi-strip
OCD 		   			 = openocd -f $(OPENOCD_BOARD) -f $(OPENOCD_INTERFACE)
PYTHON					 = python2
CONST_OBJS				 = objs/tables.o
HW_OBJS                  = $(addprefix $(OBJSDIR)/,$(notdir $(addsuffix .o, $(basename $(HW_SRC)))))

# Linker settings for simple tests and profiling
PROF_LDFLAGS             = -L../CMSIS_5/CMSIS/Lib/GCC -larm_cortexM4lf_math -Tstm32f429.ld

# Audio settings
BUFFER_SIZE=256
NUM_CHANNELS=2
CODEC_DMA_BUF_LEN=$(shell python -c 'print(int($(BUFFER_SIZE)*$(NUM_CHANNELS)))')
CFLAGS+=-DBUFFER_SIZE=$(BUFFER_SIZE)
CFLAGS+=-DCODEC_DMA_BUF_LEN=$(CODEC_DMA_BUF_LEN)
CODEC_SAMPLE_RATE=32000
CFLAGS+=-DCODEC_SAMPLE_RATE=$(CODEC_SAMPLE_RATE)

# Limiter settings
# ramp up time
N_P_seconds=0.01
N_P=$(shell python -c 'print(int($(CODEC_SAMPLE_RATE)*$(N_P_seconds)))')
# ramp down time in seconds
N_D_seconds=5
# ramp down time in samples
N_D=$(shell python -c 'print(int($(CODEC_SAMPLE_RATE)*$(N_D_seconds)))')

# The address the DFU uploader uses to write the DFU file to the device.
DFUSE_ADDR				 = 0x08000000

.PHONY: all libs

all: $(OBJSDIR) $(OBJS) $(BIN)

libs: mm_dsp mm_primitives ne_datastructures audio_limiter

# mmmidi and mm_dsp_schablone's source files are compiled by this project (so we
# don't call their Makefiles)
#mmmidi:
#	cd libs/mmmidi && $(MAKE)

#mm_dsp_schablone:
#	cd libs/mm_dsp_schablone && $(MAKE)

mm_dsp:
	cd libs/mm_dsp && $(MAKE) -f arm_cm4.mk

mm_primitives:
	cd libs/mm_primitives && $(MAKE) -f arm_cm4.mk

ne_datastructures:
	cd libs/ne_datastructures && $(MAKE) -f arm_cm4.mk

audio_limiter:
	cd libs/audio_limiter && $(MAKE) -f arm_cm4.mk

$(OBJSDIR):
	if [ ! -d "$(OBJSDIR)" ]; then mkdir $(OBJSDIR); fi

inc/version.h : scripts/gen_version_header.sh
	bash $< > $@

constants/tables.c constants/tables.h : constants/tables.py
	$(PYTHON) $<

$(CONST_OBJS) : constants/tables.c constants/tables.h
	$(CC) -c $(CFLAGS) $< -o $@

inc/_gend_fwir_header.h : $(LIMITER_IR_AF_PATH)/gen_fwir_header.py
	PYTHONPATH=$(LIMITER_IR_AF_PATH) \
    N_D=$(N_D) \
    N_P=$(N_P) \
    BUFFER_SIZE=$(BUFFER_SIZE) \
    OUTPUT_FILE=$@ python3 $(LIMITER_IR_AF_PATH)/gen_fwir_header.py

inc/_gend_tempo_map_table_header.h : scripts/gen_tempo_map.py
	PYTHONPATH=scripts \
    OUTFILE=$@ python3 scripts/gen_tempo_map.py

$(HW_OBJS) : $(OBJSDIR)/%.o: hw/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJS) : $(OBJSDIR)/%.o: %.c $(DEP) inc/version.h \
    inc/_gend_fwir_header.h inc/_gend_tempo_map_table_header.h $(HW_OBJS)
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN) : $(OBJS) $(CONST_OBJS) $(HW_OBJS) libs
	$(CC) $(filter %.o, $^) -o $@ $(CFLAGS) $(LDFLAGS)

$(BIN_STRIPPED) : $(BIN)
	$(STRIP) -g -O binary $(BIN) -o $(BIN_STRIPPED)

$(TESTS) : $(TESTDIR)/%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@

#ifeq ($(BOARD_VERSION),BOARD_V2)

dfu_flash: $(BIN_STRIPPED)
	dfu-util -D $(BIN_STRIPPED) -s $(DFUSE_ADDR):leave -a 0

#endif

# NOTE: Board versions before V2 do not have DFU hardware and must be flashed
# using scripts/flash_bin.sh and supplying the ADDRESS environment variable
# equal to DFUSE_ADDR when using stripped binaries.
# Versions with DFU hardware can simply use DFU util and specify the address
# (like the dfu_flash recipe).
# This makefile currently does not make DFUs that include the flashing address.
binary_release: $(BIN_STRIPPED)
	tag=$$(git tag --points-at HEAD); \
	if [ -z $$tag ]; then \
		echo "git commit must be tagged to do binary_release"; \
	else \
		binary_release_name=856_for_zellersasn_$${tag}_$(BOARD_VERSION).bin; \
		cp $(BIN_STRIPPED) /tmp/$$binary_release_name; \
    fi;

flash: $(BIN)
	$(OCD) -c init \
		-c "reset halt" \
	    -c "flash write_image erase $(BIN)" \
		-c "reset run" \
	    -c shutdown

erase_flash:
	$(OCD) -c init \
		-c "reset halt" \
		-c "flash erase_address 0x08000000 0x00200000" \
		-c shutdown

reset: $(BIN)
	$(OCD) -c init -c "reset run" -c shutdown

clean:
	rm -f $(BIN) objs/*.o test/*.o inc/_gend_fwir_header.h inc/_gend_tempo_map_table_header.h inc/version.h

tags:
	ctags -R . \
		$(MM_DSP_SCHABLONE_PATH)\
		$(MMMIDI_PATH)\
		$(MM_DSP_PATH)\
		$(MM_PRIMITIVES_PATH)\
		$(NE_DATASTRUCTURES_PATH)\
		$(CMSIS_INCLUDES)\

test_mem_read:
	$(OCD) -c init \
		-c "mdw 0x40021810" \
		-c shutdown | awk '{print $$1}'

test/bin/simple.bin : test/simple.c src/system_init.c src/syscalls.c src/startup.c src/fmc.c
	$(CC) $^ -o $@ $(CFLAGS) $(PROF_LDFLAGS)

flash_simple: test/bin/simple.bin
	$(OCD) -c init \
		-c "reset halt" \
	    -c "flash write_image erase $<" \
		-c "reset run" \
	    -c shutdown

/tmp/env_ramp.png : scripts/smart_envelope_demo.m
	octave --no-gui $<

/tmp/manual.html : doc/manual.md  /tmp/env_ramp.png
	markdown<$<>/tmp/manual.html

/tmp/manual.zip: /tmp/manual.html
	cd /tmp ; \
	rm -rf manual ; \
	mkdir manual ; \
	cp manual.html manual/ ; \
	cp env_ramp.png manual/ ; \
	zip -r manual.zip manual ;
    
