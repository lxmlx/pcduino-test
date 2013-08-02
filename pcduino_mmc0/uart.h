#ifndef _UART_H
#define _UART_H

#define UART_BASE	0x01c28000
#define UART(_nr)	(UART_BASE + (_nr) * 0x400)

#define UART_RBR_OFFSET		0x00
#define UART_THR_OFFSET		0x00
#define UART_DLL_OFFSET		0x00
#define UART_DLH_OFFSET		0x04
#define UART_IER_OFFSET		0x04
#define UART_IIR_OFFSET		0x08
#define UART_FCR_OFFSET		0x08
#define UART_LCR_OFFSET		0x0c
#define UART_MCR_OFFSET		0x10
#define UART_LSR_OFFSET		0x14
#define UART_MSR_OFFSET		0x18
#define UART_SCH_OFFSET		0x1c
#define UART_USR_OFFSET		0x7c
#define UART_TFL_OFFSET		0x80
#define UART_RFL_OFFSET		0x84
#define UART_HALT_OFFSET	0xa4

#define UART_RBR(_nr)		(UART(_nr) + UART_RBR_OFFSET)
#define UART_THR(_nr)		(UART(_nr) + UART_THR_OFFSET)
#define UART_DLL(_nr)		(UART(_nr) + UART_DLL_OFFSET)
#define UART_DLH(_nr)		(UART(_nr) + UART_DLH_OFFSET)
#define UART_IER(_nr)		(UART(_nr) + UART_IER_OFFSET)
#define UART_IIR(_nr)		(UART(_nr) + UART_IIR_OFFSET)
#define UART_FCR(_nr)		(UART(_nr) + UART_FCR_OFFSET)
#define UART_LCR(_nr)		(UART(_nr) + UART_LCR_OFFSET)
#define UART_MCR(_nr)		(UART(_nr) + UART_MCR_OFFSET)
#define UART_LSR(_nr)		(UART(_nr) + UART_LSR_OFFSET)
#define UART_MSR(_nr)		(UART(_nr) + UART_MSR_OFFSET)
#define UART_SCH(_nr)		(UART(_nr) + UART_SCH_OFFSET)
#define UART_USR(_nr)		(UART(_nr) + UART_USR_OFFSET)
#define UART_TFL(_nr)		(UART(_nr) + UART_TFL_OFFSET)
#define UART_RFL(_nr)		(UART(_nr) + UART_RFL_OFFSET)
#define UART_HALT(_nr)		(UART(_nr) + UART_HALT_OFFSET)

#define BAUD_115200	(0xd) /* 24 * 1000 * 1000 / 16 / 115200 = 13 */
#define NO_PARITY	(0)
#define ONE_STOP_BIT	(0)
#define DAT_LEN_8_BITS	(3)
#define LC_8_N_1	(NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

#define CONFIG_SYS_NS16550_IER	0x00

/*
 * These are the definitions for the FIFO Control Register
 */
#define UART_FCR_FIFO_EN	0x01 /* Fifo enable */
#define UART_FCR_CLEAR_RCVR	0x02 /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT	0x04 /* Clear the XMIT FIFO */
#define UART_FCR_DMA_SELECT	0x08 /* For DMA applications */
#define UART_FCR_TRIGGER_MASK	0xC0 /* Mask for the FIFO trigger range */
#define UART_FCR_TRIGGER_1	0x00 /* Mask for trigger set at 1 */
#define UART_FCR_TRIGGER_4	0x40 /* Mask for trigger set at 4 */
#define UART_FCR_TRIGGER_8	0x80 /* Mask for trigger set at 8 */
#define UART_FCR_TRIGGER_14	0xC0 /* Mask for trigger set at 14 */

#define UART_FCR_RXSR		0x02 /* Receiver soft reset */
#define UART_FCR_TXSR		0x04 /* Transmitter soft reset */

/*
 * These are the definitions for the Modem Control Register
 */
#define UART_MCR_DTR	0x01		/* DTR   */
#define UART_MCR_RTS	0x02		/* RTS   */
#define UART_MCR_OUT1	0x04		/* Out 1 */
#define UART_MCR_OUT2	0x08		/* Out 2 */
#define UART_MCR_LOOP	0x10		/* Enable loopback test mode */

