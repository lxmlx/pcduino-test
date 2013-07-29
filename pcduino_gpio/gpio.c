#include "gpio.h"

void gpio_init()
{
	PH_CFG1 = PH_CFG0 = ((1<<4)|(1<<8)|(1<<12)|(1<<16)| \
					(1<<20)|(1<<24)|(1<<28));
	PH_DRI0 = 0xffffffff;
	PH_DRI1 = 0x00ffffff;
	PH_PLL1 = PH_PLL0 = 0x55555555;
}

void gpio_set(unsigned int i)
{
	PH_DAT = i;
}
