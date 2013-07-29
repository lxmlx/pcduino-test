uarts.bin:uart.bin
	mksunxiboot uart.bin uarts.bin

uart.bin:start.S main.c syslib.c uart.c clock.c
	arm-none-linux-gnueabi-gcc -nostdlib -c start.S -o start.o
	arm-none-linux-gnueabi-gcc -nostdlib -c main.c -o main.o
	arm-none-linux-gnueabi-gcc -nostdlib -c syslib.c -o syslib.o
	arm-none-linux-gnueabi-gcc -nostdlib -c uart.c -o uart.o
	arm-none-linux-gnueabi-gcc -nostdlib -c clock.c -o clock.o
	arm-none-linux-gnueabi-ld -T u-boot-spl.lds -Ttext 0x20 start.o main.o syslib.o uart.o clock.o -Map uart.map -o uart_elf
	arm-none-linux-gnueabi-objcopy -O binary -S uart_elf uart.bin
	arm-none-linux-gnueabi-objdump -D uart_elf > uart.dis
clean:
	rm -rf *.o *.bin uart_elf *.dis *.map
