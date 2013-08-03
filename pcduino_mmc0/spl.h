#ifndef	_ASM_SPL_H_
#define	_ASM_SPL_H_

#include "types.h"

struct spl_image_info {
	const char *name;
	u8 os;
	u32 load_addr;
	u32 entry_point;
	u32 size;
	u32 flags;
};

/* Linker symbols. */
extern char __bss_start[], __bss_end[];
void spl_mmc_load_image(void);

#endif
