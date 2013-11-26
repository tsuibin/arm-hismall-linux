/*  linux/include/linux/clocksource.h
 *
 *  This file contains the structure definitions for clocksources.
 *
 *  If you are not a clocksource, or the time of day code, you should
 *  not be including this file!
 */
#ifndef _LINUX_CLOCKSOURCE_H
#define _LINUX_CLOCKSOURCE_H

#include <linux/types.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/list.h>
#include <asm/io.h>
#include <asm/div64.h>

/**
 * struct clocksource - hardware abstraction for a free running counter
 *	Provides mostly state-free accessors to the underlying hardware.
 *
 * @name:		ptr to clocksource name
 * @list:		list head for registration
 * @rating:		rating value for selection (higher is better)
 *			To avoid rating inflation the following
 *			list should give you a guide as to how
 *			to assign your clocksource a rating
 *			1-99: Unfit for real use
 *				Only available for bootup and testing purposes.
 *			100-199: Base level usability.
 *				Functional for real use, but not desired.
 *			200-299: Good.
 *				A correct and usable clocksource.
 *			300-399: Desired.
 *				A reasonably fast and accurate clocksource.
 *			400-499: Perfect
 *				The ideal clocksource. A must-use where
 *				available.
 * @type:		defines clocksource type
 * @read_fnct:		returns a cycle value
 * @ptr:		ptr to MMIO'ed counter
 * @mask:		bitmask for two's complement
 *			subtraction of non 64 bit counters
 * @mult:		cycle to nanosecond multiplier
 * @shift:		cycle to nanosecond divisor (power of two)
 * @update_callback:	called when safe to alter clocksource values
 */
struct clocksource {
	char* name;
	struct list_head list;
	int rating;
	enum {
		CLOCKSOURCE_FUNCTION,
		CLOCKSOURCE_CYCLES,
		CLOCKSOURCE_MMIO_32,
		CLOCKSOURCE_MMIO_64
	} type;
	cycle_t (*read_fnct)(void);
	void __iomem *mmio_ptr;
	cycle_t mask;
	u32 mult;
	u32 shift;
	int (*update_callback)(void);
};


/**
 * clocksource_khz2mult - calculates mult from khz and shift
 * @khz:		Clocksource frequency in KHz
 * @shift_constant:	Clocksource shift factor
 *
 * Helper functions that converts a khz counter frequency to a timsource
 * multiplier, given the clocksource shift value
 */
static inline u32 clocksource_khz2mult(u32 khz, u32 shift_constant)
{
	/*  khz = cyc/(Million ns)
	 *  mult/2^shift  = ns/cyc
	 *  mult = ns/cyc * 2^shift
	 *  mult = 1Million/khz * 2^shift
	 *  mult = 1000000 * 2^shift / khz
	 *  mult = (1000000<<shift) / khz
	 */
	u64 tmp = ((u64)1000000) << shift_constant;
	tmp += khz/2; /* round for do_div */
	do_div(tmp, khz);
	return (u32)tmp;
}

/**
 * clocksource_hz2mult - calculates mult from hz and shift
 * @hz:			Clocksource frequency in Hz
 * @shift_constant:	Clocksource shift factor
 *
 * Helper functions that converts a hz counter
 * frequency to a timsource multiplier, given the
 * clocksource shift value
 */
static inline u32 clocksource_hz2mult(u32 hz, u32 shift_constant)
{
	/*  hz = cyc/(Billion ns)
	 *  mult/2^shift  = ns/cyc
	 *  mult = ns/cyc * 2^shift
	 *  mult = 1Billion/hz * 2^shift
	 *  mult = 1000000000 * 2^shift / hz
	 *  mult = (1000000000<<shift) / hz
	 */
	u64 tmp = ((u64)1000000000) << shift_constant;
	tmp += hz/2; /* round for do_div */
	do_div(tmp, hz);
	return (u32)tmp;
}


#ifndef readq
/* Provide an a way to atomically read a u64 on a 32bit arch */
static inline unsigned long long clocksource_readq(void __iomem *addr)
{
	u32 low, high;
	/* loop is required to make sure we get an atomic read */
	do {
		high = readl(addr+4);
		low = readl(addr);
	} while (high != readl(addr+4));

	return low | (((unsigned long long)high) << 32LL);
}
#else
#define clocksource_readq(x) readq(x)
#endif


/**
 * read_clocksource: - Access the clocksource's current cycle value
 * @cs:		pointer to clocksource being read
 *
 * Uses the clocksource to return the current cycle_t value
 */
static inline cycle_t read_clocksource(struct clocksource *cs)
{
	switch (cs->type) {
	case CLOCKSOURCE_MMIO_32:
		return (cycle_t)readl(cs->mmio_ptr);
	case CLOCKSOURCE_MMIO_64:
		return (cycle_t)clocksource_readq(cs->mmio_ptr);
	case CLOCKSOURCE_CYCLES:
		return (cycle_t)get_cycles();
	default:/* case: CLOCKSOURCE_FUNCTION */
		return cs->read_fnct();
	}
}

/**
 * ppm_to_mult_adj - Converts shifted ppm values to mult adjustment
 * @cs:		Pointer to clocksource
 * @ppm:	Shifted PPM value
 *
 * Helper which converts a shifted ppm value to clocksource mult_adj value.
 *
 * XXX - this could use some optimization
 */
