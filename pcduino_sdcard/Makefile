sdcards.bin:sdcard.bin
	mksunxiboot sdcard.bin sdcards.bin

CC=arm-none-linux-gnueabi-gcc
PLATFORM_LIBGCC := -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc

OBJS-y += start.o
OBJS-y += main.o
OBJS-y += watchdog.o
OBJS-y += clock.o
OBJS-y += syslib.o
OBJS-y += pinmux.o
OBJS-y += gpio.o
OBJS-y += led.o
OBJS-y += string.o
OBJS-y += mmc.o
OBJS-y += sunxi_mmc.o
OBJS-y += eabi_compat.o
OBJS-y += timer.o
OBJS-y += time.o

sdcard.bin: $(OBJS-y) u-boot-spl.lds
	arm-none-linux-gnueabi-ld -T u-boot-spl.lds --gc-sections -Ttext 0x20 $(OBJS-y) $(PLATFORM_LIBGCC) -Map sdcard.map -o sdcard_elf
	arm-none-linux-gnueabi-objcopy -O binary -S sdcard_elf sdcard.bin
	arm-none-linux-gnueabi-objdump -D sdcard_elf > sdcard.dis

%.o: %.S
	$(CC) -nostdlib -c $< -o $@

%.o: %.c
	$(CC) -nostdlib -c $< -o $@

clean:
	rm -rf *.o *.bin *elf *.dis *.map *~
