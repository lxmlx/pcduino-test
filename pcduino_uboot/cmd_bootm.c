#include "command.h"
#include "uart.h"
#include "types.h"
#include "sunxi-common.h"
#include "common.h"
#include "vsprintf.h"
#include "global_data.h"
#include "image.h"
#include "malloc.h"
#include "env_common.h"
#include "bootm.h"
#include "stddef.h"
#include "mmc.h"
#include "interrupts.h"
#include "cmd_nvedit.h"
#include "compiler.h"
#include "config_defaults.h"
#include "cpu.h"

#define DEBUG 1

#if DEBUG
#define debug(fmt, args...) printf(fmt, ##args)
#else
#define debug(fmt, args...)
#endif

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SYS_BOOTM_LEN
#define CONFIG_SYS_BOOTM_LEN	0x800000	/* use 8MByte as default max gunzip size */
#endif

bootm_headers_t images;		/* pointers to os/initrd/fdt images */

static void boot_start_lmb(bootm_headers_t *images)
{
	lmb_init(&images->lmb);
}

/**
 * genimg_get_image - get image from special storage (if necessary)
 * @img_addr: image start address
 *
 * genimg_get_image() checks if provided image start adddress is located
 * in a dataflash storage. If so, image is moved to a system RAM memory.
 *
 * returns:
 *     image start address after possible relocation from special storage
 */
/* i stored the kernel image at sdcard, block offset 2048, block_size 512 */ 
ulong genimg_get_image(ulong img_addr)
{
	ulong ram_addr = img_addr;
	struct mmc *mmc;
	u32 image_size_sectors;
	u32 sector = 2048; /* offset = 1M */
	u32 size = 5 << 20; /* size = 5M */
	int err;

	mmc = mmc_get_dev(0);
	printf("loading image form mmc0 ... ");
		/* convert size to sectors - round up */
	image_size_sectors = (size + mmc->read_bl_len - 1) /
				mmc->read_bl_len;

	/* Read the header too to avoid extra memcpy */
	err = mmc_bread(0, sector, image_size_sectors,
				(void *)ram_addr);

	if (err == 0)
		printf("spl: mmc blk read err - %lu\n", err);
	else
		printf(" done.\n\n");

	return ram_addr;
}

/**
 * image_get_kernel - verify legacy format kernel image
 * @img_addr: in RAM address of the legacy format image to be verified
 * @verify: data CRC verification flag
 *
 * image_get_kernel() verifies legacy image integrity and returns pointer to
 * legacy image header if image verification was completed successfully.
 *
 * returns:
 *     pointer to a legacy image header if valid image was found
 *     otherwise return NULL
 */
static image_header_t *image_get_kernel(ulong img_addr, int verify)
{
	image_header_t *hdr = (image_header_t *)img_addr;

	if (!image_check_magic(hdr)) {
		printf("Bad Magic Number\n");
		return NULL;
	}

	if (!image_check_hcrc(hdr)) {
		printf("Bad Header Checksum\n");
		return NULL;
	}

	/* i have no more time, so this point we will not print */
	/* image_print_contents(hdr); */

	if (verify) {
		printf(" ### Verifying Checksum ... ");
		if (!image_check_dcrc(hdr)) {
			printf("Bad Data CRC\n");
			return NULL;
		}
		printf("OK\n");
	}

	if (!image_check_target_arch(hdr)) {
		printf("Unsupported Architecture 0x%x\n", image_get_arch(hdr));
		return NULL;
	}
	return hdr;
}

/**
 * boot_get_kernel - find kernel image
 * @os_data: pointer to a ulong variable, will hold os data start address
 * @os_len: pointer to a ulong variable, will hold os data length
 *
 * boot_get_kernel() tries to find a kernel image, verifies its integrity
 * and locates kernel data.
 *
 * returns:
 *     pointer to image header if valid image was found, plus kernel start
 *     address and length, otherwise NULL
 */
