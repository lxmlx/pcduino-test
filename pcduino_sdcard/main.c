#include "watchdog.h"
#include "clock.h"
#include "mmc.h"
#include "sunxi_mmc.h"
#include "malloc.h"
#include "spl.h"
#include "led.h"
#include "timer.h"

int test_mmc(void)
{
	struct mmc *mmc;
	int err;

	mmc_initialize((bd_t *)0);
	mmc = find_mmc_device(0);
	if (!mmc) {
		led_tx_on();
		led_rx_on();
	}

	err = mmc_init(mmc);
	if (err) {
		led_tx_on();
		led_rx_on();
	}

	return 0;
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
	timer_init();
	test_timer(50000000); /* fanle? */
	test_mmc();
	mdelay(1000);
	led_hang(10000000);
	return 0;
}
