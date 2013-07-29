#include "watchdog.h"
#include "clock.h"
#include "led.h"
#include "mmc.h"
#include "sunxi_mmc.h"

int main(void)
{
	watchdog_init();
	clock_init();
	led_init();
	//sunxi_mmc_init(0); /* sdcard and uart conflict ? */

	while (1)
	{
		led_tx_off();
		led_rx_on();
		sdelay(10000000);
		led_tx_on();
		led_rx_off();
		sdelay(10000000);
	}

	return 0;
}