#define UART_MCR_DMA_EN	0x04
#define UART_MCR_TX_DFR	0x08

/*
 * These are the definitions for the Line Control Register
 *
 * Note: if the word length is 5 bits (UART_LCR_WLEN5), then setting
 * UART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
 */
#define UART_LCR_WLS_MSK 0x03		/* character length select mask */
#define UART_LCR_WLS_5	0x00		/* 5 bit character length */
#define UART_LCR_WLS_6	0x01		/* 6 bit character length */
#define UART_LCR_WLS_7	0x02		/* 7 bit character length */
#define UART_LCR_WLS_8	0x03		/* 8 bit character length */
#define UART_LCR_STB	0x04		/* # stop Bits, off=1, on=1.5 or 2) */
#define UART_LCR_PEN	0x08		/* Parity eneble */
#define UART_LCR_EPS	0x10		/* Even Parity Select */
#define UART_LCR_STKP	0x20		/* Stick Parity */
#define UART_LCR_SBRK	0x40		/* Set Break */
#define UART_LCR_BKSE	0x80		/* Bank select enable */
#define UART_LCR_DLAB	0x80		/* Divisor latch access bit */

/*
 * These are the definitions for the Line Status Register
 */
#define UART_LSR_DR	0x01		/* Data ready */
#define UART_LSR_OE	0x02		/* Overrun */
#define UART_LSR_PE	0x04		/* Parity error */
#define UART_LSR_FE	0x08		/* Framing error */
#define UART_LSR_BI	0x10		/* Break */
#define UART_LSR_THRE	0x20		/* Xmit holding register empty */
#define UART_LSR_TEMT	0x40		/* Xmitter empty */
#define UART_LSR_ERR	0x80		/* Error */

#define UART_MSR_DCD	0x80		/* Data Carrier Detect */
#define UART_MSR_RI	0x40		/* Ring Indicator */
#define UART_MSR_DSR	0x20		/* Data Set Ready */
#define UART_MSR_CTS	0x10		/* Clear to Send */
#define UART_MSR_DDCD	0x08		/* Delta DCD */
#define UART_MSR_TERI	0x04		/* Trailing edge ring indicator */
#define UART_MSR_DDSR	0x02		/* Delta DSR */
#define UART_MSR_DCTS	0x01		/* Delta CTS */

/*
 * These are the definitions for the Interrupt Identification Register
 */
#define UART_IIR_NO_INT	0x01	/* No interrupts pending */
#define UART_IIR_ID	0x06	/* Mask for the interrupt ID */

#define UART_IIR_MSI	0x00	/* Modem status interrupt */
#define UART_IIR_THRI	0x02	/* Transmitter holding register empty */
#define UART_IIR_RDI	0x04	/* Receiver data interrupt */
#define UART_IIR_RLSI	0x06	/* Receiver line status interrupt */

/*
 * These are the definitions for the Interrupt Enable Register
 */
#define UART_IER_MSI	0x08	/* Enable Modem status interrupt */
#define UART_IER_RLSI	0x04	/* Enable receiver line status interrupt */
#define UART_IER_THRI	0x02	/* Enable Transmitter holding register int. */
#define UART_IER_RDI	0x01	/* Enable receiver data interrupt */

/* useful defaults for LCR */
#define UART_LCR_8N1	0x03


#define UART_LCRVAL UART_LCR_8N1		/* 8 data, 1 stop, no parity */
#define UART_MCRVAL (UART_MCR_DTR | \
		     UART_MCR_RTS)		/* RTS/DTR */
#define UART_FCRVAL (UART_FCR_FIFO_EN |	\
		     UART_FCR_RXSR |	\
		     UART_FCR_TXSR)		/* Clear & enable FIFOs */
#define serial_out(v, a)	writeb(v, a)
#define serial_in(v)		readb(v)

void uart_init(void);
char uart_getchar(void);
void uart_putchar(char c);
void uart_puts(char *str);
int printf(const char *fmt, ...);

#endif
