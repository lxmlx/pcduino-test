mmc0s.bin:mmc0.bin
	mksunxiboot mmc0.bin mmc0s.bin

CC=arm-none-linux-gnueabi-gcc
PLATFORM_LIBGCC := -L $(shell dirname `$(CC) -print-libgcc-file-name`) -lgcc
gccincdir := $(shell $(CC) -print-file-name=include)
CFLAGS := -Os -Wstrict-prototypes -fstack-usage -Wall -ffreestanding -nostdinc -isystem $(gccincdir) 


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
OBJS-y += div64.o
OBJS-y += mmc.o
OBJS-y += spl.o
OBJS-y += i2c.o
OBJS-y += axp209.o

mmc0.bin: $(OBJS-y) u-boot-spl.lds
	arm-none-linux-gnueabi-ld -T u-boot-spl.lds --gc-sections -Ttext 0x20 $(OBJS-y) $(PLATFORM_LIBGCC) -Map mmc0.map -o mmc0_elf
	arm-none-linux-gnueabi-objcopy -O binary -S mmc0_elf mmc0.bin
	arm-none-linux-gnueabi-objdump -D mmc0_elf > mmc0.dis

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.bin *elf *.dis *.map *~ *.su
