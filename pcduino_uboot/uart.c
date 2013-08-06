#include "uart.h"
#include "io.h"
#include "types.h"
#include "string.h"
#include "sunxi-common.h"

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

int uart_testchar(int port)
{
	return (readl(UART_LSR(port)) & UART_LSR_DR) != 0;
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

void panic(const char *fmt, ...)
{
	int i = 10000000, j = 0;
	va_list args;
	va_start(args, fmt);
	printf(fmt, args);

	while(1) {
		uart_putchar('z');
		for(j = i; j > 0; j--);
	}

	return;
}

/* test if ctrl-c was pressed */
static int ctrlc_disabled = 0;	/* see disable_ctrl() */
static int ctrlc_was_pressed = 0;
int ctrlc(void)
{
	if (!ctrlc_disabled) {
		if (uart_testchar(0)) {
			switch (uart_getchar()) {
			case 0x03:		/* ^C - Control C */
				ctrlc_was_pressed = 1;
				return 1;
			default:
				break;
			}
		}
	}
	return 0;
}

/* pass 1 to disable ctrlc() checking, 0 to enable.
 * returns previous state
 */
int disable_ctrlc(int disable)
{
	int prev = ctrlc_disabled;	/* save previous state */

	ctrlc_disabled = disable;
	return prev;
}

int had_ctrlc (void)
{
	return ctrlc_was_pressed;
}

void clear_ctrlc(void)
{
	ctrlc_was_pressed = 0;
}
