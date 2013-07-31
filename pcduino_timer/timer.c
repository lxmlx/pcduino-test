#include "common.h"
#include "io.h"
#include "timer.h"
#include "cpu.h"
#include "types.h"
#include "global_data.h"
#include "common.h"

DECLARE_GLOBAL_DATA_PTR;

/* timer0 field */
#define TIMER0_MODE_CONTINOUS   (0x0 << 7)	/* continuous mode */
#define TIMER0_MODE_SINGLE	(0x1 << 7)
#define TIMER0_DIV1    		(0x0 << 4)	/* pre scale 1 */
#define TIMER0_DIV2		(0x1 << 4)
#define TIMER0_DIV4		(0x2 << 4)
#define TIMER0_DIV8		(0x3 << 4)
#define TIMER0_DIV16		(0x4 << 4)
#define TIMER0_DIV32		(0x5 << 4)
#define TIMER0_DIV64		(0x6 << 4)
#define TIMER0_DIV128		(0x7 << 4)

#define TIMER0_SRC_LOSC	(0x0 << 2)
#define TIMER0_SRC_OSC24M	(0x1 << 2)	/* osc24m */
#define TIMER0_SRC_PLL6DIV6	(0x2 << 2)

#define TIMER0_RELOAD (0x1 << 1)	/* reload internal value */

#define TIMER0_SP		(0x0 << 0)
#define TIMER0_EN		(0x1 << 0)	/* enable timer */

#define TIMER0_LOAD_VAL		0xffffffff
#define TIMER0_CLOCK		(24 * 1000 * 1000)

/* timer1 are the same as timer0 */
#define TIMER1_MODE_CONTINOUS   (0x0 << 7)	/* continuous mode */
#define TIMER1_MODE_SINGLE	(0x1 << 7)
#define TIMER1_DIV1    		(0x0 << 4)	/* pre scale 1 */
#define TIMER1_DIV2		(0x1 << 4)
#define TIMER1_DIV4		(0x2 << 4)
#define TIMER1_DIV8		(0x3 << 4)
#define TIMER1_DIV16		(0x4 << 4)
#define TIMER1_DIV32		(0x5 << 4)
#define TIMER1_DIV64		(0x6 << 4)
#define TIMER1_DIV128		(0x7 << 4)

#define TIMER1_SRC_LOSC	(0x0 << 2)
#define TIMER1_SRC_OSC24M	(0x1 << 2)	/* osc24m */

#define TIMER1_RELOAD (0x1 << 1)	/* reload internal value */

#define TIMER1_SP		(0x0 << 0)
#define TIMER1_EN		(0x1 << 0)	/* enable timer */

#define TIMER1_LOAD_VAL		0xffffffff
#define TIMER1_CLOCK		(24 * 1000 * 1000 / 4)

/* timer2 field */
#define TIMER2_MODE_CONTINOUS   (0x0 << 7)	/* continuous mode */
#define TIMER2_MODE_SINGLE	(0x1 << 7)
#define TIMER2_DIV1    		(0x0 << 4)	/* pre scale 1 */
#define TIMER2_DIV2		(0x1 << 4)
#define TIMER2_DIV4		(0x2 << 4)
#define TIMER2_DIV8		(0x3 << 4)
#define TIMER2_DIV16		(0x4 << 4)
#define TIMER2_DIV32		(0x5 << 4)
#define TIMER2_DIV64		(0x6 << 4)
#define TIMER2_DIV128		(0x7 << 4)

#define TIMER2_SRC_LOSC	(0x0 << 2)
#define TIMER2_SRC_OSC24M	(0x1 << 2)	/* osc24m */
#define TIMER2_SRC_PLL6DIV6	(0x2 << 2)

#define TIMER2_RELOAD (0x1 << 1)	/* reload internal value */

#define TIMER2_SP		(0x0 << 0)
#define TIMER2_EN		(0x1 << 0)	/* enable timer */

#define TIMER2_LOAD_VAL		0xffffffff


/* timer3 field */
#define TIMER3_MODE_CONTINOUS   (0x0 << 4)	/* continuous mode */
#define TIMER3_MODE_SINGLE	(0x1 << 4)
/* timer3 clock source is the losc */
#define TIMER3_DIV16    	(0x0 << 2)	/* pre scale 16 */
#define TIMER3_DIV32		(0x1 << 2)	
#define TIMER3_DIV64		(0x2 << 2)

#define TIMER3_SP		(0x0 << 0)
#define TIMER3_EN		(0x1 << 0)	/* enable timer */

#define TIMER3_LOAD_VAL		0xffffffff


