jmk_project := mkinitrd
jmk_target = mkinitrd
ROOT := /home/ch/dev/bluejay
ASM ?= nasm
CC ?= gcc
LD ?= ld
CFLAGS += -I$(ROOT)/include

all: $(jmk_target)

CFLAGS += -O2
CFLAGS += -Wall -Wno-unused-function -Wno-unused-variable

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)


OBJECTS = main.o

jmk_custom_phony += install
install: mkinitrd
	cp $< $(ROOT)/bin

$(jmk_target): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *.a *.so $(jmk_target)

Makefile: Jmk
	cd "/home/ch/dev/bluejay" && ./bin/jmk

.PHONY: $(jmk_libs_phony) $(jmk_custom_phony) clean all
