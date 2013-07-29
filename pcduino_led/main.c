#include "clock.h"

#define PH_CFG1  	        (*(volatile unsigned int *)0x01c20900)	
#define PH_DAT  	        (*(volatile unsigned int *)0x01c2090C)
#define PH_DRI  	        (*(volatile unsigned int *)0x01c20910)
#define PH_PULL  	        (*(volatile unsigned int *)0x01c20918)
void gpio_init()
{
 /*PCDUINO GPIO4--PH9:
  *bit[6:4]:PH9_SELECT 001:OUTPUT
  *PCDUINO GPIO5--PH10:
  *bit[10:8]:PH10_SELECT 001:OUTPUT
  */ 
  PH_CFG1 |= ((0x1<<4)|(0x1<<8)|(0X1<<28));
  PH_DRI   = 0XFFFFFFFF;
  PH_PULL  = 0X55555555;
}

void delay()
{
	volatile int i = 0x300000;
	while (i--);
}
int main(void)
{
	char c;
	clock_init(); /* 初始化时钟 */
	gpio_init();

	while (1)
	{
		PH_DAT = 0x00;
		delay();
		PH_DAT = 0xffff;
		delay();
	}

	return 0;
}

