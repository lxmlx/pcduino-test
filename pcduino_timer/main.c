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

void test_dram()
{
	ulong data;

	data = 0;
	writel(5000,0x80000000-4);
	data = readl(0x80000000-4);
	uart_puts("delay: ");
	uart_puts(simple_itoa(data));
	uart_puts(" ms\n");
	__udelay(data);

	data = 0;
	writel(5000,0x80000000);
	data = readl(0x80000000);
	uart_puts("delay: ");
	uart_puts(simple_itoa(data));
	uart_puts(" ms\n");
	__udelay(data);

	data = 0;
	writel(5000,0xc0000000-4);
	data = readl(0xc0000000-4);
	uart_puts("delay: ");
	uart_puts(simple_itoa(data));
	uart_puts(" ms\n");
	__udelay(data);

	data = 0;
	writel(5000,0xc0000000);
	data = readl(0xc0000000);
	uart_puts("delay: ");
	uart_puts(simple_itoa(data));
	uart_puts(" ms\n");
	__udelay(data);

	uart_puts("dram ok!\n");
}

void test_uart()
{
	uart_puts("uart ok!\n");
}

/* shan duoshao weimiao zuixiao dayu 200000 */
void test_timer(int times)
{
	int cur;

	led_init();

	cur = 0;
	while (cur++ < times) {
	//while(0) {	
		led_tx_off();
		led_rx_on();
		timer0_cndelay(2400000); /* 24 * 1000 * 1000 */
		led_tx_on();
		led_rx_off();
		timer1_cndelay(600000); /* 24 * 1000 * 1000 / 4 */
	};
	cur = 0;
	while (cur++ < times) {
	//while(0) {	
		led_tx_off();
		led_rx_on();
		timer0_udelay(100000);
		led_tx_on();
		led_rx_off();
		timer1_udelay(100000);
	};
	uart_puts("timer0,1 ok!\n");
	cur = 0;
	while (cur++ < times) {
	//while(0) {
		led_tx_off();
		led_rx_on();
		timer2_cndelay(300000); /* 24 * 1000 * 1000 / 8 */
		led_tx_on();
		led_rx_off();
		timer4_cndelay(150000); /* 24 * 1000 * 1000 / 16 */
	};
	cur = 0;
	while (cur++ < times) {
	//while(0) {
		led_tx_off();
		led_rx_on();
		timer2_udelay(100000);
		led_tx_on();
		led_rx_off();
		timer4_mdelay(100);
	};
	uart_puts("timer2,4 ok!\n");
	cur = 0;
	while (cur++ < times) {
	//while(0) {
		led_tx_off();
		led_rx_on();
		timer4_cndelay(150000);
		led_tx_on();
		led_rx_off();
		timer5_cndelay(37500); 
	};
	cur = 0;
	while (cur++ < times) {
	//while(0) {
		led_tx_off();
		led_rx_on();
		timer4_mdelay(100);
		led_tx_on();
		led_rx_off();
		timer5_mdelay(100);
	};
	uart_puts("timer4,5 ok!\n");
	cur = 0;
	while (cur++ < times) {
	//while(0) {
		led_tx_off();
		led_rx_on();
		avscnt0_cndelay(100);
		led_tx_on();
		led_rx_off();
		avscnt1_cndelay(100);
	}
	uart_puts("avs0,1 ok!\n");
/* avs seems not work corect */


/*****************************/
	uart_puts("turn off the led.\n");
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
	test_uart();
	test_timer(10); /* fanle? */
	test_dram();
	uart_puts("now go hang\n");
	led_hang(10000000);
	return 0;
}
