#ifndef _SYSLIB_H
#define _SYSLIB_H

#include "type.h"

void sdelay(unsigned long loops);
void sr32(void *addr, u32 start_bit, u32 num_bits, u32 value);

#endif
