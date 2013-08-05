#include "watchdog.h"
#include "clock.h"
#include "gpio.h"
#include "global_data.h"
#include "stddef.h"
#include "initcall.h"
#include "timer.h"
#include "i2c.h"
#include "axp209.h"
#include "mmc.h"
#include "sections.h"
#include "malloc.h"
#include "string.h"
#include "memsize.h"

DECLARE_GLOBAL_DATA_PTR;

#define UARTPRINTF 1
#define DEBUG 1

#if UARTPRINTF
#include "uart.h"
	#if DEBUG
	#define debug(fmt, args...) printf(fmt, ##args)
	#else
	#define debug(fmt, args...)
	#endif
#else
#define debug(fmt, args...)
#define printf(fmt, args...)
#endif

/* do some early init */
void s_init(void)
{
	int power_failed = 0;

	watchdog_init();
	clock_init();
	gpio_init();
	uart_init();
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	power_failed |= axp209_init();
	power_failed |= axp209_set_dcdc2(1400);
	power_failed |= axp209_set_dcdc3(1250);
	power_failed |= axp209_set_ldo2(3000);
	power_failed |= axp209_set_ldo3(2800);
	power_failed |= axp209_set_ldo4(2800);
	if (!power_failed)
		clock_set_pll1(1008000000);
	else
		printf("Failed to set core voltage!. Can't set CPU frequency\n");
	timer_init();
	sunxi_mmc_init(0);
}

static int display_banner(void)
{
	printf("\n\nU-boot 2013-08-04 lvrenyang\n\n");
	debug("U-Boot code: %08lX -> %08lX  BSS: -> %08lX\n",
	       _TEXT_BASE,
	       _bss_start_ofs + _TEXT_BASE, _bss_end_ofs + _TEXT_BASE);

	return (0);
}

static int print_cpuinfo(void)
{
	printf("CPU: Allwinner A10 (SUN4I)\n");

	return 0;
}

static int checkboard(void)
{
	printf("Board: pcDuino\n");

	return 0;
}

static int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, 1 << 30);

	return 0;
}

init_fnc_t init_sequence[] = {
	display_banner,		/* say that we are here */
	print_cpuinfo,		/* display cpu info (and speed) */
	checkboard,		/* display board info */
	dram_init,
	NULL,
};

void board_init_f(ulong bootflag)
{
	bd_t *bd;
	init_fnc_t *init_fnc_ptr;
	gd_t *id;
	ulong addr, addr_sp;

	void *new_fdt = NULL;
	size_t fdt_size = 0;

	memset((void *)gd, 0, sizeof(gd_t));

	gd->mon_len = _bss_end_ofs;

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			panic("init_fnc_ptr error. now sleeping...\n");
		}
	}

	debug("monitor len: %08lX\n", gd->mon_len);
	/*
	 * Ram is setup, size stored in gd !!
	 */
	debug("ramsize: %08lX\n", gd->ram_size);

	addr = CONFIG_SYS_SDRAM_BASE + gd->ram_size;

	while(1);
}
