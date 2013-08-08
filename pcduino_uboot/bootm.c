#include "types.h"
#include "global_data.h"
#include "bootm.h"
#include "uart.h"
#include "cpu.h"
#include "setup.h"

DECLARE_GLOBAL_DATA_PTR;

static struct tag *params;

#define DEBUG 1

#if DEBUG
#define debug(fmt, args...) printf(fmt, ##args)
#else
#define debug(fmt, args...)
#endif

static ulong get_sp(void)
{
	ulong ret;

	asm("mov %0, sp" : "=r"(ret) : );
	return ret;
}

void arch_lmb_reserve(struct lmb *lmb)
{
	ulong sp;

	/*
	 * Booting a (Linux) kernel image
	 *
	 * Allocate space for command line and board info - the
	 * address should be as high as possible within the reach of
	 * the kernel (see CONFIG_SYS_BOOTMAPSZ settings), but in unused
	 * memory, which means far enough below the current stack
	 * pointer.
	 */
	sp = get_sp();
	debug("## Current stack ends at 0x%08lx ", sp);

	/* adjust sp by 4K to be safe */
	sp -= 4096;
	lmb_reserve(lmb, sp,
		    gd->bd->bi_dram[0].start + gd->bd->bi_dram[0].size - sp);
}

static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *)bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}

static void setup_memory_tags(bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

		params = tag_next (params);
	}
}

static void setup_end_tag(bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

static void boot_prep_linux(bootm_headers_t *images)
{
	debug("using: ATAGS\n");
	setup_start_tag(gd->bd);
	setup_memory_tags(gd->bd);
	setup_end_tag(gd->bd);
}

static void announce_and_cleanup(void)
{
	printf("\nStarting kernel ...\n\n");
	cleanup_before_linux();
}

/* Subcommand: GO */
static void boot_jump_linux(bootm_headers_t *images)
{
	/* seems gd->bd is not init ? */
	unsigned long machid = 4104;
	void (*kernel_entry)(int zero, int arch, uint params);
	unsigned long r2;

	kernel_entry = (void (*)(int, int, uint))images->ep;

	debug("## Transferring control to Linux (at address %08lx)" \
		"...\n", (ulong) kernel_entry);
	announce_and_cleanup();

	r2 = gd->bd->bi_boot_params;

	kernel_entry(0, machid, r2);

}

int do_bootm_linux(int flag, int argc, char * const argv[], bootm_headers_t *images)
{
	boot_prep_linux(images);
	boot_jump_linux(images);
	return 1;
}
