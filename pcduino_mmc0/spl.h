#ifndef	_ASM_SPL_H_
#define	_ASM_SPL_H_

/* Linker symbols. */
extern char __bss_start[], __bss_end[];
void spl_mmc_load_image(void);

#endif
