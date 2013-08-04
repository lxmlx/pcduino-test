#include "common.h"
#include "io.h"
#include "timer.h"
#include "cpu.h"
#include "types.h"
#include "common.h"
#include "string.h"
#include "syslib.h"

#define TIMER_MODE_CONTINOUS	(0x0 << 7)	/* continuous mode */
#define TIMER_MODE_SINGLE	(0x1 << 7)

#define TIMER_DIV1    		(0x0 << 4)	/* pre scale 1 */
#define TIMER_DIV2		(0x1 << 4)
#define TIMER_DIV4		(0x2 << 4)
#define TIMER_DIV8		(0x3 << 4)
#define TIMER_DIV16		(0x4 << 4)
#define TIMER_DIV32		(0x5 << 4)
#define TIMER_DIV64		(0x6 << 4)
#define TIMER_DIV128		(0x7 << 4)

#define TIMER_SRC_LOSC		(0x0 << 2)
#define TIMER_SRC_OSC24M	(0x1 << 2)	/* osc24m */
#define TIMER_SRC_PLL6DIV6	(0x2 << 2)
#define TIMER_SRC_EXTCLKIN0	(0x2 << 2)
#define TIMER_SRC_EXTCLKIN1	(0x2 << 2)

#define TIMER_RELOAD (0x1 << 1)			/* reload internal value */

#define TIMER_SP		(0x0 << 0)
#define TIMER_EN		(0x1 << 0)	/* enable timer */

/* timer0 field */
#define TIMER0_LOAD_VAL		0xffffffff
#define TIMER0_CLOCK		24000000	/* (24 * 1000 * 1000) */

/* timer1 are the same as timer0 */
#define TIMER1_LOAD_VAL		0xffffffff
#define TIMER1_CLOCK		6000000		/* (24 * 1000 * 1000 / 4) */

/* timer2 field */
#define TIMER2_LOAD_VAL		0xffffffff
#define TIMER2_CLOCK		3000000		/* (24 * 1000 * 1000 / 8) */

/* timer4 field */
#define TIMER4_LOAD_VAL		0xffffffff
#define TIMER4_CLOCK		1500000		/* (24 * 1000 * 1000 / 16) */

/* timer5 field */
#define TIMER5_LOAD_VAL		0xffffffff
#define TIMER5_CLOCK		375000		/* (24 * 1000 * 1000 / 64) */

#define READ_TIMER(n)	(~readl(&timer_base->timer[n].val))

/* avs field */
#define AVS_CNT1_PS		(0x1 << 9)
#define AVS_CNT0_PS		(0x1 << 8)
#define AVS_CNT1_EN		(0x1 << 1)
#define AVS_CNT0_EN		(0x1 << 0)

#define AVS_CNT1_D(DN1)		((DN1-1) << 16)	/* 1 < DN1 < 0x7ff */
#define AVS_CNT0_D(DN0)		((DN0-1) << 0)	/* 1 < DN0 < 0x7ff */

#define AVS_CNT1_MAX		0xffffffff
#define AVS_CNT0_MAX		0xffffffff

#define read_avscnt0		(readl(&timer_base->avs.cnt0))
#define read_avscnt1		(readl(&timer_base->avs.cnt1))

/* 64-bit counter field */
#define CNT64_CLK_SRC_OSC24M	(0 << 2)
#define CNT64_CLK_SRC_PLL6DIV6	(1 << 2)
#define CNT64_RL_EN		(1 << 1)
#define CNT64_CLR_EN		(1 << 0)

#define read_cnt64lo		(readl(&timer_base->cnt64.lo))
#define read_cnt64hi		(readl(&timer_base->cnt64.hi))