static const void *boot_get_kernel(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[], bootm_headers_t *images,
				ulong *os_data, ulong *os_len)
{
	image_header_t *hdr;
	ulong img_addr;
	const void *buf;

	/* find out kernel image address */
	if (argc < 2) {
		img_addr = load_addr;
		debug(" *** kernel: default image load address = 0x%08lx\n",
			load_addr);
	} else {
		img_addr = simple_strtoul(argv[1], NULL, 16);
		debug(" *** kernel: cmdline image address = 0x%08lx\n", img_addr);
	}

	/* copy from dataflash if needed */
	img_addr = genimg_get_image(img_addr);

	/* check image type, for FIT images get FIT kernel node */
	*os_data = *os_len = 0;
	buf = (void *)(unsigned long int)img_addr;
	switch (genimg_get_format(buf)) {
	case IMAGE_FORMAT_LEGACY:
		printf(" ### Booting kernel from Legacy Image at %08lx ...\n",
			img_addr);
		hdr = image_get_kernel(img_addr, images->verify);
		if (!hdr)
			return NULL;

		/* get os_data and os_len */
		switch (image_get_type(hdr)) {
		case IH_TYPE_KERNEL:
		case IH_TYPE_KERNEL_NOLOAD:
			*os_data = image_get_data(hdr);
			*os_len = image_get_data_size(hdr);
			break;
		default:
			printf("Wrong Image Type for %s command\n",
				cmdtp->name);
			return NULL;
		}

		/*
		 * copy image header to allow for image overwrites during
		 * kernel decompression.
		 */
		memmove(&images->legacy_hdr_os_copy, hdr,
			sizeof(image_header_t));

		/* save pointer to image header */
		images->legacy_hdr_os = hdr;

		images->legacy_hdr_valid = 1;
		break;

	default:
		printf("Wrong Image Format for %s command\n", cmdtp->name);
		return NULL;
	}

	debug(" *** kernel data at 0x%08lx, len = 0x%08lx (%ld)\n",
		*os_data, *os_len, *os_len);

	return buf;
}

/* read the image header and copy to memory
 * then jump to entry point
 */
static int bootm_start(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	const void *os_hdr;

	memset((void *)&images, 0, sizeof(images));
	images.verify = getenv_yesno("verify");

	boot_start_lmb(&images);

	os_hdr = boot_get_kernel(cmdtp, flag, argc, argv,
			&images, &images.os.image_start, &images.os.image_len);
	if (images.os.image_len == 0) {
		printf("ERROR: can't get kernel image!\n");
		return 1;
	}

	/* get image parameters */
	switch (genimg_get_format(os_hdr)) {
	case IMAGE_FORMAT_LEGACY:
		images.os.type = image_get_type(os_hdr);
		images.os.comp = image_get_comp(os_hdr);
		images.os.os = image_get_os(os_hdr);
		images.os.end = image_get_image_end(os_hdr);
		images.os.load = image_get_load(os_hdr);
		break;

	default:
		printf("ERROR: unknown image format type!\n");
		return 1;
	}

	/* find kernel entry point */
	if (images.legacy_hdr_valid) {
		images.ep = image_get_ep(&images.legacy_hdr_os_copy);
	} else {
		printf("Could not find kernel entry point!\n");
		return 1;
	}

	if (images.os.type == IH_TYPE_KERNEL_NOLOAD) {
		images.os.load = images.os.image_start;
		images.ep += images.os.load;
	}

	/* i don't know what's the ramdisk, so here just skip it */

	images.os.start = (ulong)os_hdr;
	images.state = BOOTM_STATE_START;
	return 0;
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

//	r2 = gd->bd->bi_boot_params;

	kernel_entry(0, machid, NULL);

}

