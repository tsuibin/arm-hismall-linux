/*
 *  include/linux/ktimer.h
 *
 *  ktimers - high-precision kernel timers
 *
 *   Copyright(C) 2005, Thomas Gleixner <tglx@linutronix.de>
 *   Copyright(C) 2005, Red Hat, Inc., Ingo Molnar
 *
 *  data type definitions, declarations, prototypes
 *
 *  Started by: Thomas Gleixner and Ingo Molnar
 *
 *  For licencing details see kernel-base/COPYING
 */
#ifndef _LINUX_KTIMER_H
#define _LINUX_KTIMER_H

#include <linux/rbtree.h>
#include <linux/ktime.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/wait.h>

/*
 * Mode arguments of xxx_ktimer functions:
 */
enum ktimer_rearm {
	KTIMER_ABS = 1,	/* Time value is absolute */
	KTIMER_REL,	/* Time value is relative to now */
	KTIMER_INCR,	/* Time value is relative to previous expiry time */
	KTIMER_FORWARD,	/* Timer is rearmed with value. Overruns accounted */
	KTIMER_REARM,	/* Timer is rearmed with interval. Overruns accounted */
	KTIMER_RESTART,	/* Timer is restarted with the stored expiry value */

	/*
	 * Expiry must not be checked when the timer is started:
	 * (can be OR-ed with another above mode flag)
	 */
	KTIMER_NOCHECK = 0x10000,
	/*
	 * Rounding is required when the time is set up. Thats an
	 * optimization for relative timers as we read current time
	 * in the enqueing code so we do not need to read is twice.
	 */
	KTIMER_ROUND = 0x20000,

	/* (used internally: no rearming) */
	KTIMER_NOREARM = 0
};

/*
 * Timer states:
 */
enum ktimer_state {
	KTIMER_INACTIVE,	/* Timer is inactive */
	KTIMER_PENDING,		/* Timer is pending */
	KTIMER_EXPIRED,		/* Timer is expired and queued in the rbtree */
	KTIMER_EXPIRED_NOQUEUE, /* Timer is expired and not queued in the rbtree */
};

struct ktimer_base;

/**
 * struct ktimer - the basic ktimer structure
 *
 * @node:	red black tree node for time ordered insertion
 * @list:	list head for easier access to the time ordered list,
 *		without walking the red black tree.
 * @expires:	the absolute expiry time in the ktimers internal
 *		representation. The time is related to the clock on
 *		which the timer is based.
 * @expired:	the absolute time when the timer expired. Used for
 *		simplifying return path calculations and for debugging
 *		purposes.
 * @interval:	the timer interval for automatic rearming
 * @overrun:	the number of intervals missed when rearming a timer
 * @state:	state of the timer
 * @function:	timer expiry callback function
 * @data:	argument for the callback function
 * @base:	pointer to the timer base (per cpu and per clock)
 *
 * The ktimer structure must be initialized by init_ktimer_#CLOCKTYPE()
 */
struct ktimer {
	struct rb_node		node;
	struct list_head	list;
	ktime_t			expires;
	ktime_t			expired;
	int			expiry_mode;
	ktime_t			interval;
	int			overrun;
	enum ktimer_state	state;
	void			(*function)(void *);
	void			*data;
	struct ktimer_base	*base;
	int			prio;
};

/**
 * struct ktimer_base - the timer base for a specific clock
 *
 * @index:	clock type index for per_cpu support when moving a timer
 *		to a base on another cpu.
 * @lock:	lock protecting the base and associated timers
 * @active:	red black tree root node for the active timers
 * @pending:	list of pending timers for simple time ordered access
 * @count:	the number of active timers
 * @resolution:	the resolution of the clock, in nanoseconds
 * @get_time:	function to retrieve the current time of the clock
 * @curr_timer:	the timer which is executing a callback right now
 * @wait:	waitqueue to wait for a currently running timer
 * @name:	string identifier of the clock
 */
