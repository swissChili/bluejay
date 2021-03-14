jmk_project := initrd
jmk_target = initrd.img
ROOT := /home/ch/dev/bluejay
ASM ?= nasm
CC ?= gcc
LD ?= ld
CFLAGS += -I$(ROOT)/include

all: $(jmk_target)


jmk_lib_path_mkinitrd = $(ROOT)/src/mkinitrd
jmk_lib_target_mkinitrd = mkinitrd
jmk_libs_phony += $(jmk_lib_path_mkinitrd)/$(jmk_lib_target_mkinitrd)

$(jmk_lib_path_mkinitrd)/$(jmk_lib_target_mkinitrd):
	$(MAKE) -C $(jmk_lib_path_mkinitrd) $(jmk_lib_target_mkinitrd)

SOURCES = config

initrd.img: $(SOURCES) | $(jmk_lib_path_mkinitrd)/$(jmk_lib_target_mkinitrd)
	$(jmk_lib_path_mkinitrd)/$(jmk_lib_target_mkinitrd) $@ $^

clean:
	rm -f *.o *.a *.so $(jmk_target)

Makefile: Jmk
	cd "/home/ch/dev/bluejay" && ./bin/jmk

.PHONY: $(jmk_libs_phony) $(jmk_custom_phony) clean all
