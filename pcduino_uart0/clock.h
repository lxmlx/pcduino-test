#ifndef _PCDUINO_UART0_CLOCK_H
#define _PCDUINO_UART0_CLOCK_H

#include "type.h"

#define CPU_AHB_APB0_CFG	0x01c20054
#define PLL1_CFG		0x01c20044
#define APB1_CLK_DIV_CFG	0x01c20058
#define APB1_GATE		0x01c2006C

void clock_init(void);

#endif
