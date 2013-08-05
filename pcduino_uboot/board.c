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
#include "sun4i.h"
#include "sunxi-common.h"
#include "common.h"

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

void __dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size =  gd->ram_size;
}
void dram_init_banksize(void)
	__attribute__((weak, alias("__dram_init_banksize")));

static int display_dram_config(void)
{
	int i;

#ifdef DEBUG
	printf("RAM Configuration:\n");

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++)
		printf("Bank #%d: %08lx ", i, gd->bd->bi_dram[i].start);
	printf("\n");
#endif
	return (0);
}

static int display_banner(void)
{
	printf("\n\nU-boot 2013-08-04 lvrenyang\n\n");
	debug("U-Boot code: %08lx -> %08lx  BSS: -> %08lx\n",
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
			panic("Init_fnc_ptr error. now sleeping...\n");
		}
	}

	debug("Monitor len: %08lX\n", gd->mon_len);
	/*
	 * Ram is setup, size stored in gd !!
	 */
	debug("Ramsize: %08lX\n", gd->ram_size);

	addr = CONFIG_SYS_SDRAM_BASE + gd->ram_size;

	/* reserve TLB table */
	gd->arch.tlb_size = 4096 * 4;
	addr -= gd->arch.tlb_size;

	/* round down to next 64 kB limit */
	addr &= ~(0x10000 - 1);

	gd->arch.tlb_addr = addr;
	debug("TLB table from 0x%08lx to 0x%08lx\n", addr, addr + gd->arch.tlb_size);

	/* round down to next 4 kB limit */
	addr &= ~(4096 - 1);
	debug("Top of RAM usable for U-Boot at: 0x%08lx\n", addr);

	/*
	 * reserve memory for U-Boot code, data & bss
	 * round down to next 4 kB limit
	 */
	addr -= gd->mon_len;
	addr &= ~(4096 - 1);
	debug("Reserving %ldk for U-Boot at: 0x%08lx\n", gd->mon_len>>10, addr);

	/*
	 * reserve memory for malloc() arena
	 */
	addr_sp = addr - TOTAL_MALLOC_LEN;
	debug("Reserving %ldk for malloc() at: 0x%08lx\n", TOTAL_MALLOC_LEN>>10, addr_sp);

	/*
	 * (permanently) allocate a Board Info struct
	 * and a permanent copy of the "global" data
	 */
	addr_sp -= sizeof(bd_t);
	bd = (bd_t *)addr_sp;
	gd->bd = bd;
	debug("Reserving %zu Bytes for Board Info at: 0x%08lx\n", sizeof(bd_t), addr_sp);

	gd->bd->bi_arch_number = CONFIG_MACH_TYPE; /* board id for Linux */

	addr_sp -= sizeof(gd_t);
	id = (gd_t *)addr_sp;
	debug("Reserving %zu Bytes for Global Data at: 0x%08lx\n", sizeof(gd_t), addr_sp);

	/* setup stackpointer for exeptions */
	gd->irq_sp = addr_sp;

	/* leave 3 words for abort-stack    */
	addr_sp -= 12;

	/* 8-byte alignment for ABI compliance */
	addr_sp &= ~0x07;

	debug("New Stack Pointer is: 0x%08lx\n", addr_sp);

	gd->bd->bi_baudrate = gd->baudrate;
	/* Ram ist board specific, so move it to board code ... */
	dram_init_banksize(); /* seems unuseful */
	display_dram_config();	/* and display it */

	gd->relocaddr = addr;
	gd->start_addr_sp = addr_sp;
	gd->reloc_off = addr - _TEXT_BASE;
	debug("Relocation Offset is: 0x%08lx\n", gd->reloc_off);
	if (new_fdt) {
		memcpy(new_fdt, gd->fdt_blob, fdt_size);
		gd->fdt_blob = new_fdt;
	}
	memcpy(id, (void *)gd, sizeof(gd_t));
}

/************************************************************************
 *
 * This is the next part if the initialization sequence: we are now
 * running from RAM and have a "normal" C environment, i. e. global
 * data can be written, BSS has been cleared, the stack size in not
 * that critical any more, etc.
 *
 ************************************************************************
 */

void board_init_r(gd_t *id, ulong dest_addr)
{
	while(1) {
		printf("z");
		mdelay(1000);
	}
}
