/*
 * author: lvrenyang
 * date: 2013-7-26 01:10
 */

#include "linkage.h"
#include "system.h"

.global _start
_start: b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction: .word _undefined_instruction
_software_interrupt:    .word _software_interrupt
_prefetch_abort:        .word _prefetch_abort
_data_abort:            .word _data_abort
_not_used:              .word _not_used
_irq:                   .word _irq
_fiq:                   .word _fiq
_pad:                   .word 0x12345678 /* now 16*4=64 */

.global _end_vect
_end_vect:

        .balignl 16,0xdeadbeef


.globl _TEXT_BASE
_TEXT_BASE:
	.word	0x4a000000

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start_ofs
_bss_start_ofs:
        .word __bss_start - _start

.globl _bss_end_ofs
_bss_end_ofs:
        .word __bss_end - _start

.globl _end_ofs
_end_ofs:
        .word _end - _start

/*
 * the actual reset code
 */
reset:
        mrs     r0, cpsr
        and     r1, r0, #0x1f           @ mask mode bits
        teq     r1, #0x1a               @ test for HYP mode
        bicne   r0, r0, #0x1f           @ clear all mode bits
        orrne   r0, r0, #0x13           @ set SVC mode
        orr     r0, r0, #0xc0           @ disable FIQ and IRQ
        msr     cpsr,r0
        
	/* Set V=0 in CP15 SCTRL register - for VBAR to point to vector */
        mrc     p15, 0, r0, c1, c0, 0   @ Read CP15 SCTRL Register
        bic     r0, #CR_V               @ V = 0
        mcr     p15, 0, r0, c1, c0, 0   @ Write CP15 SCTRL Register

        /* Set vector address in CP15 VBAR register */
        ldr     r0, =_start
        mcr     p15, 0, r0, c12, c0, 0  @Set VBAR

        bl      cpu_init_cp15
        bl      cpu_init_crit

        bl      main

/*************************************************************************
 *
 * cpu_init_cp15
 *
 * Setup CP15 registers (cache, MMU, TLBs). The I-cache is turned on unless
 * CONFIG_SYS_ICACHE_OFF is defined.
 *
 *************************************************************************/
ENTRY(cpu_init_cp15)
        /*
         * Invalidate L1 I/D
         */
        mov     r0, #0                  @ set up for MCR
        mcr     p15, 0, r0, c8, c7, 0   @ invalidate TLBs
        mcr     p15, 0, r0, c7, c5, 0   @ invalidate icache
        mcr     p15, 0, r0, c7, c5, 6   @ invalidate BP array
        mcr     p15, 0, r0, c7, c10, 4  @ DSB
        mcr     p15, 0, r0, c7, c5, 4   @ ISB

        /*
         * disable MMU stuff and caches
         */
        mrc     p15, 0, r0, c1, c0, 0
        bic     r0, r0, #0x00002000     @ clear bits 13 (--V-)
        bic     r0, r0, #0x00000007     @ clear bits 2:0 (-CAM)
        orr     r0, r0, #0x00000002     @ set bit 1 (--A-) Align
        orr     r0, r0, #0x00000800     @ set bit 11 (Z---) BTB
        orr     r0, r0, #0x00001000     @ set bit 12 (I) I-cache
        mcr     p15, 0, r0, c1, c0, 0
        mov     pc, lr                  @ back to my caller
ENDPROC(cpu_init_cp15)

ENTRY(cpu_init_crit)
        /*
         * Jump to board specific initialization...
         * The Mask ROM will have already initialized
         * basic memory. Go here to bump up clock rate and handle
         * wake up conditions.
         */
	ldr	sp, =0x8000
	bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */
	mov     pc, lr                  @ back to my caller
ENDPROC(cpu_init_crit)

