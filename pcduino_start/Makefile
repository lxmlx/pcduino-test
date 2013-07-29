start.bin:start.S
	${CROSS_COMPILE}gcc -nostdlib -c start.S -o start.o
	${CROSS_COMPILE}ld -Ttext 0xD0020010 start.o -o start_elf
	${CROSS_COMPILE}objcopy -O binary -S start_elf led.bin

clean:
	rm -rf *.o *.bin start_elf *.dis
