#include "linkage.h"
#include "sunxi-common.h"
#include "asm-offsets.h"

ENTRY(_main)

/*
 * Set up initial C runtime environment and call board_init_f(0).
 */
	ldr	sp, =(CONFIG_SYS_INIT_SP_ADDR)
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	sub	sp, #GD_SIZE	/* allocate one GD above SP */
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	mov	r8, sp		/* GD is above SP */
	mov	r0, #0
	bl	board_init_f

/*
 * Set up intermediate environment (new sp and gd) and call
 * relocate_code(addr_moni). Trick here is that we'll return
 * 'here' but relocated.
 */

	ldr	sp, [r8, #GD_START_ADDR_SP]	/* sp = gd->start_addr_sp */
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	ldr	r8, [r8, #GD_BD]		/* r8 = gd->bd */
	sub	r8, r8, #GD_SIZE		/* new GD is below bd */

	adr	lr, here /* we let relocate_code return in old address */
	ldr	r0, [r8, #GD_RELOC_OFF]		/* r0 = gd->reloc_off */
	add	lr, lr, r0
	ldr	r0, [r8, #GD_RELOCADDR]		/* r0 = gd->relocaddr */
	b	relocate_code
here:

/* Set up final (full) environment */

	bl	c_runtime_cpu_setup	/* we still call old routine here */

	ldr	r0, =__bss_start	/* this is auto-relocated! */
	ldr	r1, =__bss_end		/* this is auto-relocated! */

	mov	r2, #0x00000000		/* prepare zero to clear BSS */

clbss_l:cmp	r0, r1			/* while not at end of BSS */
	strlo	r2, [r0]		/* clear 32-bit BSS word */
	addlo	r0, r0, #4		/* move to next */
	blo	clbss_l
	
	/* call board_init_r(gd_t *id, ulong dest_addr) */
	mov	r0, r8			/* gd_t */
	ldr	r1, [r8, #GD_RELOCADDR]	/* dest_addr */
	/* call board_init_r */
	ldr	pc, =board_init_r	/* this is auto-relocated! */

	/* we should not return here. */

ENDPROC(_main)