/* timer4 field */
#define TIMER4_MODE_CONTINOUS   (0x0 << 7)	/* continuous mode */
#define TIMER4_MODE_SINGLE	(0x1 << 7)
#define TIMER4_DIV1    		(0x0 << 4)	/* pre scale 1 */
#define TIMER4_DIV2		(0x1 << 4)
#define TIMER4_DIV4		(0x2 << 4)
#define TIMER4_DIV8		(0x3 << 4)
#define TIMER4_DIV16		(0x4 << 4)
#define TIMER4_DIV32		(0x5 << 4)
#define TIMER4_DIV64		(0x6 << 4)
#define TIMER4_DIV128		(0x7 << 4)

#define TIMER4_SRC_LOSC			(0x0 << 2)
#define TIMER4_SRC_OSC24M		(0x1 << 2)	/* osc24m */
#define TIMER4_SRC_EXTERNALCLKIN0	(0x2 << 2)

#define TIMER4_RELOAD (0x1 << 1)	/* reload internal value */

#define TIMER4_SP		(0x0 << 0)
#define TIMER4_EN		(0x1 << 0)	/* enable timer */

#define TIMER4_LOAD_VAL		0xffffffff


/* timer5 field */
#define TIMER5_MODE_CONTINOUS   (0x0 << 7)	/* continuous mode */
#define TIMER5_MODE_SINGLE	(0x1 << 7)
#define TIMER5_DIV1    		(0x0 << 4)	/* pre scale 1 */
#define TIMER5_DIV2		(0x1 << 4)
#define TIMER5_DIV4		(0x2 << 4)
#define TIMER5_DIV8		(0x3 << 4)
#define TIMER5_DIV16		(0x4 << 4)
#define TIMER5_DIV32		(0x5 << 4)
#define TIMER5_DIV64		(0x6 << 4)
#define TIMER5_DIV128		(0x7 << 4)

#define TIMER5_SRC_LSOSC		(0x0 << 2)
#define TIMER5_SRC_OSC24M		(0x1 << 2)	/* osc24m */
#define TIMER5_SRC_EXTERNALCLKIN1	(0x2 << 2)

#define TIMER5_RELOAD (0x1 << 1)	/* reload internal value */

#define TIMER5_SP		(0x0 << 0)
#define TIMER5_EN		(0x1 << 0)	/* enable timer */

#define TIMER5_LOAD_VAL		0xffffffff

#define READ_TIMER(n)	(~readl(&timer_base->timer[n].val))

static struct sunxi_timer_reg *timer_base =
	(struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

/* init timer register */
int timer_init_timer0(void)
{
	writel(TIMER0_LOAD_VAL, &timer_base->timer[0].inter);
	writel(TIMER0_MODE_CONTINOUS | TIMER0_DIV1 | TIMER0_SRC_OSC24M | 
		TIMER0_RELOAD | TIMER0_EN, &timer_base->timer[0].ctl);
}

int timer_init_timer1(void)
{
	writel(TIMER1_LOAD_VAL, &timer_base->timer[1].inter);
	writel(TIMER1_MODE_CONTINOUS | TIMER1_DIV4 | TIMER1_SRC_OSC24M | 
		TIMER1_RELOAD | TIMER1_EN, &timer_base->timer[1].ctl);
}

int timer_init_timer2(void)
{
}

int timer_init_timer3(void)
{
}

int timer_init_timer4(void)
{
}

int timer_init_timer5(void)
{
}

u32 read_timer(int idx)
{
	return READ_TIMER(idx);
}

void timer0_udelay(unsigned long usec)
{
	long tmo = usec * (TIMER0_CLOCK / 1000) / 1000;
	ulong now, last = READ_TIMER(0);

	while (tmo > 0) {
		now = READ_TIMER(0);
		if (now > last)	/* normal (non rollover) */
			tmo -= now - last;
		else		/* rollover */
			tmo -= TIMER0_LOAD_VAL - last + now;
		last = now;
	}
}

void timer1_udelay(unsigned long usec)
{
	long tmo = usec * (TIMER1_CLOCK / 1000) / 1000;
	ulong now, last = READ_TIMER(1);

	while (tmo > 0) {
		now = READ_TIMER(1);
		if (now > last)	/* normal (non rollover) */
			tmo -= now - last;
		else		/* rollover */
			tmo -= TIMER1_LOAD_VAL - last + now;
		last = now;
	}
}

void __udelay(unsigned long usec)
{
	timer0_udelay(usec);
}


int timer_init_avs(void)
{
}

int timer_init_wdog(void)
{
}

int timer_init_cnt64(void)
{
}

int timer_init_rtc(void)
{
}

int timer_init_alarm(void)
{
}

int timer_init_all(void)
{
	timer_init_timer0();
	timer_init_timer1();
}
