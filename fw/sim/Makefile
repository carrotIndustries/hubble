CC=gcc
LD=ld 
AR=ar
AS=as
CP=objcopy
OD=objdump

EXECUTABLE=sim

all: $(EXECUTABLE)

SRC = sim.c \
      sfp_hw.c \
      led_hw.c \
      m88e111_hw.c \
      bat_hw.c \
      dpy_hw.c \
      ret_hw.c \

INC = -I. \
      -I../

include ../common/sources.mk

#  C source files
CFILES = $(filter %.c, $(SRC))

BUILDPREFIX=build/emu/

# Object files
COBJ = $(addprefix $(BUILDPREFIX),$(CFILES:.c=.o))
SOBJ = $(addprefix $(BUILDPREFIX),$(ASMFILES:.s=.o))
OBJ  = $(COBJ) $(SOBJ)

OPTIMIZE=-O0 -fdata-sections -ffunction-sections
DEBUG   =-g3
CFLAGS  =$(DEBUG)  $(OPTIMIZE) $(shell pkg-config --cflags libpulse) -MP -MMD -pthread -Wall -Wno-main
LDFLAGS = -lzmq -lm $(shell pkg-config --libs libpulse) -pthread -lmbedcrypto

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $(sort $(OBJ)) $(LDFLAGS) -o $@

$(COBJ): $(BUILDPREFIX)%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) -c $(DEFINES) $(INC) $(CFLAGS) $< -o $@

$(SOBJ): $(BUILDPREFIX)%.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(BIN) $(EXECUTABLE) $(COBJ:.o=.d) $(MAPS_C) $(MAPS_HEADER) $(MELODIES_C)

-include  $(COBJ:.o=.d)

.PHONY: clean
