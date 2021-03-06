#include "timer.h"
#include "types.h"
#include "cpu.h"
#include "watchdog.h"
#include "io.h"

#define WDT_CTRL_RESTART	(0x1 << 0)
#define WDT_CTRL_KEY		(0x0a57 << 1)

#define WDT_MODE_EN		(0x1 << 0)
#define WDT_MODE_RESET_EN	(0x1 << 1)
#define WDT_MAX_TIMEOUT		16
#define WDT_MODE_TIMEOUT(n) \
	 (wdt_timeout_map[(n) < WDT_MAX_TIMEOUT ? (n) : WDT_MAX_TIMEOUT] << 3)


/*
 * Watchdog timeout table. The sunxi cores only use 4 bits for the watchdog as
 * set by the table below. The gaps are filled by rounding up to the next
 * second up.
 */
const unsigned int wdt_timeout_map[] = {
	[0] = 0b0000,  /* 0.5s*/
	[1] = 0b0001,  /* 1s  */
	[2] = 0b0010,  /* 2s  */
	[3] = 0b0011,  /* 3s  */
	[4] = 0b0100,  /* 4s  */
	[5] = 0b0101,  /* 5s  */
	[6] = 0b0110,  /* 6s  */
	[7] = 0b0111,  /* 8s  */
	[8] = 0b0111,  /* 8s  */
	[9] = 0b1000, /* 10s */
	[10] = 0b1000, /* 10s */
	[11] = 0b1001, /* 12s */
	[12] = 0b1001, /* 12s */
	[13] = 0b1010, /* 14s */
	[14] = 0b1010, /* 14s */
	[15] = 0b1011, /* 16s */
	[16] = 0b1011, /* 16s */
};


void watchdog_reset(void)
{
	static const struct sunxi_wdog *wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;

	sr32(&wdog->ctl, 0, 1, WDT_CTRL_RESTART);
}

void watchdog_set(int timeout)
{
	static struct sunxi_wdog *const wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;
	u32 reg_val;

	/* Set timeout, reset & enable */
	if (timeout >= 0) {
		reg_val |= WDT_MODE_TIMEOUT(timeout) | WDT_MODE_RESET_EN |
			   WDT_MODE_EN;
		writel(reg_val, &wdog->mode);
	} else {
		writel(0, &wdog->mode);
	}
	watchdog_reset();
}

void watchdog_init(void)
{
	watchdog_set(WDT_OFF); /* no timeout */
}
