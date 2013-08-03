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
#include "string.h"
#include "syslib.h"
#include "mmc.h"
#include "spl.h"
#include "common.h"
#include "i2c.h"
#include "axp209.h"

void test_dram(void)
{
	ulong data;

	data = 3000;
	writel(data, 0x80000000-4);
	printf("write: %u", data);
	data = readl(0xc0000000-4);
	printf("  read: %u\n", data);

	data = 4000;
	writel(data, 0xc0000000-4);
	printf("write: %u", data);
	data = readl(0x80000000-4);
	printf("  read: %u\n", data);

	data = 5000;
	writel(data, 0x80000000);
	printf("write: %u", data);
	data = readl(0xc0000000);
	printf("  read: %u\n", data);
	data = 6000;
	writel(data, 0xc0000000);
	printf("write: %u", data);
	data = readl(0x40000000);
	printf("  read: %u\n", data);

	uart_puts("dram ok!\n");
}

/* shan duoshao weimiao zuixiao dayu 200000 */
void test_timer(int times)
{
	u32 cur = 0;
	u32 start;
	u32 tmo = 1000;
	u32 tmp = 0;

	led_init();
	while (cur++ < times) {	
		start = get_mtimer(0);
		led_tx_off();
		led_rx_on();
		while ((tmp = get_mtimer(start)) < tmo);
		led_tx_on();
		led_rx_off();
		mdelay(1000);
	};
	uart_puts("timer ok!\n");
	/*****************************/
	uart_puts("turn off the led.\n");
	led_tx_off();
	led_rx_off();
}

int main(void)
{
	int power_failed = 0;

	watchdog_init();
	clock_init();
	gpio_init();
	uart_init();
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	sunxi_dram_init();
	power_failed |= axp209_init();
	power_failed |= axp209_set_dcdc2(1400);
	power_failed |= axp209_set_dcdc3(1250);
	power_failed |= axp209_set_ldo2(3000);
	power_failed |= axp209_set_ldo3(2800);
	power_failed |= axp209_set_ldo4(2800);
	if (!power_failed)
		clock_set_pll1(1008000000);
	else
		printf("Failed to set core voltage!. Can't set CPU frequency\n");
	timer_init_all();
	sunxi_mmc_init(0);

	//test_timer(10); /* fanle? */
	//test_dram();
	spl_mmc_load_image();
	uart_puts("now go hang\n");
	led_hang(10000000);
	return 0;
}
