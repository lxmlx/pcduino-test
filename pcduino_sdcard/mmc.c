#include "mmc.h"
#include "list.h"
#include "u-boot.h"

/* Set block count limit because of 16 bit register limit on some hardware*/
#ifndef CONFIG_SYS_MMC_MAX_BLK_COUNT
#define CONFIG_SYS_MMC_MAX_BLK_COUNT 65535
#endif

static struct list_head mmc_devices;
static int cur_dev_num = -1;

/* xianshishi luobanchengxu, zhege kuangjia yihou zailai yanjiu */
int mmc_register(struct mmc *mmc)
{
	/* Setup the universal parts of the block interface just once */
	mmc->block_dev.if_type = IF_TYPE_MMC;
	mmc->block_dev.dev = cur_dev_num++;
	mmc->block_dev.removable = 1;
//	mmc->block_dev.block_read = mmc_bread;
//	mmc->block_dev.block_write = mmc_bwrite;
//	mmc->block_dev.block_erase = mmc_berase;
	if (!mmc->b_max)
		mmc->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	INIT_LIST_HEAD (&mmc->link);

	list_add_tail(&mmc->link, &mmc_devices);

	return 0;
}

int mmc_start_init(struct mmc *mmc)
{
	int err = 0;

	return err;
}

static void do_preinit(void)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		if (m->preinit)
			mmc_start_init(m);
	}
}

int mmc_initialize(bd_t *bis)
{
	INIT_LIST_HEAD(&mmc_devices);
	cur_dev_num = 0;

	sunxi_mmc_init(0);

	do_preinit();
	return 0;
}
