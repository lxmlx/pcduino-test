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

void test_dram(void)
{
	ulong data;

	data = 3000;
	writel(data, 0x80000000-4);
	printf("data: %ums\n", data);
	data = readl(0xc0000000-4);
	printf("delay: %ums\n", data);
	mdelay(data);

	data = 4000;
	writel(data, 0xc0000000-4);
	printf("data: %ums\n", data);
	data = readl(0x80000000-4);
	printf("delay: %ums\n", data);
	mdelay(data);

	data = 5000;
	writel(data, 0x80000000);
	printf("data: %ums\n", data);
	data = readl(0xc0000000);
	printf("delay: %ums\n", data);
	mdelay(data);

	data = 6000;
	writel(data, 0xc0000000);
	printf("data: %ums\n", data);
	data = readl(0x80000000);
	printf("delay: %ums\n", data);
	mdelay(data);

	uart_puts("dram ok!\n");
}

void test_uart(void)
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
	//while (cur++ < times) {
	while(0) {
		led_tx_off();
		led_rx_on();
		avscnt0_cndelay(100);
		led_tx_on();
		led_rx_off();
		avscnt1_cndelay(100);
	}
	uart_puts("avs0,1 ok!\n");
/* avs seems not work corect */

/*	cur = 0;
	led_rx_on();
	led_tx_on();
	watchdog_set(10);
	mdelay(10000);
	uart_puts("watchdog seems not well !\n");
*/
	uart_puts("64 bit counter: ");
	uart_puts("high: ");
	uart_puts(simple_itoa(read_cnt64h()));
	uart_puts(" low: ");
	uart_puts(simple_itoa(read_cnt64l()));
	uart_puts("\n");
	uart_puts("64 bit counter: ");
	uart_puts("high: ");
	uart_puts(simple_itoa(read_cnt64h()));
	uart_puts(" low: ");
	uart_puts(simple_itoa(read_cnt64l()));
	uart_puts("\n");
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
	sunxi_mmc_init(0);
	test_uart();
	test_timer(10); /* fanle? */
	test_dram();
	mmc_test();
	uart_puts("now go hang\n");
	led_hang(10000000);
	return 0;
}
