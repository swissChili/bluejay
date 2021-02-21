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
			switch_table.o
CFLAGS = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding -m32 -O2 -g
LDFLAGS = -Tlink.ld -melf_i386
ASMFLAGS = -felf
QEMUFLAGS = -d cpu_reset

kernel.elf: $(SOURCES)
	ld $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *.bin *.elf ../bin/*.iso

debug: kernel.elf
	qemu-system-i386 -s -S -kernel kernel.elf &
	@echo run "target remote localhost:1234" to connect to qemu
	gdb
	@pkill qemu-system-i38

qemu: kernel.elf
	qemu-system-i386 $(QEMUFLAGS) -monitor stdio -kernel kernel.elf -no-reboot

qemu-iso: install
	qemu-system-i386 $(QEMUFLAGS) -monitor stdio ../bin/bluejay.iso

.s.o:
	nasm $(ASMFLAGS) $<

install: kernel.elf
	cp kernel.elf ../boot/
	rm -f ../bin/bluejay.iso
	grub-mkrescue -o ../bin/bluejay.iso ..

.PHONY: install qemu clean qemu-iso debug
