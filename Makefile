OPENOCD_INTERFACE		?= interface/stlink-v2.cfg
OPENOCD_BOARD			?= /usr/local/share/openocd/scripts/board/stm32f429discovery.cfg#board/stm32f429discovery.cfg
OPTIMIZE				 ?= -O0
BIN 					 = main.elf
BIN_STRIPPED			 = main-stripped.bin
MMMIDI_PATH				 = ../mmmidi
MM_DSP_PATH				 = ../mm_dsp
MM_PRIMITIVES_PATH		 = ../mm_primitives
NE_DATASTRUCTURES_PATH   = ../ne_datastructures
MM_DSP_SCHABLONE_PATH    = ../mm_dsp_schablone
LIMITER_IR_AF_PATH       = ../audio_limiter
SRC					     = $(notdir $(wildcard $(MM_DSP_SCHABLONE_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard $(MMMIDI_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard src/*.c))
LIB						 = $(MM_DSP_PATH)/lib
LIB						+= $(MM_PRIMITIVES_PATH)/lib
LIB						+= $(NE_DATASTRUCTURES_PATH)/lib
LIB						+= $(LIMITER_IR_AF_PATH)
CMSIS_INCLUDES           = ../../build/CMSIS/Include
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
OCD 		   			 = sudo openocd -f $(OPENOCD_BOARD) -f $(OPENOCD_INTERFACE)
PYTHON					 = python
CONST_OBJS				 = objs/tables.o

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

all: $(OBJSDIR) $(OBJS) $(BIN)

$(OBJSDIR):
	if [ ! -d "$(OBJSDIR)" ]; then mkdir $(OBJSDIR); fi

constants/tables.c constants/tables.h : constants/tables.py
	$(PYTHON) $<

$(CONST_OBJS) : constants/tables.c constants/tables.h
	$(CC) -c $(CFLAGS) $< -o $@

inc/_gend_fwir_header.h :
	PYTHONPATH=$(LIMITER_IR_AF_PATH) \
    N_D=$(N_D) \
    N_P=$(N_P) \
    BUFFER_SIZE=$(BUFFER_SIZE) \
    OUTPUT_FILE=$@ python3 $(LIMITER_IR_AF_PATH)/gen_fwir_header.py

$(OBJS) : $(OBJSDIR)/%.o: %.c $(DEP) inc/_gend_fwir_header.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN) : $(OBJS) $(CONST_OBJS) $(LIBDEP)
	$(CC) $(filter %.o, $^) -o $@ $(CFLAGS) $(LDFLAGS)

$(BIN_STRIPPED) : $(BIN)
	$(STRIP) -g -O binary $(BIN) -o $(BIN_STRIPPED)

$(TESTS) : $(TESTDIR)/%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@

dfu_flash: $(BIN_STRIPPED)
	sudo dfu-util -D $(BIN_STRIPPED) -s $(DFUSE_ADDR):leave -a 0

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
	rm -f objs/*.o test/*.o inc/_gend_fwir_header.h


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
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

flash_simple: test/bin/simple.bin
	$(OCD) -c init \
		-c "reset halt" \
	    -c "flash write_image erase $<" \
		-c "reset run" \
	    -c shutdown
