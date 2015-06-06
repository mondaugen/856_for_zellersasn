BIN 					 = main.elf
MM_DSP_SCHABLONE_PATH	 = ../mm_dsp_schablone
MMDSCH_ALSA_PATH		 = ../mmdsch_alsa
MMMIDI_PATH				 = ../mmmidi
MM_DSP_PATH				 = ../mm_dsp
MM_PRIMITIVES_PATH		 = ../mm_primitives
NE_DATASTRUCTURES_PATH   = ../ne_datastructures
PLLZN_PATH	 			 = ../presets_lowlevel_zn
SRC						 = $(notdir $(wildcard $(MMDSCH_ALSA_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard $(MM_DSP_SCHABLONE_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard $(MMMIDI_PATH)/src/*.c))
SRC					    += $(notdir $(wildcard src/*.c))
LIB						 = $(MM_DSP_PATH)/lib
LIB						+= $(MM_PRIMITIVES_PATH)/lib
LIB						+= $(NE_DATASTRUCTURES_PATH)/lib
LIB						+= $(PLLZN_PATH)/lib
INC 				     = $(MM_DSP_SCHABLONE_PATH)/inc
INC					    += $(MMDSCH_ALSA_PATH)/inc
INC					    += $(MMMIDI_PATH)/inc
INC					    += $(MM_DSP_PATH)/inc
INC					    += $(MM_PRIMITIVES_PATH)/inc
INC					    += $(NE_DATASTRUCTURES_PATH)/inc
INC						+= $(PLLZN_PATH)/inc
INC						+= inc
VPATH				     = $(MMDSCH_ALSA_PATH)/src:$(MMMIDI_PATH)/src:src
VPATH				    += :$(MM_DSP_SCHABLONE_PATH)/src
VPATH					+= :$(foreach path, $(INC), :$(path))
VPATH					+= :$(foreach path, $(LIB), :$(path))
DEP						 = $(foreach inc, $(INC), $(notdir $(wildcard $(inc)/*.h)))
LIBDEP					 = $(foreach lib, $(LIB), $(notdir $(wildcard $(lib)/*.a)))
CFLAGS					+= $(foreach inc,$(INC),-I$(inc))
CFLAGS					+= -ggdb3

LDFLAGS					 = $(foreach lib,$(LIB),-L$(lib))
LDFLAGS				     += -lasound -lm -lmm_dsp -lmm_primitives \
							-lne_datastructures -lpresets_lowlevel_std
OBJSDIR					 = objs
OBJS					 = $(addprefix $(OBJSDIR)/,\
						   	$(addsuffix .o, $(basename $(SRC))))
CC 						 = gcc

all: $(OBJSDIR) $(OBJS) $(BIN)

$(OBJSDIR):
	if [ ! -d "$(OBJSDIR)" ]; then mkdir $(OBJSDIR); fi

$(OBJS) : $(OBJSDIR)/%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN) : $(OBJS) $(LIBDEP)
	$(CC) $(filter %.o, $^) -o $@ $(LDFLAGS)

clean:
	rm objs/*.o

tags:
	ctags -R . \
		$(MM_DSP_SCHABLONE_PATH)\
		$(MMDSCH_ALSA_PATH)\
		$(MMMIDI_PATH)\
		$(MM_DSP_PATH)\
		$(MM_PRIMITIVES_PATH)\
		$(NE_DATASTRUCTURES_PATH)
