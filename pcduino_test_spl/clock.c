#define CPU_AHB_APB0_CFG  	(*(volatile unsigned int *)0x01c20054)
#define PLL1_CFG  	        (*(volatile unsigned int *)0x01c20000)	
#define APB1_CLK_DIV_CFG  	(*(volatile unsigned int *)0x01c20058)	
#define APB1_GATE  	        (*(volatile unsigned int *)0x01c2006C)	

/* int time = 200; */
/* 如果链接地址不在0x20，那么就不能用全局变量静态变量等等，不能绝对跳转 */

void sdelay(unsigned long loops)
{
  __asm__ volatile("1:\n" "subs %0, %1, #1\n"
		   "bne 1b":"=r" (loops):"0"(loops));
}
void clock_init(void)
{
	PLL1_CFG = ((1<<31) | (0<<16) | (20<<8) | (2<<4) | (0<<1));

	sdelay(200);

   	CPU_AHB_APB0_CFG = ((2<<16)); /* CPU_CLK_SRC_SEL 01:PLL1 */

	APB1_CLK_DIV_CFG = ((0<<24));

	APB1_GATE = (0x1<<16);
}
