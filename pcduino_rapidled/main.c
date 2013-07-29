#include "watchdog.h"
#include "clock.h"
#include "gpio.h"
#include "led.h"
#include "uart.h"

int main(void)
{
	watchdog_init();
	clock_init();
	gpio_init();
	led_init();
	uart_init();

	while (1)
	{
		led_tx_off();
		led_rx_on();
		sdelay(10000000);
		led_tx_on();
		led_rx_off();
		sdelay(10000000);
		uart_putchar('z');
	}

	return 0;
}
