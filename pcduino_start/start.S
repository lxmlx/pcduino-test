.section .test

data:
.byte 0x11
.align 2

.globl _start
_start:
 mov r0, #data
 mov r7, #1
 svc 0x00000000
