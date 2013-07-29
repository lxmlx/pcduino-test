#ifndef _IO_H
#define _IO_H

#include "type.h"

/*
 * TODO: The kernel offers some more advanced versions of barriers, it might
 * have some advantages to use them instead of the simple one here.
 */

#define writel(v,a)	(*(volatile unsigned int *)(a) = (v))
#define readl(a)	(*(volatile unsigned int *)(a))

#endif
