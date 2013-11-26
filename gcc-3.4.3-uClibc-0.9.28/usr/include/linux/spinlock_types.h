#ifndef __LINUX_SPINLOCK_TYPES_H
#define __LINUX_SPINLOCK_TYPES_H

/*
 * include/linux/spinlock_types.h - generic spinlock type definitions
 *                                  and initializers
 *
 * portions Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 */

#if defined(CONFIG_SMP)
# include <asm/spinlock_types.h>
#else
# include <linux/spinlock_types_up.h>
#endif

typedef struct {
	__raw_spinlock_t raw_lock;
#if defined(CONFIG_PREEMPT) && defined(CONFIG_SMP)
	unsigned int break_lock;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	unsigned int magic, owner_cpu;
	void *owner;
#endif
} raw_spinlock_t;

#define RAW_SPINLOCK_MAGIC	0xdead4ead

typedef struct {
	__raw_rwlock_t raw_lock;
#if defined(CONFIG_PREEMPT) && defined(CONFIG_SMP)
	unsigned int break_lock;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	unsigned int magic, owner_cpu;
	void *owner;
#endif
} raw_rwlock_t;

#define RWLOCK_MAGIC		0xdeaf1eed

#define SPINLOCK_OWNER_INIT	((void *)-1L)

#ifdef CONFIG_DEBUG_SPINLOCK
# define RAW_SPIN_LOCK_UNLOCKED						\
	(raw_spinlock_t) {	.raw_lock = __RAW_SPIN_LOCK_UNLOCKED,	\
				.magic = RAW_SPINLOCK_MAGIC,		\
				.owner = SPINLOCK_OWNER_INIT,		\
				.owner_cpu = -1 }
# define RAW_RW_LOCK_UNLOCKED						\
	(raw_rwlock_t) {	.raw_lock = __RAW_RW_LOCK_UNLOCKED,	\
				.magic = RWLOCK_MAGIC,			\
				.owner = SPINLOCK_OWNER_INIT,		\
				.owner_cpu = -1 }
#else
# define _RAW_SPIN_LOCK_UNLOCKED \
		{	.raw_lock = __RAW_SPIN_LOCK_UNLOCKED }
# define _RAW_RW_LOCK_UNLOCKED \
		{	.raw_lock = __RAW_RW_LOCK_UNLOCKED }
# define RAW_SPIN_LOCK_UNLOCKED \
	(raw_spinlock_t) _RAW_SPIN_LOCK_UNLOCKED
# define RAW_RW_LOCK_UNLOCKED \
	(raw_rwlock_t) _RAW_RW_LOCK_UNLOCKED
#endif

#define DEFINE_RAW_SPINLOCK(name) \
	raw_spinlock_t name __cacheline_aligned_in_smp = RAW_SPIN_LOCK_UNLOCKED

#define __DEFINE_RAW_SPINLOCK(name) \
	raw_spinlock_t name = RAW_SPIN_LOCK_UNLOCKED

#define DEFINE_RAW_RWLOCK(name) \
	raw_rwlock_t name __cacheline_aligned_in_smp = RAW_RW_LOCK_UNLOCKED

#define __raw_spin_lock_init(lock) \
	do { *(lock) = RAW_SPIN_LOCK_UNLOCKED; } while (0)
#define __raw_rwlock_init(lock) \
	do { *(lock) = RAW_RW_LOCK_UNLOCKED; } while (0)

#endif /* __LINUX_SPINLOCK_TYPES_H */
