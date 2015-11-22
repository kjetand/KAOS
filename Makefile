KERNEL_ADDR = 0x8000

CC     = ~/opt/cross/bin/i686-elf-gcc
LD     = ld
CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -m32 -O2
OFILES = kernel.o \
         screen.o \
         string.o

all: clean bootloader.bin kernel.bin
	dd if=/dev/zero of=KAOS.bin bs=512 count=2880
	dd if=bootloader.bin of=KAOS.bin conv=notrunc
	dd if=kernel.bin of=KAOS.bin conv=notrunc bs=512 seek=2

bootloader.bin: boot/bootloader.asm
	nasm -f bin boot/bootloader.asm -o bootloader.bin

kernel.bin: $(OFILES)
	$(LD) -o kernel.bin -Ttext $(KERNEL_ADDR) $(OFILES) \
	-e kernel_main -m elf_i386 --oformat binary

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

qemu: all
	qemu-system-i386 -fda KAOS.bin -boot a

bochs: all
	bochs

clean:
	rm -f *.bin
	rm -f *.o
