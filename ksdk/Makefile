
OBJS = \
	lib/kcrt.o \
	lib/string.o \
	lib/syscall.o

INCLUDE_DIR = include/

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

CFLAGS = -Wall -O0 -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -ffreestanding -fno-common -nostdlib
CFLAGS += -mno-relax -fno-pie -no-pie -mcmodel=medany -I..

D_FILES = $(OBJS:%.o=%.d)
INCLUDE_PARAMS = $(foreach d, $(INCLUDE_DIR), -I$d)
CFLAGS += $(INCLUDE_PARAMS)

all: libk.a

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

libk.a: $(OBJS)
	$(AR) cr libk.a $(OBJS)

clean:
	rm $(OBJS) $(D_FILES) libk.a