#define BOOTM_ERR_RESET		-1
#define BOOTM_ERR_OVERLAP	-2
#define BOOTM_ERR_UNIMPLEMENTED	-3
static int bootm_load_os(image_info_t os, ulong *load_end, int boot_progress)
{
	uint8_t comp = os.comp;
	ulong load = os.load;
	ulong blob_start = os.start;
	ulong blob_end = os.end;
	ulong image_start = os.image_start;
	ulong image_len = os.image_len;
	__maybe_unused uint unc_len = CONFIG_SYS_BOOTM_LEN;
	int no_overlap = 0;
	void *load_buf, *image_buf;

	const char *type_name = "lvrenyang";

	load_buf = (void *)(unsigned long int)load;
	image_buf = (void *)(unsigned long int)image_start;
	switch (comp) {
	case IH_COMP_NONE:
		if (load == blob_start || load == image_start) {
			printf("   XIP %s ... ", type_name);
			no_overlap = 1;
		} else {
			printf("   Loading %s ... ", type_name);
			memmove_wd(load_buf, image_buf, image_len, CHUNKSZ);
		}
		*load_end = load + image_len;
		printf("OK\n");
		break;
#ifdef CONFIG_GZIP
	case IH_COMP_GZIP:
		printf("   Uncompressing %s ... ", type_name);
		/* gunzip is so hard that i want yamede */
		if (/*gunzip(load_buf, unc_len, image_buf, &image_len) != 0*/!0) {
			printf("GUNZIP: uncompress, out-of-mem or overwrite "
				"error - must RESET board to recover\n");
			return BOOTM_ERR_RESET;
		}

		*load_end = load + image_len;
		break;
#endif /* CONFIG_GZIP */
	default:
		printf("Unimplemented compression type %d\n", comp);
		return BOOTM_ERR_UNIMPLEMENTED;
	}

	flush_cache(load, (*load_end - load) * sizeof(ulong));

	printf("OK\n");
	debug("   kernel loaded at 0x%08lx, end = 0x%08lx\n", load, *load_end);

	if (!no_overlap && (load < blob_end) && (*load_end > blob_start)) {
		debug("images.os.start = 0x%lX, images.os.end = 0x%lx\n",
			blob_start, blob_end);
		debug("images.os.load = 0x%lx, load_end = 0x%lx\n", load,
			*load_end);

		return BOOTM_ERR_OVERLAP;
	}

	return 0;
}

/*******************************************************************/
/* bootm - boot application image from image in memory */
/*******************************************************************/
int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *endp;
	ulong load_end = 0;
	int ret;

#ifdef DEBUG
	int i;
	debug("do_bootm: flag: %d, argc: %d, argv: ",
		flag, argc);
	for(i = 0; i < argc; i++)
		debug("%s, ", argv[i]);
	debug("\n");
#endif

	if (argc > 1) {
		simple_strtoul(argv[1], &endp, 16);

		if (*endp != 0)
			return -1;
	}

	/* ************************************** 
	 * here i only want to boot the kernel
	 * and i will not check anything 
	 * but it seems not possible, so we get
	 * the image header first , and then boot
	 * the kernel.
	 * **************************************
	 */

	if (bootm_start(cmdtp, flag, argc, argv))
		return 1;

	/*
	 * We have reached the point of no return: we are going to
	 * overwrite all exception vector code, so we cannot easily
	 * recover from any failures any more...
	 */
	disable_interrupts();

	ret = bootm_load_os(images.os, &load_end, 1);
	if (ret < 0)
		return 1;

	boot_jump_linux(&images);

	return 1;
}

#ifdef CONFIG_SYS_LONGHELP
static char bootm_help_text[] =
	"[addr [arg ...]]\n    - boot application image stored in memory\n"
	"\tpassing arguments 'arg ...'; when booting a Linux kernel,\n"
	"\t'arg' can be the address of an initrd image\n";
#endif

U_BOOT_CMD(
	bootm, CONFIG_SYS_MAXARGS, 1, do_bootm,
	"boot application image from memory", bootm_help_text
);
