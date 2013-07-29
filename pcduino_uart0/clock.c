#include "clock.h"
#include "io.h"
#include "syslib.h"

void clock_init(void)
{

	/* Set safe defaults until PMU is configured */
	writel((0<<0) | (1<<4) | (0<<8) | (1<<16), CPU_AHB_APB0_CFG);
	writel(0xa1005000, PLL1_CFG);
	sdelay(200);
	writel((0<<0) | (1<<4) | (0<<8) | (2<<16), CPU_AHB_APB0_CFG);

	/* uart clock source is apb1*/
	sr32((void *)APB1_CLK_DIV_CFG, 24, 2, 0);
	sr32((void *)APB1_CLK_DIV_CFG, 16, 2, 0);
	sr32((void *)APB1_CLK_DIV_CFG, 0, 5, 0);

	/* open the clock for uart */
	sr32((void *)APB1_GATE, 16, 1, 1);

}
