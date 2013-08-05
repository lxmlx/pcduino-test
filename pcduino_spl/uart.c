#include "uart.h"
#include "io.h"
#include "types.h"
#include "string.h"
#include "common.h"

void uart_init(void)
{
	while(!(serial_in(UART_LSR(0)) & UART_LSR_TEMT));
	serial_out(CONFIG_SYS_NS16550_IER, UART_IER(0));
	serial_out((UART_LCR_BKSE | UART_LCRVAL), UART_LCR(0));
	serial_out(0, UART_DLL(0));
	serial_out(0, UART_DLH(0));
	serial_out(UART_LCRVAL, UART_LCR(0));
	serial_out(UART_MCRVAL, UART_MCR(0));
	serial_out(UART_FCRVAL, UART_FCR(0));
	serial_out((UART_LCR_BKSE | UART_LCRVAL), UART_LCR(0));
	serial_out(0xd & 0xff, UART_DLL(0));
	serial_out((0xd >> 8) & 0xff, UART_DLH(0));
	serial_out(UART_LCRVAL, UART_LCR(0));
	return;
}

char uart_getchar(void)
{
	/* 查询状态寄存器，直到有有效数据 */
	while (!(readl(UART_LSR(0)) & (1<<0)) );
	return serial_in(UART_RBR(0));
}

void uart_putchar(char c)
{
	/* 查询状态寄存器，直到发送缓存为空 */
	while (!(readl(UART_LSR(0)) & (1<<5)) );
	serial_out(c, UART_THR(0));
	return;
}

void uart_puts(char *str)
{
	while(*str)
		uart_putchar(*str++);
}

int printf(const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CONFIG_SYS_PBSIZE];

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vscnprintf(printbuffer, sizeof(printbuffer), fmt, args);
	va_end(args);

	/* Print the string */
	uart_puts(printbuffer);
	return i;
}
