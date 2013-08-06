u-boot.bin:u-boot
	arm-none-linux-gnueabi-objcopy --gap-fill=0xff -O binary u-boot u-boot.bin

CC=arm-none-linux-gnueabi-gcc
PLATFORM_LIBGCC := -L $(shell dirname `$(CC) -print-libgcc-file-name`) -lgcc
gccincdir := $(shell $(CC) -print-file-name=include)
CFLAGS := -Os -Wstrict-prototypes -Wall -ffreestanding -nostdinc \
		-isystem $(gccincdir)  -lgcc

OBJS-y += start.o
OBJS-y += bss.o
OBJS-y += interrupts.o
OBJS-y += lowlevel_init.o
OBJS-y += board.o
OBJS-y += syslib.o
OBJS-y += watchdog.o
OBJS-y += clock.o
OBJS-y += eabi_compat.o
OBJS-y += gpio.o
OBJS-y += pinmux.o
OBJS-y += crt0.o
OBJS-y += string.o
OBJS-y += uart.o
OBJS-y += vsprintf.o
OBJS-y += i2c.o
OBJS-y += axp209.o
OBJS-y += timer.o
OBJS-y += mmc.o
OBJS-y += div64.o
OBJS-y += ctype.o
OBJS-y += memsize.o
OBJS-y += relocate.o
OBJS-y += cache-cp15.o
OBJS-y += cache_v7.o
OBJS-y += dlmalloc.o
OBJS-y += errno.o
OBJS-y += env_common.o
OBJS-y += hushtable.o
OBJS-y += env_flags.o
OBJS-y += env_attr.o
OBJS-y += linux_string.o
OBJS-y += cmd_nvedit.o
OBJS-y += env_callback.o
OBJS-y += image.o
OBJS-y += main.o
OBJS-y += hush.o
OBJS-y += command.o

u-boot: $(OBJS-y) u-boot.lds
	arm-none-linux-gnueabi-ld -T u-boot.lds --gc-sections -Bstatic \
		-Ttext 0x4A000000 $(OBJS-y) $(PLATFORM_LIBGCC) -Map u-boot.map -o u-boot
	arm-none-linux-gnueabi-objdump -D u-boot > u-boot.dis

%.o: %.S
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o u-boot *.bin *elf *.dis *.map *~ *.su
