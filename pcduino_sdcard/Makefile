sdcards.bin:sdcard.bin
	mksunxiboot sdcard.bin sdcards.bin

CC=arm-none-linux-gnueabi-gcc
PLATFORM_LIBGCC := -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc

sdcard.bin:start.S main.c watchdog.c clock.c syslib.c pinmux.c gpio.c led.c string.c mmc.c eabi_compat.c
	arm-none-linux-gnueabi-gcc -nostdlib -c start.S -o start.o
	arm-none-linux-gnueabi-gcc -nostdlib -c main.c -o main.o
	arm-none-linux-gnueabi-gcc -nostdlib -c watchdog.c -o watchdog.o
	arm-none-linux-gnueabi-gcc -nostdlib -c clock.c -o clock.o
	arm-none-linux-gnueabi-gcc -nostdlib -c syslib.c -o syslib.o
	arm-none-linux-gnueabi-gcc -nostdlib -c pinmux.c -o pinmux.o
	arm-none-linux-gnueabi-gcc -nostdlib -c gpio.c -o gpio.o
	arm-none-linux-gnueabi-gcc -nostdlib -c led.c -o led.o
	arm-none-linux-gnueabi-gcc -nostdlib -c string.c -o string.o
	arm-none-linux-gnueabi-gcc -nostdlib -c mmc.c -o mmc.o
	arm-none-linux-gnueabi-gcc -nostdlib -c sunxi_mmc.c -o sunxi_mmc.o
	arm-none-linux-gnueabi-gcc -nostdlib -c eabi_compat.c -o eabi_compat.o
	arm-none-linux-gnueabi-ld -T u-boot-spl.lds -Ttext 0x20 start.o main.o watchdog.o clock.o syslib.o pinmux.o gpio.o led.o string.o mmc.o sunxi_mmc.o eabi_compat.o $(PLATFORM_LIBGCC) -Map sdcard.map -o sdcard_elf
	arm-none-linux-gnueabi-objcopy -O binary -S sdcard_elf sdcard.bin
	arm-none-linux-gnueabi-objdump -D sdcard_elf > sdcard.dis

clean:
	rm -rf *.o *.bin *elf *.dis *.map *~
