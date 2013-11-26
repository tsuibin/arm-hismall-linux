/*  linux/include/linux/timeofday.h
 *
 *  This file contains the interface to the time of day subsystem
 */
#ifndef _LINUX_TIMEOFDAY_H
#define _LINUX_TIMEOFDAY_H
#include <linux/calc64.h>
#include <linux/types.h>
#include <linux/time.h>
#include <linux/timex.h>

#ifdef CONFIG_GENERIC_TIME
/* Public definitions */

/* monotonic -> wall time offset accessors */
extern nsec_t get_realtime_offset_ns(void);
extern void get_realtime_offset_ts(struct timespec *ts);

/* realtime clock accessors */
extern void get_realtime_clock_ts(struct timespec *ts);
extern nsec_t get_realtime_clock_ns(void);

/* monotonic clock accessors */
extern void get_monotonic_clock_ts(struct timespec *ts);
extern nsec_t get_monotonic_clock_ns(void);

/* legacy timeofday interfaces*/
#define getnstimeofday(ts) get_realtime_clock_ts(ts)
extern void do_gettimeofday(struct timeval *tv);
extern int do_settimeofday(struct timespec *ts);

extern void timeofday_init(void);
extern int clocksource_hres_capable(void);

#ifndef CONFIG_IS_TICK_BASED
#define arch_getoffset() (0)
#else
extern unsigned long arch_getoffset(void);
#endif

#else /* CONFIG_GENERIC_TIME */

#define timeofday_init()
extern void getnstimeofday (struct timespec *tv);

#endif /* CONFIG_GENERIC_TIME */

#endif /* _LINUX_TIMEOFDAY_H */
