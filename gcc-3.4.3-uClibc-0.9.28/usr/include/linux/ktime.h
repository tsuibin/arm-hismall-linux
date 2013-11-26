/*
 *  include/linux/ktime.h
 *
 *  ktime_t - nanosecond-resolution time format.
 *
 *   Copyright(C) 2005, Thomas Gleixner <tglx@linutronix.de>
 *   Copyright(C) 2005, Red Hat, Inc., Ingo Molnar
 *
 *  data type definitions, declarations, prototypes and macros.
 *
 *  Started by: Thomas Gleixner and Ingo Molnar
 *
 *  For licencing details see kernel-base/COPYING
 */
#ifndef _LINUX_KTIME_H
#define _LINUX_KTIME_H

#include <linux/jiffies.h>
#include <linux/timeofday.h>

/*
 * ktime_t:
 *
 * On 64-bit CPUs a single 64-bit variable is used to store the ktimers
 * internal representation of time values in scalar nanoseconds. The
 * design plays out best on 64-bit CPUs, where most conversions are
 * NOPs and most arithmetic ktime_t operations are plain arithmetic
 * operations.
 *
 * On 32-bit CPUs an optimized representation of the timespec structure
 * is used to avoid expensive conversions from and to timespecs. The
 * endian-aware order of the tv struct members is choosen to allow
 * mathematical operations on the tv64 member of the union too, which
 * for certain operations produces better code.
 *
 * For architectures with efficient support for 64/32-bit conversions the
 * plain scalar nanosecond based representation can be selected by the
 * config switch CONFIG_KTIME_SCALAR.
 */

#define KTIME_ZERO			0
#define KTIME_MAX			(~((u64)1 << 63))

/*
 * ktime_t definitions when using the 64-bit scalar representation:
 */

#if (BITS_PER_LONG == 64) || defined(CONFIG_KTIME_SCALAR)

typedef s64 ktime_t;

/* Define a ktime_t variable and initialize it to zero: */
#define DEFINE_KTIME(kt)		ktime_t kt = 0

/*
 * Compare two ktime_t variables. The comparison operand is
 * given as a literal in the macro call (e.g. <, >, ==):
 *
 * ( E.g. "ktime_cmp(t1, <, t2) is still more readable to programmers
 *   than ktime_before()/ktime_after() would be. )
 */
#define ktime_cmp(a, op, b)		((a) op (b))

/*
 * Compare a ktime_t variable and a constant. The comparison operand is
 * given as a literal in the macro call (e.g. <, >, ==):
 */
#define ktime_cmp_val(a, op, b)		((a) op (b))

/**
 * ktime_set - Set a ktime_t variable from a seconds/nanoseconds value
 *
 * @secs:	seconds to set
 * @nsecs:	nanoseconds to set
 *
 * Return the ktime_t representation of the value
 */
#define ktime_set(sec, nsec)	(((s64)(sec) * NSEC_PER_SEC) + (s64)(nsec))

/*
 * Set a ktime_t variable to a value in a scalar nanosecond representation
 *
 * NOTE: use only with KTIME_ZERO or KTIME_MAX to maintain compability
 * with the union type version.
 */
#define ktime_set_scalar(kt, s)		(kt) = (s)

/*
 * The following 3 macros are used for the nanosleep restart handling
 * to store the "low" and "high" part of a 64-bit ktime variable.
 * (on 32-bit CPUs the restart block has 32-bit fields, so we have to
 *  split the 64-bit value up into two pieces)
 *
 * In the scalar representation we have to split up the 64-bit scalar:
 */

/* Set the "low" and "high" part of a ktime_t variable: */
#define ktime_set_low_high(l, h)	((s64)((u64)(l)) | (((s64)(h)) << 32))

/* Get the "low" part of a ktime_t variable: */
#define ktime_get_low(kt)		((kt) & 0xFFFFFFFF)

/* Get the "high" part of a ktime_t variable: */
#define ktime_get_high(kt)		((kt) >> 32)

/* Subtract two ktime_t variables. rem = lhs -rhs: */
#define ktime_sub(lhs, rhs)		((lhs) - (rhs))

/* Add two ktime_t variables. res = lhs + rhs: */
#define ktime_add(lhs, rhs)		((lhs) + (rhs))

/*
 * Add a ktime_t variable and a scalar nanosecond value.
 * res = kt + nsval:
 */
#define ktime_add_ns(kt, nsval)		((kt) + (nsval))

/* convert a timespec to ktime_t format: */
#define timespec_to_ktime(ts)		ktime_set((ts).tv_sec, (ts).tv_nsec)

