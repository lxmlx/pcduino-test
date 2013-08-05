#include "image.h"
#include "mmc.h"
#include "common.h"
#include "uart.h"
#include "spl.h"

#ifndef CONFIG_SYS_UBOOT_START
#define CONFIG_SYS_UBOOT_START	CONFIG_SYS_TEXT_BASE
#endif

struct spl_image_info spl_image;

static void jump_to_image_no_args(struct spl_image_info *spl_image)
{
	typedef void (*image_entry_noargs_t)(void);

	image_entry_noargs_t image_entry =
			(image_entry_noargs_t) spl_image->entry_point;
	printf("Jumping to entry point: 0x%x\n\n", spl_image->entry_point);
	image_entry();/* here jump */
}

static int mmc_load_image_raw(struct mmc *mmc, unsigned long sector)
{
	unsigned long err;
	u32 image_size_sectors;

	spl_image.size = CONFIG_SYS_MONITOR_LEN;
	spl_image.entry_point = CONFIG_SYS_UBOOT_START;
	spl_image.load_addr = CONFIG_SYS_TEXT_BASE;
	spl_image.os = IH_OS_U_BOOT;
	spl_image.name = "U-Boot";

	/* convert size to sectors - round up */
	image_size_sectors = (spl_image.size + mmc->read_bl_len - 1) /
				mmc->read_bl_len;

	/* Read the header too to avoid extra memcpy */
	err = mmc_bread(0, sector, image_size_sectors,
				(void *)spl_image.load_addr);

	if (err == 0) {
		printf("spl: mmc blk read err - %lu\n", err);
	}
	return (err == 0);
}

void spl_mmc_load_image(void)
{
	struct mmc *mmc;
	int err;

	mmc = mmc_get_dev(0);
	printf("\n\nLoading boot image...");
	err = mmc_load_image_raw(mmc,
		CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR);

	if (err) {
		printf("load_image_error!\n");
		return;
	}
	printf("done!\n");
	jump_to_image_no_args(&spl_image);
}

