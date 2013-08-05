#include "gpio.h"

int gpio_init(void)
{
	sunxi_gpio_set_cfgpin(SUNXI_GPB(22), SUN4I_GPB22_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(23), SUN4I_GPB23_UART0_RX);
	sunxi_gpio_set_pull(SUNXI_GPB(22), 1);
	sunxi_gpio_set_pull(SUNXI_GPB(23), 1);
	return 0;
}