static inline int ppm_to_mult_adj(struct clocksource *cs, int ppm)
{
	u64 mult_adj;
	int ret_adj;

	/* The basic math is as follows:
	 *     cyc * mult/2^shift * (1 + ppm/MILL) = scaled ns
	 * We want to precalculate the ppm factor so it can be added
	 * to the multiplyer saving the extra multiplication step.
	 *     cyc * (mult/2^shift + (mult/2^shift) * (ppm/MILL)) =
	 *     cyc * (mult/2^shift + (mult*ppm/MILL)/2^shift) =
	 *     cyc * (mult + (mult*ppm/MILL))/2^shift =
	 * Thus we want to calculate the value of:
	 *     mult*ppm/MILL
	 */
 	mult_adj = abs(ppm);
	mult_adj = (mult_adj * cs->mult)>>SHIFT_USEC;
	mult_adj += 1000000/2; /* round for div*/
	do_div(mult_adj, 1000000);
	if (ppm < 0)
		ret_adj = -(int)mult_adj;
	else
		ret_adj = (int)mult_adj;
	return ret_adj;
}

/**
 * cyc2ns - converts clocksource cycles to nanoseconds
 * @cs:		Pointer to clocksource
 * @ntp_adj:	Multiplier adjustment value
 * @cycles:	Cycles
 *
 * Uses the clocksource and ntp ajdustment to convert cycle_ts to nanoseconds.
 *
 * XXX - This could use some mult_lxl_ll() asm optimization
 */
static inline nsec_t cyc2ns(struct clocksource *cs, int ntp_adj, cycle_t cycles)
{
	u64 ret;
	ret = (u64)cycles;
	ret *= (cs->mult + ntp_adj);
	ret >>= cs->shift;
	return (nsec_t)ret;
}

/**
 * cyc2ns_rem - converts clocksource cycles to nanoseconds w/ remainder
 * @cs:		Pointer to clocksource
 * @ntp_adj:	Multiplier adjustment value
 * @cycles:	Cycles
 * @rem:	Remainder
 *
 * Uses the clocksource and ntp ajdustment interval to convert cycle_t to
 * nanoseconds. Add in remainder portion which is stored in (ns<<cs->shift)
 * units and save the new remainder off.
 *
 * XXX - This could use some mult_lxl_ll() asm optimization.
 */
static inline nsec_t cyc2ns_rem(struct clocksource *cs, int ntp_adj, cycle_t cycles, u64* rem)
{
	u64 ret;
	ret = (u64)cycles;
	ret *= (cs->mult + ntp_adj);
	if (rem) {
		ret += *rem;
		*rem = ret & ((1<<cs->shift)-1);
	}
	ret >>= cs->shift;
	return (nsec_t)ret;
}


/**
 * struct clocksource_interval - Fixed interval conversion structure
 *
 * @cycles:	A specified number of cycles
 * @nsecs:	The number of nanoseconds equivalent to the cycles value
 * @remainder:	Non-integer nanosecond remainder stored in (ns<<cs->shift) units
 * @remainder_ns_overflow:	Value at which the remainder is equal to
 *				one second
 *
 * This is a optimization structure used by cyc2ns_fixed_rem() to avoid the
 * multiply in cyc2ns().
 *
 * Unless you're the timeofday_periodic_hook, you should not be using this!
 */
struct clocksource_interval {
	cycle_t cycles;
	nsec_t nsecs;
	u64 remainder;
	u64 remainder_ns_overflow;
};

/**
 * calculate_clocksource_interval - Calculates a clocksource interval struct
 *
 * @c:		Pointer to clocksource.
 * @adj:	Multiplyer adjustment.
 * @length_nsec: Desired interval length in nanoseconds.
 *
 * Calculates a fixed cycle/nsec interval for a given clocksource/adjustment
 * pair and interval request.
 *
 * Unless you're the timeofday_periodic_hook, you should not be using this!
 */
static inline struct clocksource_interval
calculate_clocksource_interval(struct clocksource *c, long adj,
				unsigned long length_nsec)
{
	struct clocksource_interval ret;
	u64 tmp;

	/* XXX - All of this could use a whole lot of optimization */
	tmp = length_nsec;
	tmp <<= c->shift;
	do_div(tmp, c->mult+adj);

	ret.cycles = (cycle_t)tmp;
	if(ret.cycles == 0)
		ret.cycles = 1;

	ret.remainder = 0;
	ret.remainder_ns_overflow = 1 << c->shift;
	ret.nsecs = cyc2ns_rem(c, adj, ret.cycles, &ret.remainder);

	return ret;
}

/**
 * cyc2ns_fixed_rem -
 *	converts clocksource cycles to nanoseconds using fixed intervals
 *
 * @interval:	precalculated clocksource_interval structure
 * @cycles:	Number of clocksource cycles
 * @rem:	Remainder
 *
 * Uses a precalculated fixed cycle/nsec interval to convert cycles to
 * nanoseconds. Returns the unaccumulated cycles in the cycles pointer as
 * well as uses and updates the value at the remainder pointer
 *
 * Unless you're the timeofday_periodic_hook, you should not be using this!
 */
static inline nsec_t cyc2ns_fixed_rem(struct clocksource_interval interval, cycle_t *cycles, u64* rem)
{
	nsec_t delta_nsec = 0;
	while(*cycles > interval.cycles) {
		delta_nsec += interval.nsecs;
		*cycles -= interval.cycles;
		*rem += interval.remainder;
		while(*rem > interval.remainder_ns_overflow) {
			*rem -= interval.remainder_ns_overflow;
			delta_nsec += 1;
		}
	}
	return delta_nsec;
}


/* used to install a new clocksource */
void register_clocksource(struct clocksource*);
void reselect_clocksource(void);
struct clocksource* get_next_clocksource(void);
#endif
