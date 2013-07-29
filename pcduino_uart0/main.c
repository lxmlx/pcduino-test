#include "clock.h"
#include "uart.h"

#define PB_CFG2         (*(volatile unsigned int *)0x01c2082c)

#define WDOG_CTRL_REG	(*(volatile unsigned int *)0x01c20c90)
#define WDOG_MODE_REG	(*(volatile unsigned int *)0x01c20c94)

void watchdog_init()
{
	WDOG_MODE_REG = 0;
	WDOG_CTRL_REG = ((1 << 0) | (0x0a57 << 1));
}

void gpio_init()
{
	sr32((void *)PB_CFG2, 28, 3, 2);
	sr32((void *)PB_CFG2, 24, 3, 2);
	sr32((void *)PLL1_CFG, 12, 2, 1);
	sr32((void *)PLL1_CFG, 14, 2, 1);
}

void delay(unsigned int a)
{
        volatile int i = a;
        while (i--);
}

int main(void)
{
	watchdog_init();
	clock_init(); /* 初始化时钟 */
	gpio_init();
	uart_init(); /* 初始化UART0 */

	while (1) {
		delay(100000);
		uart_putchar('O');
		uart_putchar('K');
		uart_putchar('\n');
	}
	return 0;
}
