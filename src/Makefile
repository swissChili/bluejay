SOURCES = boot.o main.o descriptor_tables.o mem.o vga.o gdt_flush.o idt.o interrupts.o
CFLAGS = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding -m32 -O2 -g
LDFLAGS = -Tlink.ld -melf_i386
ASMFLAGS = -felf

kernel.elf: $(SOURCES)
	ld $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *.bin *.elf

qemu: install
	qemu-system-i386 -monitor stdio ../bin/bluejay.iso

.s.o:
	nasm $(ASMFLAGS) $<

install: kernel.elf
	cp kernel.elf ../boot/
	grub-mkrescue -o ../bin/bluejay.iso ..

.PHONY: install qemu clean
