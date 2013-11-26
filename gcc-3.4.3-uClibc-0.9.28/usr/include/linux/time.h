#ifndef _LINUX_TIME_H
#define _LINUX_TIME_H

#include <linux/types.h>

#ifdef __KERNEL__
#include <linux/calc64.h>
#include <linux/seqlock.h>
#endif

#ifndef _STRUCT_TIMESPEC
#define _STRUCT_TIMESPEC
struct timespec {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* nanoseconds */
};
#endif /* _STRUCT_TIMESPEC */

struct timeval {
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* microseconds */
};

struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};

#ifdef __KERNEL__

/* timeofday base types */
typedef s64 nsec_t;
typedef u64 cycle_t;

/* Parameters used to convert the timespec values */
#define MSEC_PER_SEC (1000L)
#define USEC_PER_SEC (1000000L)
#define NSEC_PER_SEC (1000000000L)
#define NSEC_PER_USEC (1000L)

static __inline__ int timespec_equal(struct timespec *a, struct timespec *b) 
{ 
	return (a->tv_sec == b->tv_sec) && (a->tv_nsec == b->tv_nsec);
} 

#define timespec_valid(ts) \
(((ts)->tv_sec >= 0) && (((unsigned) (ts)->tv_nsec) < NSEC_PER_SEC))

extern unsigned long
mktime (unsigned int year, unsigned int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec);

extern struct timespec xtime;
extern struct timespec wall_to_monotonic;
extern raw_seqlock_t xtime_lock;

static inline unsigned long get_seconds(void)
{ 
	return xtime.tv_sec;
}

extern void set_normalized_timespec (struct timespec *ts, time_t sec, long nsec);

struct timespec current_kernel_time(void);

#define CURRENT_TIME (current_kernel_time())
#define CURRENT_TIME_SEC ((struct timespec) { xtime.tv_sec, 0 })

extern void do_gettimeofday(struct timeval *tv);
extern int do_settimeofday(struct timespec *tv);
extern int do_sys_settimeofday(struct timespec *tv, struct timezone *tz);
extern void do_posix_clock_monotonic_gettime(struct timespec *ts);
extern long do_utimes(char __user * filename, struct timeval * times);
struct itimerval;
extern int do_setitimer(int which, struct itimerval *value, struct itimerval *ovalue);
extern int do_getitimer(int which, struct itimerval *value);

extern struct timespec timespec_trunc(struct timespec t, unsigned gran);


/**
 * timespec_to_ns - Convert timespec to nanoseconds
 * @ts:		pointer to the timespec variable to be converted
 *
 * Returns the scalar nanosecond representation of the timespec
 * variable
 */
static inline nsec_t timespec_to_ns(struct timespec *ts)
{
	nsec_t res = (nsec_t) ts->tv_sec * NSEC_PER_SEC;

	return res + (nsec_t) ts->tv_nsec;
}

/**
 * timeval_to_ns - Convert timeval to nanoseconds
 * @ts:		pointer to the timeval variable to be converted
 *
 * Returns the scalar nanosecond representation of the timeval
 * variable
 */
static inline nsec_t timeval_to_ns(struct timeval *tv)
{
	nsec_t res = (nsec_t) tv->tv_sec * NSEC_PER_SEC;

	return res + (nsec_t) tv->tv_usec * NSEC_PER_USEC;
}

extern void ns_to_timespec(struct timespec *ts, nsec_t nsec);
extern void ns_to_timeval(struct timeval *tv, nsec_t nsec);

static inline void normalize_timespec(struct timespec *ts)
{
	while ((unsigned long)ts->tv_nsec > NSEC_PER_SEC) {
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec++;
	}
}

static inline struct timespec timespec_add_ns(struct timespec a, nsec_t ns)
{
	while(ns > NSEC_PER_SEC) {
		normalize_timespec(&a);
		ns -= NSEC_PER_SEC;
		a.tv_nsec += NSEC_PER_SEC;
	}
	a.tv_nsec += ns;
	normalize_timespec(&a);

	return a;
}

#endif /* __KERNEL__ */

#define NFDBITS			__NFDBITS

#define FD_SETSIZE		__FD_SETSIZE
#define FD_SET(fd,fdsetp)	__FD_SET(fd,fdsetp)
#define FD_CLR(fd,fdsetp)	__FD_CLR(fd,fdsetp)
#define FD_ISSET(fd,fdsetp)	__FD_ISSET(fd,fdsetp)
#define FD_ZERO(fdsetp)		__FD_ZERO(fdsetp)

/*
 * Names of the interval timers, and structure
 * defining a timer setting.
 */
#define	ITIMER_REAL	0
#define	ITIMER_VIRTUAL	1
#define	ITIMER_PROF	2

struct  itimerspec {
        struct  timespec it_interval;    /* timer period */
        struct  timespec it_value;       /* timer expiration */
};

struct	itimerval {
	struct	timeval it_interval;	/* timer interval */
	struct	timeval it_value;	/* current value */
};


/*
 * The IDs of the various system clocks (for POSIX.1b interval timers).
 */
#define CLOCK_REALTIME		 0
#define CLOCK_MONOTONIC	  	 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID	 3

/*
 * The IDs of various hardware clocks
 */
#define CLOCK_SGI_CYCLE 10
#define MAX_CLOCKS 16
#define CLOCKS_MASK  (CLOCK_REALTIME | CLOCK_MONOTONIC)
#define CLOCKS_MONO (CLOCK_MONOTONIC)

/*
 * The various flags for setting POSIX.1b interval timers.
 */

#define TIMER_ABSTIME 0x01


#endif
