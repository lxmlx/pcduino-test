#include "image.h"
#include "mmc.h"
#include "common.h"
#include "uart.h"

static int mmc_load_image_raw(struct mmc *mmc, unsigned long sector)
{
	unsigned long err;
	//u32 image_size_sectors;
	struct image_header *header;

	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE -
			sizeof(struct image_header));

	err = mmc_bread(0, sector, 1, header);
	if (err == 0)
		goto end;

end:
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
	err = mmc_load_image_raw(mmc,
		CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR);

	if (err)
		printf("load_image_error!\n");
}
