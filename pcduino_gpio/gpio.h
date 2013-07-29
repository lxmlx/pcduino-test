#ifndef _GPIO_H
#define _GPIO_H

#define PIO_BASE_ADDR		0x01c20800

#define PH_CFG0_OFFSET		0xfc
#define PH_CFG0			(*(volatile unsigned int *)(PIO_BASE_ADDR + PH_CFG0_OFFSET))
#define PH_CFG1_OFFSET		0x100
#define PH_CFG1			(*(volatile unsigned int *)(PIO_BASE_ADDR + PH_CFG1_OFFSET))
#define PH_DAT_OFFSET		0x10c
#define PH_DAT			(*(volatile unsigned int *)(PIO_BASE_ADDR + PH_DAT_OFFSET))
#define PH_DRI0_OFFSET		0x110
#define PH_DRI0			(*(volatile unsigned int *)(PIO_BASE_ADDR + PH_DRI0_OFFSET))
#define PH_DRI1_OFFSET		0x114
#define PH_DRI1			(*(volatile unsigned int *)(PIO_BASE_ADDR + PH_DRI1_OFFSET))
#define PH_PLL0_OFFSET		0x118
#define PH_PLL0			(*(volatile unsigned int *)(PIO_BASE_ADDR + PH_PLL0_OFFSET))
#define PH_PLL1_OFFSET		0x11c
#define PH_PLL1			(*(volatile unsigned int *)(PIO_BASE_ADDR + PH_PLL1_OFFSET))



void gpio_init();
void gpio_set(unsigned int i);

#endif
