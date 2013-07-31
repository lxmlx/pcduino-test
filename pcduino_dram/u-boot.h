#ifndef _U_BOOT_H_
#define _U_BOOT_H_	1

#ifndef __ASSEMBLY__
#include "types.h"
#include "common.h"
typedef struct bd_info {
	unsigned int	bi_baudrate;	/* serial console baudrate */
    ulong	        bi_arch_number;	/* unique id for this board */
    ulong	        bi_boot_params;	/* where this board expects params */
	unsigned long	bi_arm_freq; /* arm frequency */
	unsigned long	bi_dsp_freq; /* dsp core frequency */
	unsigned long	bi_ddr_freq; /* ddr frequency */
    struct				/* RAM configuration */
    {
	ulong start;
	ulong size;
    }			bi_dram[CONFIG_NR_DRAM_BANKS];
} bd_t;
#endif

/* For image.h:image_check_target_arch() */
#define IH_ARCH_DEFAULT IH_ARCH_ARM

#endif	/* _U_BOOT_H_ */

