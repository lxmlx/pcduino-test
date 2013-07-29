/*
 * 通过对GPIO进行控制来实现串口led灯的亮灭。
 * 此处将复杂操作包装成各种实现。
 */

#include "gpio.h"

int led_init(void)
{
	sunxi_gpio_set_cfgpin(SUNXI_GPH(15), SUNXI_GPIO_OUTPUT);
	sunxi_gpio_set_cfgpin(SUNXI_GPH(16), SUNXI_GPIO_OUTPUT);
	sunxi_gpio_set_pull(SUNXI_GPH(15), 1);
	sunxi_gpio_set_pull(SUNXI_GPH(16), 1);
	return 0;
}

int led_tx_on(void)
{
	sunxi_gpio_set_dat_bit(SUNXI_GPH(15), 1);
	return 0;
}

int led_tx_off(void)
{
	sunxi_gpio_set_dat_bit(SUNXI_GPH(15), 0);
	return 0;

}

int led_rx_on(void)
{
	sunxi_gpio_set_dat_bit(SUNXI_GPH(16), 1);
	return 0;

}

int led_rx_off(void)
{
	sunxi_gpio_set_dat_bit(SUNXI_GPH(16), 0);
	return 0;

}
