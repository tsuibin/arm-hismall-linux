/*  linux/include/linux/clockchips.h
 *
 *  This file contains the structure definitions for clockchips.
 *
 *  If you are not a clockchip, or the time of day code, you should
 *  not be including this file!
 */
#ifndef _LINUX_CLOCKCHIPS_H
#define _LINUX_CLOCKCHIPS_H

#include <linux/config.h>

#ifdef CONFIG_GENERIC_TIME

#include <linux/clocksource.h>
#include <linux/interrupt.h>

/* Clock event modes and commands */
enum {
	CLOCK_EVT_NONE,
	CLOCK_EVT_STARTUP,
	CLOCK_EVT_PERIODIC,
	CLOCK_EVT_ONESHOT,
	CLOCK_EVT_IPI,
	CLOCK_EVT_STOP,
	CLOCK_EVT_SHUTDOWN,
	CLOCK_EVT_RUN_CYCLIC,
	CLOCK_EVT_SCHEDTICK,
	CLOCK_EVT_NOTICK,
};

/* Clock event capability flags */
#define CLOCK_CAP_TICK		0x000001

#if defined(CONFIG_HIGH_RES_TIMERS) || defined(CONFIG_DYNTICK)
#define CLOCK_CAP_NEXTEVT	0x000002
#else
#define CLOCK_CAP_NEXTEVT	0x000000
#endif

#define CLOCK_CAP_UPDATE	0x000004

#ifndef CONFIG_PROFILE_NMI
#define CLOCK_CAP_PROFILE	0x000008
#else
#define CLOCK_CAP_PROFILE	0x000000
#endif

#define CLOCK_CAP_MASK		(CLOCK_CAP_TICK | CLOCK_CAP_NEXTEVT | CLOCK_CAP_PROFILE | CLOCK_CAP_UPDATE)

/* The device has its own interrupt handler */
#define CLOCK_HAS_IRQHANDLER	0x010000

struct clock_event;

/**
 * struct clock_event - clock event descriptor
 *
 * @name:		ptr to clock event name
 * @capabilities:	capabilities of the event chip
 * @max_delta_ns:	maximum delta value in ns
 * @min_delta_ns:	minimum delta value in ns
 * @mult:		nanosecond to cycles multiplier
 * @shift:		nanoseconds to cycles divisor (power of two)
 * @set_next_event:	set next event
 * @set_mode:		set mode function
 * @suspend:		suspend function (optional)
 * @resume:		resume function (optional)
 * @evthandler:		Assigned by the framework to be called by the low
 *			level handler of the event source
 * @start_event:	called on entry (optional for chip handling...)
 * @end_event:		called on exit (optional for chip handling...)
 * @priv:		private device data
 */
struct clock_event {
	const char* name;
	unsigned int capabilities;
	unsigned long max_delta_ns;
	unsigned long min_delta_ns;
	u32 mult;
	u32 shift;
	void (*set_next_event)(unsigned long evt);
	void (*set_mode)(int mode);
	int (*suspend)(void);
	int (*resume)(void);
	void (*event_handler)(struct pt_regs *regs);
	void (*start_event)(void *priv);
	void (*end_event)(void *priv);
	unsigned int irq;
	void *priv;
};



/*
 * Calculate a multiplication factor with shift=32
 */
static inline unsigned long div_sc32(unsigned long a, unsigned long b)
{
	u64 tmp = ((u64)a) << 32;
	do_div(tmp, b);
	return (unsigned long) tmp;
}

static inline unsigned long mpy_sc32(unsigned long a, unsigned long b)
{
	u64 res = (u64) a * b;

	return (unsigned long) (res >> 32);
}

/* Clock event layer functions */
extern int setup_local_clockevent(struct clock_event *, cpumask_t cpumask);
extern int setup_global_clockevent(struct clock_event *, cpumask_t cpumask);
extern unsigned long clockevent_delta2ns(unsigned long latch, struct clock_event *evt);
extern void init_clockevents(void);

extern int clockevents_init_next_event(void);
extern int clockevents_set_next_event(ktime_t expires, ktime_t now);
extern void clockevents_trigger_next_event(void);
extern int clockevents_next_event_available(void);

#else
# define init_clockevents() do { } while(0)
#endif

#endif
