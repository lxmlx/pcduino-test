leds.bin:led.bin
	mksunxiboot led.bin leds.bin

led.bin:start.S main.c clock.c
	arm-none-linux-gnueabi-gcc -nostdlib -c start.S -o start.o
	arm-none-linux-gnueabi-gcc -nostdlib -c main.c -o main.o
	arm-none-linux-gnueabi-gcc -nostdlib -c clock.c -o clock.o	
	arm-none-linux-gnueabi-ld -T u-boot-spl.lds -Ttext 0x20 start.o main.o clock.o -Map led.map -o led_elf
	arm-none-linux-gnueabi-objcopy -O binary -S led_elf led.bin
	arm-none-linux-gnueabi-objdump -D led_elf > led.dis

clean:
	rm -rf *.o *.bin led_elf *.dis *.map
