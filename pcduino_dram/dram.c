#include "dram.h"
#include "io.h"
#include "common.h"
#include "timer.h"
#include "clock.h"

static void mctl_ddr3_reset(void)
{
	struct sunxi_dram_reg *dram =
			(struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	struct sunxi_timer_reg *timer =
			(struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	u32 reg_val;

	writel(0, &timer->cpu_cfg);
	reg_val = readl(&timer->cpu_cfg);
	reg_val >>= 6;
	reg_val &= 0x3;

	if (reg_val != 0) {
		setbits_le32(&dram->mcr, 0x1 << 12);
		sdelay(0x100);
		clrbits_le32(&dram->mcr, 0x1 << 12);
	} else

	{
		clrbits_le32(&dram->mcr, 0x1 << 12);
		sdelay(0x100);
		setbits_le32(&dram->mcr, 0x1 << 12);
	}
}

static void mctl_set_drive(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrsetbits_le32(&dram->mcr, 0x3, (0x6 << 12) | 0xffc);
}

static void mctl_itm_disable(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	setbits_le32(&dram->ccr, 0x1 << 28);
}

static void mctl_itm_enable(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrbits_le32(&dram->ccr, 0x1 << 28);
}

static void mctl_enable_dll0(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrsetbits_le32(&dram->dllcr[0], 0x1 << 30, 0x1 << 31);
	sdelay(0x100);

	clrbits_le32(&dram->dllcr[0], 0x3 << 30);
	sdelay(0x1000);

	clrsetbits_le32(&dram->dllcr[0], 0x1 << 31, 0x1 << 30);
	sdelay(0x1000);
}

/*
 * Note: This differs from pm/standby in that it checks the bus width
 */
static void mctl_enable_dllx(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 i, n, bus_width;

	bus_width = readl(&dram->dcr);
	bus_width >>= 6;
	bus_width &= 7;

	if (bus_width == 3)
		n = 5;
	else
		n = 3;

	for (i = 1; i < n; i++)
		clrsetbits_le32(&dram->dllcr[i], 0x1 << 30, 0x1 << 31);
	sdelay(0x100);

	for (i = 1; i < n; i++)
		clrbits_le32(&dram->dllcr[i], 0x3 << 30);
	sdelay(0x1000);

	for (i = 1; i < n; i++)
		clrsetbits_le32(&dram->dllcr[i], 0x1 << 31, 0x1 << 30);
	sdelay(0x1000);
}

static u32 hpcr_value[32] = {
	0x0301, 0x0301, 0x0301, 0x0301,
	0x0301, 0x0301, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0x1031, 0x1031, 0x0735, 0x1035,
	0x1035, 0x0731, 0x1031, 0x0735,
	0x1035, 0x1031, 0x0731, 0x1035,
	0x1031, 0x0301, 0x0301, 0x0731
};

static void mctl_configure_hostport(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 i;

	for (i = 0; i < 32; i++)
		writel(hpcr_value[i], &dram->hpcr[i]);
}

static void mctl_setup_dram_clock(u32 clk)
{
	u32 reg_val;
	struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* setup DRAM PLL */
	reg_val = readl(&ccm->pll5_cfg);
	reg_val &= ~0x3;
	reg_val |= 0x1;			/* m factor */
	reg_val &= ~(0x3 << 4);
	reg_val |= 0x1 << 4;		/* k factor */
	reg_val &= ~(0x1f << 8);
	reg_val |= ((clk / 24) & 0x1f) << 8;	/* n factor */
	reg_val &= ~(0x3 << 16);
	reg_val |= 0x1 << 16;		/* p factor */
	reg_val &= ~(0x1 << 29);	/* PLL on */
	reg_val |= (u32) 0x1 << 31;	/* PLL En */
	writel(reg_val, &ccm->pll5_cfg);
	sdelay(0x100000);

	setbits_le32(&ccm->pll5_cfg, 0x1 << 29);

	/* reset GPS */
	clrbits_le32(&ccm->gps_clk_cfg, 0x3);
	setbits_le32(&ccm->ahb_gate0, 0x1 << 26);
	sdelay(0x20);
	clrbits_le32(&ccm->ahb_gate0, 0x1 << 26);

	/* setup MBUS clock */
	reg_val = (0x1 << 31) | (0x2 << 24) | (0x1);
	writel(reg_val, &ccm->mbus_clk_cfg);

	/*
	 * open DRAMC AHB & DLL register clock
	 * close it first
	 */
	clrbits_le32(&ccm->ahb_gate0, 0x1 << 14);

	sdelay(0x1000);

	/* then open it */
	setbits_le32(&ccm->ahb_gate0, 0x1 << 14);

	sdelay(0x1000);
}

static int dramc_scan_readpipe(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 reg_val;

	/* data training trigger */
	setbits_le32(&dram->ccr, 0x1 << 30);

	/* check whether data training process has completed */
	while (readl(&dram->ccr) & (0x1 << 30));

	/* check data training result */
	reg_val = readl(&dram->csr);
	if (reg_val & (0x1 << 20))
		return -1;

	return 0;
}

static void dramc_clock_output_en(u32 on)
{
	struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	if (on)
		setbits_le32(&ccm->dram_clk_cfg,
			     0x1 << SUN4I_CCM_SDRAM_DCLK_OUT_OFFSET);
	else
		clrbits_le32(&ccm->dram_clk_cfg,
			     0x1 << SUN4I_CCM_SDRAM_DCLK_OUT_OFFSET);
}

static void dramc_set_autorefresh_cycle(u32 clk)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 reg_val;
	u32 tmp_val;
	u32 dram_size;

	if (clk < 600) {
		dram_size = readl(&dram->dcr);
		dram_size >>= 3;
		dram_size &= 0x7;
		if (dram_size <= 0x2)
			reg_val = (131 * clk) >> 10;
		else
			reg_val = (336 * clk) >> 10;

		tmp_val = (7987 * clk) >> 10;
		tmp_val = tmp_val * 9 - 200;
		reg_val |= tmp_val << 8;
		reg_val |= 0x8 << 24;
		writel(reg_val, &dram->drr);
	} else {
		writel(0x0, &dram->drr);
	}
}

static struct dram_para dram_para = {
	.clock = 408,
	.type = 3,
	.rank_num = 1,
	.density = 2048,
	.io_width = 8,
	.bus_width = 32,
	.cas = 6,
	.zq = 123,
	.odt_en = 0,
	.size = 1024,
	.tpr0 = 0x30926692,
	.tpr1 = 0x1090,
	.tpr2 = 0x1a0c8,
	.tpr3 = 0,
	.tpr4 = 0,
	.tpr5 = 0,
	.emr1 = 0x4,
	.emr2 = 0,
	.emr3 = 0,
};

int dramc_init(struct dram_para *para)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 reg_val;
	int ret_val;

	/* check input dram parameter structure */
	if (!para)
		return -1;

	/* setup DRAM relative clock */
	mctl_setup_dram_clock(para->clock);

	/* reset external DRAM */
	mctl_ddr3_reset();
	mctl_set_drive();

	/* dram clock off */
	dramc_clock_output_en(0);

	/* select dram controller 1 */
	writel(0x16237495, &dram->csel);

	mctl_itm_disable();
	mctl_enable_dll0();

	/* configure external DRAM */
	reg_val = 0;
	if (para->type == 3)
		reg_val |= 0x1;
	reg_val |= (para->io_width >> 3) << 1;

	if (para->density == 256)
		reg_val |= 0x0 << 3;
	else if (para->density == 512)
		reg_val |= 0x1 << 3;
	else if (para->density == 1024)
		reg_val |= 0x2 << 3;
	else if (para->density == 2048)
		reg_val |= 0x3 << 3;
	else if (para->density == 4096)
		reg_val |= 0x4 << 3;
	else if (para->density == 8192)
		reg_val |= 0x5 << 3;
	else
		reg_val |= 0x0 << 3;

	reg_val |= ((para->bus_width >> 3) - 1) << 6;

	reg_val |= (para->rank_num - 1) << 10;

	reg_val |= 0x1 << 12;
	reg_val |= ((0x1) & 0x3) << 13;

	writel(reg_val, &dram->dcr);

	/* dram clock on */
	dramc_clock_output_en(1);

	sdelay(0x10);

	while (readl(&dram->ccr) & (0x1 << 31));

	mctl_enable_dllx();

	/* set odt impendance divide ratio */
	reg_val = ((para->zq) >> 8) & 0xfffff;
	reg_val |= ((para->zq) & 0xff) << 20;
	reg_val |= (para->zq) & 0xf0000000;
	writel(reg_val, &dram->zqcr0);

	/* set I/O configure register */
	reg_val = 0x00cc0000;
	reg_val |= (para->odt_en) & 0x3;
	reg_val |= ((para->odt_en) & 0x3) << 30;
	writel(reg_val, &dram->iocr);

	/* set refresh period */
	dramc_set_autorefresh_cycle(para->clock);

	/* set timing parameters */
	writel(para->tpr0, &dram->tpr0);
	writel(para->tpr1, &dram->tpr1);
	writel(para->tpr2, &dram->tpr2);

	/* set mode register */
	if (para->type == 3) {
		/* ddr3 */
		reg_val = 0x0;
		reg_val |= (para->cas - 4) << 4;
		reg_val |= 0x5 << 9;
	} else if (para->type == 2) {
		/* ddr2 */
		reg_val = 0x2;
		reg_val |= para->cas << 4;
		reg_val |= 0x5 << 9;
	}
	writel(reg_val, &dram->mr);

	writel(para->emr1, &dram->emr);
	writel(para->emr2, &dram->emr2);
	writel(para->emr3, &dram->emr3);

	/* set DQS window mode */
	clrsetbits_le32(&dram->ccr, 0x1 << 17, 0x1 << 14);

	/* reset external DRAM */
	setbits_le32(&dram->ccr, 0x1 << 31);

	while (readl(&dram->ccr) & (0x1 << 31));

	/* scan read pipe value */
	mctl_itm_enable();
	ret_val = dramc_scan_readpipe();

	if (ret_val < 0)
		return 0;

	/* configure all host port */
	mctl_configure_hostport();

	return get_ram_size((long *)PHYS_SDRAM_1, 1 << 30);
}

int sunxi_dram_init(void)
{
	return dramc_init(&dram_para);
}
