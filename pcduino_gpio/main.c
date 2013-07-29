#include "clock.h"
#include "gpio.h"

void loop(unsigned int n)
{
	unsigned char i;
	for (i = 0; i < n; i ++) {
                gpio_set(0x0);
                delay(100);
                gpio_set(0x0fffffff);
                delay(100);
	}
}

int main(void)
{
	clock_init(); /* 初始化时钟 */
	gpio_init();

	while (1) {
		/*gpio_set(0x0);
		delay(0x300000);
		gpio_set(0x0fffffff);
		delay(0x100000);*/
		loop(100000);
	}

	return 0;
}
