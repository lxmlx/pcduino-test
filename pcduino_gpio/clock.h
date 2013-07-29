#ifndef _CLOCK_H
#define _CLOCK_H

#define CPU_AHB_APB0_CFG        (*(volatile unsigned int *)0x01c20054)
#define PLL1_CFG                (*(volatile unsigned int *)0x01c20000)  
#define APB1_CLK_DIV_CFG        (*(volatile unsigned int *)0x01c20058)  
#define APB1_GATE               (*(volatile unsigned int *)0x01c2006C)  


void clock_init(void);

#endif
