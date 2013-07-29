led.bin:start.S main.c clock.c
	arm-none-linux-gnueabi-gcc -nostdlib -c start.S -o start.o
	arm-none-linux-gnueabi-gcc -nostdlib -c main.c -o main.o
	arm-none-linux-gnueabi-gcc -nostdlib -c clock.c -o clock.o	
	arm-none-linux-gnueabi-ld -Ttext 0xD0020010 start.o main.o clock.o  -o led_elf
	arm-none-linux-gnueabi-objcopy -O binary -S led_elf led.bin

clean:
	rm -rf *.o *.bin led_elf *.dis