static struct sunxi_timer_reg *timer_base =
	(struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

/* init timer register */
int timer_init_timer0(void)
{
	writel(TIMER0_LOAD_VAL, &timer_base->timer[0].inter);
	writel(TIMER_MODE_CONTINOUS | TIMER_DIV1 | TIMER_SRC_OSC24M | 
		TIMER_RELOAD | TIMER_EN, &timer_base->timer[0].ctl);
	return 0;
}

int timer_init_timer1(void)
{
	writel(TIMER1_LOAD_VAL, &timer_base->timer[1].inter);
	writel(TIMER_MODE_CONTINOUS | TIMER_DIV4 | TIMER_SRC_OSC24M | 
		TIMER_RELOAD | TIMER_EN, &timer_base->timer[1].ctl);
	return 0;
}

int timer_init_timer2(void)
{
	writel(TIMER2_LOAD_VAL, &timer_base->timer[2].inter);
	writel(TIMER_MODE_CONTINOUS | TIMER_DIV8 | TIMER_SRC_OSC24M |
		TIMER_RELOAD | TIMER_EN, &timer_base->timer[2].ctl);
	return 0;
}

int timer_init_timer3(void)
{	/* no cur val */
	return 0;
}

int timer_init_timer4(void)
{
	writel(TIMER4_LOAD_VAL, &timer_base->timer[4].inter);
	writel(TIMER_MODE_CONTINOUS | TIMER_DIV16 | TIMER_SRC_OSC24M |
		TIMER_RELOAD | TIMER_EN, &timer_base->timer[4].ctl);
	return 0;
}

int timer_init_timer5(void)
{
	writel(TIMER5_LOAD_VAL, &timer_base->timer[5].inter);
	writel(TIMER_MODE_CONTINOUS | TIMER_DIV64 | TIMER_SRC_OSC24M |
		TIMER_RELOAD | TIMER_EN, &timer_base->timer[5].ctl);
	return 0;
}

u32 read_timer(int idx)
{
	return READ_TIMER(idx);
}

void timer0_cndelay(u32 cnt)
{
	u32 tmo = cnt, tmolast = cnt;
	u32 now, last = READ_TIMER(0);

	while (tmo > 0) {
		now = READ_TIMER(0);
		if (now > last)
			tmo -= now - last;
		else
			tmo -= TIMER0_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer0_udelay(u32 usec)
{
	u32 tmo = (TIMER0_CLOCK / 1000000 ) * usec;
	u32 tmolast = tmo;
	u32 now, last = READ_TIMER(0);

	while (tmo > 0) {
		now = READ_TIMER(0);
		if (now > last)	/* normal (non rollover) */
			tmo -= now - last;
		else		/* rollover */
			tmo -= TIMER0_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer1_cndelay(u32 cnt)
{
	u32 tmo = cnt, tmolast = cnt;
	u32 now, last = READ_TIMER(1);

	while (tmo > 0) {
		now = READ_TIMER(1);
		if (now > last)
			tmo -= now - last;
		else
			tmo -= TIMER1_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer1_udelay(u32 usec)
{
	u32 tmo = (TIMER1_CLOCK / 1000000 ) * usec;
	u32 tmolast = tmo;
	u32 now, last = READ_TIMER(1);

	while (tmo > 0) {
		now = READ_TIMER(1);
		if (now > last)	/* normal (non rollover) */
			tmo -= now - last;
		else		/* rollover */
			tmo -= TIMER1_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer2_cndelay(u32 cnt)
{
	u32 tmo = cnt, tmolast = cnt;
	u32 now, last = READ_TIMER(2);

	while (tmo > 0) {
		now = READ_TIMER(2);
		if (now > last)
			tmo -= now - last;
		else
			tmo -= TIMER2_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer2_udelay(u32 usec)
{
	u32 tmo = (TIMER2_CLOCK / 1000000 ) * usec;
	u32 tmolast = tmo;
	u32 now, last = READ_TIMER(2);

	while (tmo > 0) {
		now = READ_TIMER(2);
		if (now > last)	/* normal (non rollover) */
			tmo -= now - last;
		else
			tmo -= TIMER2_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer4_cndelay(u32 cnt)
{
	u32 tmo = cnt, tmolast = cnt;
	u32 now, last = READ_TIMER(4);

	while (tmo > 0) {
		now = READ_TIMER(4);
		if (now > last)
			tmo -= now - last;
		else
			tmo -= TIMER4_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer4_mdelay(u32 msec)
{
	u32 tmo = (TIMER4_CLOCK / 1000) * msec;
	u32 tmolast = tmo;
	u32 now, last = READ_TIMER(4);

	while (tmo > 0) {
		now = READ_TIMER(4);
		if (now > last)	/* normal (non rollover) */
			tmo -= now - last;
		else		/* rollover */
			tmo -= TIMER4_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer5_cndelay(u32 cnt)
{
	u32 tmo = cnt, tmolast = cnt;
	u32 now, last = READ_TIMER(5);

	while (tmo > 0) {
		now = READ_TIMER(5);
		if (now > last)
			tmo -= now - last;
		else
			tmo -= TIMER5_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void timer5_mdelay(u32 msec)
{
	u32 tmo = (TIMER5_CLOCK / 1000) * msec;
	u32 tmolast = tmo;
	u32 now, last = READ_TIMER(5);

	while (tmo > 0) {
		now = READ_TIMER(5);
		if (now > last)	/* normal (non rollover) */
			tmo -= now - last;
		else
			tmo -= TIMER5_LOAD_VAL - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

u32 get_mtimer(u32 base)
{
	u32 divisor = TIMER5_CLOCK / 1000; /* per msec 375 */
	u32 before = base * divisor;
	u32 now = READ_TIMER(5);
	if (now > before)
		return (now - before) / divisor;
	else
		return (TIMER5_LOAD_VAL - before + now) / divisor;
}

void mdelay(u32 msec)
{
	while(msec--)
		timer0_udelay(1000);
}

int timer_init_avs(void)
{
	writel(AVS_CNT1_D(24) | AVS_CNT0_D(24), &timer_base->avs.div);
	writel(AVS_CNT1_EN | AVS_CNT0_EN, &timer_base->avs.ctl);
	return 0;
}

u32 read_avs(int idx)
{
	switch(idx) {
	case 0:
		return read_avscnt0;
	case 1:
		return read_avscnt1;
	default:
		return 0xffffffff;
	}
}

void avscnt0_cndelay(u32 cnt)
{
	u32 tmo = cnt, tmolast = cnt;
	u32 now, last = read_avscnt0;

	while (tmo > 0) {
		now = read_avscnt0;
		if (now > last)
			tmo -= now - last;
		else if (now < last)
			tmo -= AVS_CNT0_MAX - last + now;
		last = now;
		
		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

void avscnt1_cndelay(u32 cnt)
{
	u32 tmo = cnt, tmolast = cnt;
	u32 now, last = read_avscnt1;

	while (tmo > 0) {
		now = read_avscnt1;
		if (now > last)
			tmo -= now - last;
		else if (now < last)
			tmo -= AVS_CNT1_MAX - last + now;
		last = now;

		if (tmo > tmolast)
			break;
		tmolast = tmo;
	}
	return;
}

int timer_init_wdog(void)
{
	return 0;
}

int timer_init_cnt64(void)
{
	sr32(&timer_base->cnt64.ctl, 2, 1, 0);
	sr32(&timer_base->cnt64.ctl, 0, 1, 1);
	return 0;
}

ulong read_cnt64l(void)
{
	return read_cnt64lo;
}

ulong read_cnt64h(void)
{
	return read_cnt64hi;
}

int timer_init_rtc(void)
{
	return 0;
}

int timer_init_alarm(void)
{
	return 0;
}

int timer_init_all(void)
{
	timer_init_timer0();
	timer_init_timer1();
	timer_init_timer2();
	timer_init_timer3();
	timer_init_timer4();
	timer_init_timer5();
	timer_init_avs();
	timer_init_cnt64();
	return 0;
}