/* convert a timeval to ktime_t format: */
#define timeval_to_ktime(tv)		ktime_set((tv).tv_sec, (tv).tv_usec * 1000)

/* Map the ktime_t to timespec conversion to ns_to_timespec function */
#define ktime_to_timespec(ts, kt)	ns_to_timespec(ts, kt)

/* Map the ktime_t to timeval conversion to ns_to_timeval function */
#define ktime_to_timeval(tv, kt)	ns_to_timeval(tv, kt)

/* Map the ktime_t to clock_t conversion to the inline in jiffies.h: */
#define ktime_to_clock_t(kt)		nsec_to_clock_t(kt)

/* Convert ktime_t to nanoseconds - NOP in the scalar storage format: */
#define ktime_to_ns(kt)			(kt)

#else

/*
 * Helper macros/inlines to get the ktime_t math right in the timespec
 * representation. The macros are sometimes ugly - their actual use is
 * pretty okay-ish, given the circumstances. We do all this for
 * performance reasons. The pure scalar nsec_t based code was nice and
 * simple, but created too many 64-bit / 32-bit conversions and divisions.
 *
 * Be especially aware that negative values are represented in a way
 * that the tv.sec field is negative and the tv.nsec field is greater
 * or equal to zero but less than nanoseconds per second. This is the
 * same representation which is used by timespecs.
 *
 *   tv.sec < 0 and 0 >= tv.nsec < NSEC_PER_SEC
 */

typedef union {
	s64	tv64;
	struct {
# ifdef __BIG_ENDIAN
	s32	sec, nsec;
# else
	s32	nsec, sec;
# endif
	} tv;
} ktime_t;

/* Define a ktime_t variable and initialize it to zero: */
#define DEFINE_KTIME(kt)		ktime_t kt = { .tv64 = 0 }

/*
 * Compare two ktime_t variables. The comparison operand is
 * given as a literal in the macro call (e.g. <, >, ==):
 */
#define ktime_cmp(a, op, b)		((a).tv64 op (b).tv64)

/*
 * Compare a ktime_t variable and a constant. The comparison operand is
 * given as a literal in the macro call (e.g. <, >, ==):
 */
#define ktime_cmp_val(a, op, b)		((a).tv64 op (b))

/* Set a ktime_t variable to a value in sec/nsec representation: */
static inline ktime_t ktime_set(long secs, unsigned long nsecs)
{
	return (ktime_t) { .tv.sec = secs, .tv.nsec = nsecs };
}

/*
 * Set the scalar value of a ktime variable (union type)
 * NOTE: use only with KTIME_ZERO or KTIME_MAX!
 */
#define ktime_set_scalar(kt, s)		(kt).tv64 = (s)

/*
 * The following 3 macros are used for the nanosleep restart handling
 * to store the "low" and "high" part of a 64-bit ktime variable.
 * (on 32-bit CPUs the restart block has 32-bit fields, so we have to
 *  split the 64-bit value up into two pieces)
 *
 * In the union type representation this is just storing and restoring
 * the sec and nsec members of the tv structure:
 */

/* Set the "low" and "high" part of a ktime_t variable: */
#define ktime_set_low_high(l, h)	ktime_set(h, l)

/* Get the "low" part of a ktime_t variable: */
#define ktime_get_low(kt)		(kt).tv.nsec

/* Get the "high" part of a ktime_t variable: */
#define ktime_get_high(kt)		(kt).tv.sec

/**
 * ktime_sub - subtract two ktime_t variables
 *
 * @lhs:	minuend
 * @rhs:	subtrahend
 *
 * Returns the remainder of the substraction
 */
static inline ktime_t ktime_sub(ktime_t lhs, ktime_t rhs)
{
	ktime_t res;

	res.tv64 = lhs.tv64 - rhs.tv64;
	if (res.tv.nsec < 0)
		res.tv.nsec += NSEC_PER_SEC;

	return res;
}

/**
 * ktime_add - add two ktime_t variables
 *
 * @add1:	addend1
 * @add2:	addend2
 *
 * Returns the sum of addend1 and addend2
 */
static inline ktime_t ktime_add(ktime_t add1, ktime_t add2)
{
	ktime_t res;

	res.tv64 = add1.tv64 + add2.tv64;
	/*
	 * performance trick: the (u32) -NSEC gives 0x00000000Fxxxxxxx
	 * so we subtract NSEC_PER_SEC and add 1 to the upper 32 bit.
	 *
	 * it's equivalent to:
	 *   tv.nsec -= NSEC_PER_SEC
	 *   tv.sec ++;
	 */
	if (res.tv.nsec >= NSEC_PER_SEC)
		res.tv64 += (u32)-NSEC_PER_SEC;

	return res;
}

