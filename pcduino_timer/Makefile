timers.bin:timer.bin
	mksunxiboot timer.bin timers.bin

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
OBJS-y += eabi_compat.o
OBJS-y += timer.o
OBJS-y += uart.o
OBJS-y += dram.o
OBJS-y += memsize.o
OBJS-y += vsprintf.o
OBJS-y += ctype.o

timer.bin: $(OBJS-y) u-boot-spl.lds
	arm-none-linux-gnueabi-ld -T u-boot-spl.lds --gc-sections -Ttext 0x20 $(OBJS-y) $(PLATFORM_LIBGCC) -Map timer.map -o timer_elf
	arm-none-linux-gnueabi-objcopy -O binary -S timer_elf timer.bin
	arm-none-linux-gnueabi-objdump -D timer_elf > timer.dis

%.o: %.S
	$(CC) -nostdlib -c $< -o $@

%.o: %.c
	$(CC) -nostdlib -Os -c $< -o $@

clean:
	rm -rf *.o *.bin *elf *.dis *.map *~
