.global _start

_start:
	ldr sp, =0x00007f00

	b main
