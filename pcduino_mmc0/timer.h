#ifndef _SUNXI_TIMER_H_
#define _SUNXI_TIMER_H_

#include "types.h"

/* General purpose timer (16 bytes)*/
struct sunxi_timer {
	u32 ctl;
	u32 inter;
	u32 val;
	u8 res[4];/* baoliu? weile tianchong wei? */
};

/* Audio video sync (16 bytes) */
struct sunxi_avs {
	u32 ctl;		/* 0x80 */
	u32 cnt0;		/* 0x84 */
	u32 cnt1;		/* 0x88 */
	u32 div;		/* 0x8c */
};

/* 64 bit counter (12 bytes) */
struct sunxi_64cnt {
	u32 ctl;		/* 0xa0 */
	u32 lo;			/* 0xa4 */
	u32 hi;			/* 0xa8 */
};

/* Watchdog (8 bytes) */
struct sunxi_wdog {
	u32 ctl;		/* 0x90 */
	u32 mode;		/* 0x94 */
};

/* Rtc  (12 bytes) */
struct sunxi_rtc {
	u32 ctl;		/* 0x100 */
	u32 yymmdd;		/* 0x104 */
	u32 hhmmss;		/* 0x108 */
};

/* Alarm (20 bytes) */
struct sunxi_alarm {
	u32 ddhhmmss;		/* 0x10c */
	u32 hhmmss;		/* 0x110 */
	u32 en;			/* 0x114 */
	u32 irqen;		/* 0x118 */
	u32 irqsta;		/* 0x11c */
};

/* Timer general purpose register (4 bytes) */
struct sunxi_tgp {
	u32 tgpd;
};

struct sunxi_timer_reg {
	u32 tirqen;		/* 0x00 */
	u32 tirqsta;		/* 0x04 */
	u8 res1[8];
	struct sunxi_timer timer[6];	/* We have 6 timers */
	u8 res2[16];
	struct sunxi_avs avs;
	struct sunxi_wdog wdog;
	u8 res3[8];
	struct sunxi_64cnt cnt64;
	u8 res4[0x58];
	struct sunxi_rtc rtc;
	struct sunxi_alarm alarm;
	struct sunxi_tgp tgp[4];
	u8 res5[8];
	u32 cpu_cfg;
};

int timer_init_all(void);
u32 read_timer(int idx);
void timer0_cndelay(u32 cnt);
void timer1_cndelay(u32 cnt);
void timer2_cndelay(u32 cnt);
void timer4_cndelay(u32 cnt);
void timer5_cndelay(u32 cnt);
void timer0_udelay(u32 usec);
void timer1_udelay(u32 usec);
void timer2_udelay(u32 usec);
void timer4_mdelay(u32 msec);
void timer5_mdelay(u32 msec);
void mdelay(u32 usec);
u32 get_mtimer(void);

void avscnt0_cndelay(u32 cnt);
void avscnt1_cndelay(u32 cnt);
u32 read_avs(int idx);

ulong read_cnt64l(void);
ulong read_cnt64h(void);



#endif
