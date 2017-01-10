OPTIMIZE				 ?= -O0
BIN 					 = main.elf
MMMIDI_PATH				 = ../mmmidi
MM_DSP_PATH				 = ../mm_dsp
MM_PRIMITIVES_PATH		 = ../mm_primitives
NE_DATASTRUCTURES_PATH   = ../ne_datastructures
MM_DSP_SCHABLONE_PATH    = ../mm_dsp_schablone
SRC					     = $(notdir $(wildcard $(MM_DSP_SCHABLONE_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard $(MMMIDI_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard src/*.c))
LIB						 = $(MM_DSP_PATH)/lib
LIB						+= $(MM_PRIMITIVES_PATH)/lib
LIB						+= $(NE_DATASTRUCTURES_PATH)/lib
CMSIS_INCLUDES           = ../../build/CMSIS/Include
INC 				     = $(MM_DSP_SCHABLONE_PATH)/inc
INC					    += $(MMMIDI_PATH)/inc
INC					    += $(MM_DSP_PATH)/inc
INC					    += $(MM_PRIMITIVES_PATH)/inc
INC					    += $(NE_DATASTRUCTURES_PATH)/inc
INC						+= inc
INC						+= $(CMSIS_INCLUDES)
VPATH				     = $(MMMIDI_PATH)/src:src
VPATH				    += :$(MM_DSP_SCHABLONE_PATH)/src
VPATH					+= :$(foreach path, $(INC), :$(path))
VPATH					+= :$(foreach path, $(LIB), :$(path))
DEP						 = $(foreach inc, $(INC), $(notdir $(wildcard $(inc)/*.h)))
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
							-lne_datastructures -Tstm32f429.ld
OBJSDIR					 = objs
OBJS					 = $(addprefix $(OBJSDIR)/,\
						   	$(addsuffix .o, $(basename $(SRC))))
TESTDIR					 = test
TESTSRC					 = $(notdir $(wildcard $(TESTDIR)/*.c))
TESTS					 = $(addprefix $(TESTDIR)/,\
						    $(addsuffix .o, $(basename $(TESTSRC))))
VPATH				    += :test
CC 						 = arm-none-eabi-gcc
OCD 		   = sudo openocd -f /usr/local/share/openocd/scripts/board/stm32f429discovery.cfg -f interface/stlink-v2.cfg

all: $(OBJSDIR) $(OBJS) $(BIN) $(TESTS)

$(OBJSDIR):
	if [ ! -d "$(OBJSDIR)" ]; then mkdir $(OBJSDIR); fi

$(OBJS) : $(OBJSDIR)/%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN) : $(OBJS) $(LIBDEP)
	$(CC) $(filter %.o, $^) -o $@ $(CFLAGS) $(LDFLAGS)

$(TESTS) : $(TESTDIR)/%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@

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
	rm objs/*.o test/*.o

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
