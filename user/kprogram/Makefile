# Sample Makefile for Kalorona-Kernel User Program;

# Project Info;
PROJECT_NAME = kprogram

# KSDK Configure;
KSDK_PATH = ../../ksdk

# Mode: (Debug/Release)
MODE = Debug

#--------------------------------------------------

KSDK_INCLUDE = $(KSDK_PATH)/include
KSDK_LIB = $(KSDK_PATH)/libk.a

EXECUTABLE_NAME = $(PROJECT_NAME).elf

INCLUDE_DIR = \
	../../ \
	$(KSDK_INCLUDE)

OBJS = \
	main.o

D_FILES = $(OBJS:%.o=%.d)
INCLUDE_PARAMS = $(foreach d, $(INCLUDE_DIR), -I$d)

CC = $(CROSS_COMPILE)gcc

CFLAGS = -Wall -O0 -fno-omit-frame-pointer
CFLAGS += -MD
CFLAGS += -ffreestanding -fno-common -nostdlib
CFLAGS += -I. -mno-relax -fno-pie -no-pie -mcmodel=medany
CFLAGS += $(INCLUDE_PARAMS) -Wl,-e_start

ifeq ($(MODE), Debug)
	CFLAGS += -ggdb -gdwarf-2
endif

all: $(EXECUTABLE_NAME)

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXECUTABLE_NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(KSDK_LIB) -o $(EXECUTABLE_NAME)

clean:
	rm -rf $(EXECUTABLE_NAME) $(OBJS) $(D_FILES)