/**
 * ktime_add_ns - Add a scalar nanoseconds value to a ktime_t variable
 *
 * @kt:		addend
 * @nsec:	the scalar nsec value to add
 *
 * Returns the sum of kt and nsec in ktime_t format
 */
extern ktime_t ktime_add_ns(ktime_t kt, u64 nsec);

/**
 * timespec_to_ktime - convert a timespec to ktime_t format
 *
 * @ts:		the timespec variable to convert
 *
 * Returns a ktime_t variable with the converted timespec value
 */
static inline ktime_t timespec_to_ktime(struct timespec ts)
{
	return (ktime_t) { .tv.sec = (s32)ts.tv_sec,
			   .tv.nsec = (s32)ts.tv_nsec };
}

/**
 * timeval_to_ktime - convert a timeval to ktime_t format
 *
 * @tv:		the timeval variable to convert
 *
 * Returns a ktime_t variable with the converted timeval value
 */
static inline ktime_t timeval_to_ktime(struct timeval tv)
{
	return (ktime_t) { .tv.sec = (s32)tv.tv_sec,
			   .tv.nsec = (s32)tv.tv_usec * 1000 };
}

/**
 * ktime_to_timespec - convert a ktime_t variable to timespec format
 *
 * @ts:		pointer to timespec variable to store result
 * @kt:		the ktime_t variable to convert
 *
 * Stores the timespec representation of the ktime value in
 * the timespec variable pointed to by @ts
 */
static inline void ktime_to_timespec(struct timespec *ts, ktime_t kt)
{
	ts->tv_sec = (time_t) kt.tv.sec;
	ts->tv_nsec = (long) kt.tv.nsec;
}

/**
 * ktime_to_timeval - convert a ktime_t variable to timeval format
 *
 * @tv:		pointer to timeval variable to store result
 * @kt:		the ktime_t variable to convert
 *
 * Stores the timeval representation of the ktime value in
 * the timeval variable pointed to by @tv
 */
static inline void ktime_to_timeval(struct timeval *tv, ktime_t kt)
{
	tv->tv_sec = (time_t) kt.tv.sec;
	tv->tv_usec = (suseconds_t) (kt.tv.nsec / NSEC_PER_USEC);
}

/**
 * ktime_to_clock_t - convert a ktime_t variable to clock_t format
 * @kt:		the ktime_t variable to convert
 *
 * Returns a clock_t variable with the converted value
 */
static inline clock_t ktime_to_clock_t(ktime_t kt)
{
	return nsec_to_clock_t( (u64) kt.tv.sec * NSEC_PER_SEC + kt.tv.nsec);
}

/**
 * ktime_to_ns - convert a ktime_t variable to scalar nanoseconds
 * @kt:		the ktime_t variable to convert
 *
 * Returns the scalar nanoseconds representation of kt
 */
static inline u64 ktime_to_ns(ktime_t kt)
{
	return (u64) kt.tv.sec * NSEC_PER_SEC + kt.tv.nsec;
}

#endif

#define KTIME_LOW_RES		(NSEC_PER_SEC/HZ)

#ifdef CONFIG_GENERIC_TIME

#if (BITS_PER_LONG == 64) || defined(CONFIG_KTIME_SCALAR)
#define ktime_get() get_monotonic_clock_ns()
#define ktime_get_real() get_realtime_clock_ns()
#define ktime_get_real_offset() get_realtime_offset_ns()
#else

/* Get the monotonic time in ktime_t format: */
extern ktime_t ktime_get(void);

/* Get the real (wall-) time in ktime_t format: */
extern ktime_t ktime_get_real(void);

#endif

#define ktime_get_ts(ts) get_monotonic_clock_ts(ts)
#define ktime_get_real_ts(ts) get_realtime_clock_ts(ts)

#else /* CONFIG_GENERIC_TIME */

/* Get the monotonic time in ktime_t format: */
extern ktime_t ktime_get(void);

/* Get the real (wall-) time in ktime_t format: */
extern ktime_t ktime_get_real(void);

/* Get the monotonic time in timespec format: */
extern void ktime_get_ts(struct timespec *ts);

/* Get the real (wall-) time in timespec format: */
#define ktime_get_real_ts(ts)	getnstimeofday(ts)

#endif /* !CONFIG_GENERIC_TIME */

#endif
