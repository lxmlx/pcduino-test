#include "watchdog.h"
#include "clock.h"
#include "malloc.h"
#include "spl.h"
#include "led.h"
#include "timer.h"
#include "uart.h"
#include "gpio.h"
#include "dram.h"
#include "io.h"

void test_dram()
{
	writel(2000,0x80000000-4);
	mdelay(readl(0x80000000-4));
	uart_puts("dram ok!\n");
}

void test_uart()
{
	uart_puts("uart ok!\n");
}

/* shan duoshao weimiao zuixiao dayu 200000 */
void test_timer(ulong udelay)
{
	ulong start;
	led_init();
	start = read_timer(0);
	while ((read_timer(0) - start) < udelay) {
		led_tx_off();
		led_rx_on();
		timer0_udelay(2000000);
		led_tx_on();
		led_rx_off();
		timer1_udelay(2000000);
	};
	led_tx_off();
	led_rx_off();
}

int main(void)
{
	watchdog_init();
	clock_init();
	gpio_init();
	uart_init();
	timer_init_all();
	sunxi_dram_init();
	test_timer(50000000); /* fanle? */
	test_uart();
	test_dram();
	led_hang(10000000);
	return 0;
}
