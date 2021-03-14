jmk_project := kernel
jmk_target = kernel.elf
ROOT := /home/ch/dev/bluejay
ASM ?= nasm
CC ?= gcc
LD ?= ld
CFLAGS += -I$(ROOT)/include

all: $(jmk_target)

CFLAGS += -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding
CFLAGS += -O2
CFLAGS += -g
CFLAGS += -m32
CFLAGS += -Wall -Wno-unused-function -Wno-unused-variable
ASM = nasm
    

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

.s.o:
	$(ASM) $(ASMFLAGS) $< -o $@
    



jmk_lib_path_initrd = $(ROOT)/boot/initrd
jmk_lib_target_initrd = initrd.img
jmk_libs_phony += $(jmk_lib_path_initrd)/$(jmk_lib_target_initrd)

$(jmk_lib_path_initrd)/$(jmk_lib_target_initrd):
	$(MAKE) -C $(jmk_lib_path_initrd) $(jmk_lib_target_initrd)

LDFLAGS = -Tlink.ld -melf_i386
ASMFLAGS = -felf
QEMUFLAGS = -d cpu_reset

OBJECTS = 	boot.o \
			main.o \
			descriptor_tables.o \
			io.o \
			vga.o \
			gdt_flush.o \
			idt.o \
			log.o \
			irq.o \
			pic.o \
			timer.o \
			paging.o \
			switch_table.o \
			scan_codes.o \
			kheap.o \
			alloc.o \
			vfs.o \
			multiboot.o \
			vfs_initrd.o

$(jmk_target): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^
    

debug: kernel.elf
	qemu-system-i386 -s -S -kernel kernel.elf &
	@echo run "target remote localhost:1234" to connect to qemu
	gdb
	@pkill qemu-system-i38

qemu: kernel.elf
	qemu-system-i386 $(QEMUFLAGS) -monitor stdio -kernel kernel.elf -no-reboot

qemu-iso: install
	qemu-system-i386 $(QEMUFLAGS) -monitor stdio $(ROOT)/bin/bluejay.iso

scan_codes.c: gen_scan_codes.py scan_codes.tsv 
	python3 $< > $@

install: kernel.elf $(jmk_lib_path_initrd)/$(jmk_lib_target_initrd)
	cp kernel.elf $(ROOT)/boot/
	rm -f $(ROOT)/bin/bluejay.iso
	grub-mkrescue -o $(ROOT)/bin/bluejay.iso $(ROOT)

clean:
	rm -f *.o *.a *.so $(jmk_target)

Makefile: Jmk
	cd "/home/ch/dev/bluejay" && ./bin/jmk

.PHONY: $(jmk_libs_phony) $(jmk_custom_phony) clean all
