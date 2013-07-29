#include "clock.h"

#define UART0_LCR   (*(volatile unsigned int *)0x01C2800C) 
#define UART0_DLH   (*(volatile unsigned int *)0x01C28004) 
#define UART0_DLL   (*(volatile unsigned int *)0x01C28000) 
#define UART0_LSR   (*(volatile unsigned int *)0x01C28014) 
#define UART0_THR   (*(volatile unsigned int *)0x01C28000) 
#define UART0_RBR   (*(volatile unsigned int *)0x01C28000) 
#define UART0_USR   (*(volatile unsigned int *)0x01C2807c)

void uart_init(void)
{
	while(UART0_USR & (1<<0)); /* 当UART0_USR[0]为0时，跳出循环 */
	/* 设置 数据位 奇偶校验位 停止位 */
	UART0_LCR = ((1<<7) | (0<<3) | (0<<2) | (2<<0));
	/*set baudrate*/
	UART0_DLH = 0;
	UART0_DLL = 13;
	sdelay(200);
	while(UART0_USR & (1<<0));
	UART0_LCR &= ~(1<<7);
}

char uart_getchar(void)
{
	char c;
	/* 查询状态寄存器，直到有有效数据 */
	while (!(UART0_LSR & (1<<0)));
	c = UART0_RBR; /* 读取接收寄存器的值 */
	return c;
}

void uart_putchar(char c)
{
	UART0_THR = c;
	/* 查询状态寄存器，直到发送缓存为空 */
	while (!(UART0_LSR & (1<<5)));
	return;
}
