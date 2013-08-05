#include "linkage.h"
#include "sunxi-common.h"
#include "asm-offsets.h"

ENTRY(lowlevel_init)
	/*
	 * Setup a temporary stack
	 */
	ldr	sp, =CONFIG_SYS_INIT_SP_ADDR
	bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */

	sub	sp, #GD_SIZE
	bic	sp, sp, #7
	mov	r8, sp

	/*
	 * Save the old lr(passed in ip) and the current lr to stack
	 */
	push	{ip, lr}
	/* lr is r14 , linkaddress return address
	 * r12 is ip, thumb
	 */

	/*
	 * go setup pll, mux, memory
	 */
	bl	s_init
	pop	{ip, pc}
	/* chuzhan erbushi ldr lai huifu pc*/
ENDPROC(lowlevel_init)