struct ktimer_base {
	int			index;
	raw_spinlock_t		lock;
	struct rb_root		active;
	struct list_head	pending;
	int			count;
	unsigned long		resolution;
	ktime_t			(*get_time)(void);
	struct ktimer		*curr_timer;
	wait_queue_head_t	wait;
#ifdef CONFIG_HIGH_RES_TIMERS
	struct list_head	expired;
	ktime_t			(*getoffset)(void);
	int			(*reprogram)(struct ktimer *t,
					     struct ktimer_base *b, ktime_t n);
#endif
	char			*name;
};

#define KTIMER_POISON		((void *) 0x00100101)

#ifdef CONFIG_HIGH_RES_TIMERS

extern void ktimer_clock_notify(void);
extern void clock_was_set(void);
extern int ktimer_interrupt(void);

/*
 * The resolution of the clocks. The resolution value is returned in
 * the clock_getres() system call to give application programmers an
 * idea of the (in)accuracy of timers. Timer values are rounded up to
 * this resolution values.
 */
#define KTIME_REALTIME_RES		CONFIG_HIGH_RES_RESOLUTION
#define KTIME_MONOTONIC_RES		CONFIG_HIGH_RES_RESOLUTION

#define ktimer_trace(a,b)		trace_special(ktime_get_high(a),ktime_get_low(a),b)

#else

#define KTIME_REALTIME_RES		KTIME_LOW_RES
#define KTIME_MONOTONIC_RES		KTIME_LOW_RES

/*
 * clock_was_set() is a NOP for non- high-resolution systems. The
 * time-sorted order guarantees that a timer does not expire early and
 * is expired in the next softirq when the clock was advanced.
 */
#define clock_was_set()			do { } while (0)
#define ktimer_clock_notify()		do { } while (0)

static inline int ktimer_interrupt(void)
{
	return 0;
}

#define ktimer_trace(a,b)		trace_special_u64(a,b)

#endif

/* Exported timer functions: */

/* Initialize timers: */
extern void ktimer_init(struct ktimer *timer);
extern void ktimer_init_real(struct ktimer *timer);

/* Basic timer operations: */
extern int ktimer_start(struct ktimer *timer, ktime_t *tim, int mode);
extern int ktimer_restart(struct ktimer *timer, ktime_t *tim, int mode);
extern int ktimer_cancel(struct ktimer *timer);
extern int ktimer_try_to_cancel(struct ktimer *timer);

/* Query timers: */
extern ktime_t ktimer_get_remtime(struct ktimer *timer);
extern ktime_t ktimer_get_expiry(struct ktimer *timer, ktime_t *now);
extern int ktimer_get_res(clockid_t which_clock, struct timespec *tp);
extern int ktimer_get_res_real(clockid_t which_clock, struct timespec *tp);

static inline int ktimer_active(struct ktimer *timer)
{
	return timer->state != KTIMER_INACTIVE;
}

/* Convert with rounding based on resolution of timer's clock: */
extern ktime_t ktimer_round_timeval(struct ktimer *timer, struct timeval *tv);
extern ktime_t ktimer_round_timespec(struct ktimer *timer, struct timespec *ts);

/* Precise sleep: */
extern long ktimer_nanosleep(struct timespec *rqtp,
			     struct timespec __user *rmtp, int mode);
extern long ktimer_nanosleep_real(struct timespec *rqtp,
				  struct timespec __user *rmtp, int mode);

#if defined(CONFIG_SMP) || defined(CONFIG_PREEMPT_SOFTIRQS)
extern void wait_for_ktimer(struct ktimer *timer);
#else
# define wait_for_ktimer(t)	do { } while (0)
#endif

/* Soft interrupt function to run the ktimer queues: */
extern void ktimer_run_queues(void);

/* Bootup initialization: */
extern void __init ktimers_init(void);

#endif
