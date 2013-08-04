#ifndef _SYSLIB_H
#define _SYSLIB_H

#include "types.h"

void sdelay(unsigned long loops);
void sr32(void *addr, u32 start_bit, u32 num_bits, u32 value);
u32 wait_on_value(u32 read_bit_mask, u32 match_value, void *read_addr,
		  u32 bound);

#endif
