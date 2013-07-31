#include "watchdog.h"
#include "clock.h"
#include "mmc.h"
#include "sunxi_mmc.h"
#include "malloc.h"
#include "spl.h"
#include "led.h"
#include "timer.h"
#include "uart.h"
#include "gpio.h"

int test_mmc(void)
{
	struct mmc *mmc;
	int err;

	mmc_initialize((bd_t *)0);
	mmc = find_mmc_device(0);
	if (!mmc) {
		uart_puts("find mmc device failed!\n");
	}

	err = mmc_init(mmc);
	if (err) {
		uart_puts("mmc init failed! ");
		switch(err) {
		case (-16):
			uart_puts("no sd card inserted!");
			break;
		case (-17):
			uart_puts("unusable card!");
			break;
		case (-18):
			uart_puts("communications error!");
			break;
		case (-19):
			uart_puts("timeout!");
			break;
		case (-20):
			uart_puts("in progress!");
			break;
		default:
			uart_puts("unknow!");
			break;
		}
	}

	return 0;
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
	start = read_timer();
	while ((read_timer() - start) < udelay) {
		led_tx_off();
		led_rx_on();
		__udelay(50000);
		led_tx_on();
		led_rx_off();
		__udelay(50000);
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
	timer_init();
	test_timer(50000000); /* fanle? */
	test_uart();
	test_mmc();
	led_hang(10000000);
	return 0;
}
