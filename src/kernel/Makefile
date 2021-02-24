SOURCES = 	boot.o \
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
			scan_codes.o
CFLAGS = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding \
			-m32 -O2 -g -Wall -Wno-unused-function -Wno-unused-variable
LDFLAGS = -Tlink.ld -melf_i386
ASMFLAGS = -felf
QEMUFLAGS = -d cpu_reset

JAYROOT = ../../

kernel.elf: $(SOURCES)
	ld $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *.bin *.elf $(JAYROOT)/bin/*.iso

debug: kernel.elf
	qemu-system-i386 -s -S -kernel kernel.elf &
	@echo run "target remote localhost:1234" to connect to qemu
	gdb
	@pkill qemu-system-i38

qemu: kernel.elf
	qemu-system-i386 $(QEMUFLAGS) -monitor stdio -kernel kernel.elf -no-reboot

qemu-iso: install
	qemu-system-i386 $(QEMUFLAGS) -monitor stdio $(JAYROOT)/bin/bluejay.iso

scan_codes.c: gen_scan_codes.py scan_codes.tsv 
	python3 $< > $@

.s.o:
	nasm $(ASMFLAGS) $<

install: kernel.elf
	cp kernel.elf $(JAYROOT)/boot/
	rm -f $(JAYROOT)/bin/bluejay.iso
	grub-mkrescue -o $(JAYROOT)/bin/bluejay.iso $(JAYROOT)

.PHONY: install qemu clean qemu-iso debug
