#ifndef _SUNXI_WATCHDOG_H_
#define _SUNXI_WATCHDOG_H_

/* Timeout limits */
#define WDT_MAX_TIMEOUT 16
#define WDT_OFF -1

void watchdog_reset(void);
void watchdog_set(int timeout);
void watchdog_init(void);

#endif

