#include "clock.h"

#define PH_CFG0			(*(volatile unsigned int *)0x01c208fc)
#define PH_CFG1  	        (*(volatile unsigned int *)0x01c20900)	
#define PH_CFG2			(*(volatile unsigned int *)0x01c20904)
#define PH_DAT  	        (*(volatile unsigned int *)0x01c2090C)
#define PH_DRI0  	        (*(volatile unsigned int *)0x01c20910)
#define PH_DRI1			(*(volatile unsigned int *)0x01c20914)
#define PH_PULL0  	        (*(volatile unsigned int *)0x01c20918)
#define PH_PULL1		(*(volatile unsigned int *)0x01c2091c)
void gpio_init()
{
  PH_CFG0 = ((1 << 28) | (1 << 24) | (1 << 20) | (1 << 16) | (1 << 12) \
		 | (1 << 8) | (1 << 4) | (1 << 0));
  PH_CFG1 |= ((0x1<<4)|(0x1<<8)|(0X1<<28));
  PH_CFG2 |= (0x1);
  PH_DRI0  = 0XFFFFFFFF;
  PH_DRI1  = 0xffffffff;
  PH_PULL0  = 0X55555555;
  PH_PULL1  = 0x55555555;
}

void delay(unsigned int a)
{
	volatile int i = a;
	while (i--);
}
int main(void)
{
	char c;
	clock_init();  /* 初始化时钟 */
	gpio_init();

	while (1)
	{
		PH_DAT = 0x0fff0000;
		delay(100000);
		PH_DAT = 0x0000ffff;
		delay(100000);
	}

	return 0;
}
