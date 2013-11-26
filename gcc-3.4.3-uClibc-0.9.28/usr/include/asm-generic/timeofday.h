/*  linux/include/asm-generic/timeofday.h
 *
 *  This file contains the asm-generic interface
 *  to the arch specific calls used by the time of day subsystem
 */
#ifndef _ASM_GENERIC_TIMEOFDAY_H
#define _ASM_GENERIC_TIMEOFDAY_H
#include <linux/types.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/timeofday.h>
#include <linux/clocksource.h>

#include <asm/div64.h>
#ifdef CONFIG_GENERIC_TIME
/* Required externs */
extern nsec_t read_persistent_clock(void);
extern void sync_persistent_clock(struct timespec ts);

#ifdef CONFIG_GENERIC_TIME_VSYSCALL
extern void arch_update_vsyscall_gtod(struct timespec wall_time,
				cycle_t offset_base, struct clocksource* clock,
				int ntp_adj);
#else
#define arch_update_vsyscall_gtod(x,y,z,w) {}
#endif /* CONFIG_GENERIC_TIME_VSYSCALL */

#endif /* CONFIG_GENERIC_TIME */
#endif
