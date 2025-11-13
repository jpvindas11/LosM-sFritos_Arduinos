CC = gcc
AS = as
LD = ld

CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector -O2
ASFLAGS = --32
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o kernel.o

all: kernel.elf

boot.o: boot.s
	$(AS) $(ASFLAGS) boot.s -o boot.o

kernel.o: kernel.c kernel.h
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o kernel.elf

iso: kernel.elf
	mkdir -p isodir/boot/grub
	cp kernel.elf isodir/boot/
	cp grub_from_iso.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

run: iso
	qemu-system-i386 -cdrom myos.iso

clean:
	rm -f *.o kernel.elf
	rm -rf isodir myos.iso

.PHONY: all iso run clean
