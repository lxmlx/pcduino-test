#include "clock.h"
#include "common.h"
#include "types.h"
#include "io.h"

static void clock_init_safe(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* Set safe defaults until PMU is configured */
	writel(AXI_DIV_1 << 0 | AHB_DIV_2 << 4 | APB0_DIV_1 << 8 |
	       CPU_CLK_SRC_OSC24M << 16, &ccm->cpu_ahb_apb0_cfg);
	writel(0xa1005000, &ccm->pll1_cfg);
	sdelay(200);
	writel(AXI_DIV_1 << 0 | AHB_DIV_2 << 4 | APB0_DIV_1 << 8 |
	       CPU_CLK_SRC_PLL1 << 16, &ccm->cpu_ahb_apb0_cfg);
}

int clock_init(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	clock_init_safe();

	/* uart clock source is apb1 */
	sr32(&ccm->apb1_clk_div_cfg, 24, 2, APB1_CLK_SRC_OSC24M);
	sr32(&ccm->apb1_clk_div_cfg, 16, 2, APB1_FACTOR_N);
	sr32(&ccm->apb1_clk_div_cfg, 0, 5, APB1_FACTOR_M);

	/* open the clock for uart */
	sr32(&ccm->apb1_gate, 16 + CONFIG_CONS_INDEX - 1, 1, CLK_GATE_OPEN);

	return 0;
}

/* Return PLL5 frequency in Hz
 * Note: Assumes PLL5 reference is 24MHz clock
 */
unsigned int clock_get_pll5(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll5_cfg);
	int n = (rval >> 8) & 0x1f;
	int k = ((rval >> 4) & 3) + 1;
	int p = 1 << ((rval >> 16) & 3);
	return 24000000 * n * k / p;
}


int clock_twi_onoff(int port, int state)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	if (port > 2)
		return -1;

	/* set the apb1 clock gate for twi */
	sr32(&ccm->apb1_gate, 0 + port, 1, state);

	return 0;
}

#define PLL1_CFG(N, K, M, P)	(1 << 31 | 0 << 30 | 8 << 26 | 0 << 25 | \
				 16 << 20 | (P) << 16 | 2 << 13 | (N) << 8 | \
				 (K) << 4 | 0 << 3 | 0 << 2 | (M) << 0)
#define RDIV(a, b)		((a + (b) - 1) / (b))

struct {
	u32 pll1_cfg;
	unsigned int freq;
} pll1_para[] = {
	{ PLL1_CFG(16, 0, 0, 0), 384000000 },
	{ PLL1_CFG(16, 1, 0, 0), 768000000 },
	{ PLL1_CFG(20, 1, 0, 0), 960000000 },
	{ PLL1_CFG(21, 1, 0, 0), 1008000000},
	{ PLL1_CFG(22, 1, 0, 0), 1056000000},
	{ PLL1_CFG(23, 1, 0, 0), 1104000000},
	{ PLL1_CFG(24, 1, 0, 0), 1152000000},
	{ PLL1_CFG(25, 1, 0, 0), 1200000000},
	{ PLL1_CFG(26, 1, 0, 0), 1248000000},
	{ PLL1_CFG(27, 1, 0, 0), 1296000000},
	{ PLL1_CFG(28, 1, 0, 0), 1344000000},
	{ PLL1_CFG(29, 1, 0, 0), 1392000000},
	{ PLL1_CFG(30, 1, 0, 0), 1440000000},
	{ PLL1_CFG(31, 1, 0, 0), 1488000000},
	{ PLL1_CFG(31, 1, 0, 0), ~0},
};
