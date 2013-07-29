leds.bin:led.bin
	mksunxiboot led.bin leds.bin

led.bin:start.S main.c watchdog.c clock.c syslib.c pinmux.c gpio.c led.c uart.c
	arm-none-linux-gnueabi-gcc -nostdlib -c start.S -o start.o
	arm-none-linux-gnueabi-gcc -nostdlib -c main.c -o main.o
	arm-none-linux-gnueabi-gcc -nostdlib -c watchdog.c -o watchdog.o
	arm-none-linux-gnueabi-gcc -nostdlib -c clock.c -o clock.o
	arm-none-linux-gnueabi-gcc -nostdlib -c syslib.c -o syslib.o
	arm-none-linux-gnueabi-gcc -nostdlib -c pinmux.c -o pinmux.o
	arm-none-linux-gnueabi-gcc -nostdlib -c gpio.c -o gpio.o
	arm-none-linux-gnueabi-gcc -nostdlib -c led.c -o led.o
	arm-none-linux-gnueabi-gcc -nostdlib -c uart.c -o uart.o
	arm-none-linux-gnueabi-ld -T u-boot-spl.lds -Ttext 0x20 start.o main.o watchdog.o clock.o syslib.o pinmux.o gpio.o led.o uart.o -Map led.map -o led_elf
	arm-none-linux-gnueabi-objcopy -O binary -S led_elf led.bin
	arm-none-linux-gnueabi-objdump -D led_elf > led.dis

clean:
	rm -rf *.o *.bin *elf *.dis *.map *~
