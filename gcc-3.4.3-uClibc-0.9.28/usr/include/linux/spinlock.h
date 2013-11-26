#ifndef __LINUX_SPINLOCK_H
#define __LINUX_SPINLOCK_H

/*
 * include/linux/spinlock.h - generic spinlock/rwlock declarations
 *
 * here's the role of the various spinlock/rwlock related include files:
 *
 * on SMP builds:
 *
 *  asm/spinlock_types.h: contains the raw_spinlock_t/raw_rwlock_t and the
 *                        initializers
 *
 *  linux/spinlock_types.h:
 *                        defines the generic type and initializers
 *
 *  asm/spinlock.h:       contains the __raw_spin_*()/etc. lowlevel
 *                        implementations, mostly inline assembly code
 *
 *   (also included on UP-debug builds:)
 *
 *  linux/spinlock_api_smp.h:
 *                        contains the prototypes for the _spin_*() APIs.
 *
 *  linux/spinlock.h:     builds the final spin_*() APIs.
 *
 * on UP builds:
 *
 *  linux/spinlock_type_up.h:
 *                        contains the generic, simplified UP spinlock type.
 *                        (which is an empty structure on non-debug builds)
 *
 *  linux/spinlock_types.h:
 *                        defines the generic type and initializers
 *
 *  linux/spinlock_up.h:
 *                        contains the __raw_spin_*()/etc. version of UP
 *                        builds. (which are NOPs on non-debug, non-preempt
 *                        builds)
 *
 *   (included on UP-non-debug builds:)
 *
 *  linux/spinlock_api_up.h:
 *                        builds the _spin_*() APIs.
 *
 *  linux/spinlock.h:     builds the final spin_*() APIs.
 */

#include <linux/config.h>
#include <linux/preempt.h>
#include <linux/linkage.h>
#include <linux/compiler.h>
#include <linux/thread_info.h>
#include <linux/kernel.h>
#include <linux/cache.h>
#include <linux/stringify.h>

#include <asm/system.h>

/*
 * Must define these before including other files, inline functions need them
 */
#define LOCK_SECTION_NAME                       \
        ".text.lock." __stringify(KBUILD_BASENAME)

#define LOCK_SECTION_START(extra)               \
        ".subsection 1\n\t"                     \
        extra                                   \
        ".ifndef " LOCK_SECTION_NAME "\n\t"     \
        LOCK_SECTION_NAME ":\n\t"               \
        ".endif\n"

#define LOCK_SECTION_END                        \
        ".previous\n\t"

#define __lockfunc fastcall __attribute__((section(".spinlock.text")))

/*
 * Pull the raw_spinlock_t and raw_rwlock_t definitions:
 */
#include <linux/spinlock_types.h>

extern int __lockfunc generic__raw_read_trylock(raw_rwlock_t *lock);

/*
 * Pull the __raw*() functions/declarations (UP-nondebug doesnt need them):
 */
#if defined(CONFIG_SMP)
# include <asm/spinlock.h>
#else
# include <linux/spinlock_up.h>
#endif

/*
 * Pull the RT types:
 */
#include <linux/rt_lock.h>

/*
 * Pull the _spin_*()/_read_*()/_write_*() functions/declarations:
 */
#if defined(CONFIG_SMP) || defined(CONFIG_DEBUG_SPINLOCK)
# include <linux/spinlock_api_smp.h>
#else
# include <linux/spinlock_api_up.h>
#endif

#if 0
#ifdef CONFIG_DEBUG_SPINLOCK
 extern void _raw_spin_lock(raw_spinlock_t *lock);
#define _raw_spin_lock_flags(lock, flags) __raw_spin_lock(lock)
 extern int _raw_spin_trylock(raw_spinlock_t *lock);
 extern void _raw_spin_unlock(raw_spinlock_t *lock);

 extern void _raw_read_lock(raw_rwlock_t *lock);
 extern int _raw_read_trylock(raw_rwlock_t *lock);
 extern void _raw_read_unlock(raw_rwlock_t *lock);
 extern void _raw_write_lock(raw_rwlock_t *lock);
 extern int _raw_write_trylock(raw_rwlock_t *lock);
 extern void _raw_write_unlock(raw_rwlock_t *lock);
#else
# define _raw_spin_unlock(lock)		__raw_spin_unlock(&(lock)->raw_lock)
# define _raw_spin_trylock(lock)	__raw_spin_trylock(&(lock)->raw_lock)
# define _raw_spin_lock(lock)		__raw_spin_lock(&(lock)->raw_lock)
# define _raw_spin_lock_flags(lock, flags) \
		__raw_spin_lock_flags(&(lock)->raw_lock, *(flags))
# define _raw_read_lock(rwlock)		__raw_read_lock(&(rwlock)->raw_lock)
# define _raw_write_lock(rwlock)	__raw_write_lock(&(rwlock)->raw_lock)
# define _raw_read_unlock(rwlock)	__raw_read_unlock(&(rwlock)->raw_lock)
# define _raw_write_unlock(rwlock)	__raw_write_unlock(&(rwlock)->raw_lock)
# define _raw_read_trylock(rwlock)	__raw_read_trylock(&(rwlock)->raw_lock)
# define _raw_write_trylock(rwlock)	__raw_write_trylock(&(rwlock)->raw_lock)
#endif
#endif

extern int __bad_spinlock_type(void);

/*
 * The following ones are only implemented on PREEMPT_RT, but
 * the type selection macros need the prototypes even though the
 * functions never get called (hence, linked):
 */
#if !defined(CONFIG_PREEMPT_RT) || \
	defined(CONFIG_DEBUG_RT_LOCKING_MODE) || \
	defined(CONFIG_RT_DEADLOCK_DETECT) || \
	defined(CONFIG_DEBUG_IRQ_FLAGS)
# undef DEBUG_RT_DONT_INLINE
# define DEBUG_RT_DONT_INLINE
#endif

#ifdef DEBUG_RT_DONT_INLINE
extern void __lockfunc _spin_lock(spinlock_t *lock);
extern void __lockfunc _spin_lock_bh(spinlock_t *lock);
extern void __lockfunc _spin_lock_irq(spinlock_t *lock);
extern void __lockfunc _spin_unlock(spinlock_t *lock);
extern void __lockfunc _spin_unlock_no_resched(spinlock_t *lock);
extern void __lockfunc _spin_unlock_bh(spinlock_t *lock);
extern void __lockfunc _spin_unlock_irq(spinlock_t *lock);
extern unsigned long __lockfunc _spin_lock_irqsave(spinlock_t *lock);
extern void __lockfunc _spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);
#else
/*
 * Inlined shortcuts for the most common APIs:
 */
extern void __down_mutex(struct rt_mutex *lock);
extern void __up_mutex_nosavestate(struct rt_mutex *lock);
extern void __up_mutex_savestate(struct rt_mutex *lock);

static inline void _spin_lock(spinlock_t *lock)
{
	__down_mutex(&lock->lock);
}
static inline void _spin_lock_bh(spinlock_t *lock)
{
	__down_mutex(&lock->lock);
}
static inline void _spin_lock_irq(spinlock_t *lock)
{
	__down_mutex(&lock->lock);
}
static inline unsigned long __lockfunc _spin_lock_irqsave(spinlock_t *lock)
{
	__down_mutex(&lock->lock);
	return 0;
}
static inline void _spin_unlock(spinlock_t *lock)
{
	__up_mutex_savestate(&lock->lock);
}
static inline void _spin_unlock_no_resched(spinlock_t *lock)
{
	__up_mutex_savestate(&lock->lock);
}
static inline void _spin_unlock_bh(spinlock_t *lock)
{
	__up_mutex_savestate(&lock->lock);
}
static inline void _spin_unlock_irq(spinlock_t *lock)
{
	__up_mutex_savestate(&lock->lock);
}
static inline void _spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags)
{
	__up_mutex_savestate(&lock->lock);
}
#endif
extern void __lockfunc _spin_unlock_wait(spinlock_t *lock);
extern int __lockfunc _spin_trylock(spinlock_t *lock);
extern int __lockfunc _spin_trylock_bh(spinlock_t *lock);
extern int __lockfunc _spin_trylock_irq(spinlock_t *lock);
extern int __lockfunc _spin_trylock_irqsave(spinlock_t *lock, unsigned long *flags);
extern int _spin_can_lock(spinlock_t *lock);
extern int _spin_is_locked(spinlock_t *lock);
extern int atomic_dec_and_spin_lock(atomic_t *atomic, spinlock_t *lock);
extern void _spin_lock_init(spinlock_t *lock, char *name, char *file, int line);

#undef TYPE_EQUAL
#define TYPE_EQUAL(lock, type) \
		__builtin_types_compatible_p(typeof(lock), type *)

#define PICK_OP(type, optype, op, lock)				\
do {								\
	if (TYPE_EQUAL((lock), type))				\
		_raw_##optype##op((type *)(lock));		\
	else if (TYPE_EQUAL(lock, spinlock_t))			\
		_spin##op((spinlock_t *)(lock));		\
	else __bad_spinlock_type();				\
} while (0)

#define PICK_OP_RET(type, optype, op, lock...)			\
({								\
	unsigned long __ret;					\
								\
	if (TYPE_EQUAL((lock), type))	  			\
		__ret = _raw_##optype##op((type *)(lock));	\
	else if (TYPE_EQUAL(lock, spinlock_t))			\
		__ret = _spin##op((spinlock_t *)(lock));	\
	else __ret = __bad_spinlock_type();			\
								\
	__ret;							\
})

#define PICK_OP2(type, optype, op, lock, flags)			\
do {								\
	if (TYPE_EQUAL((lock), type))				\
		_raw_##optype##op((type *)(lock), flags);	\
	else if (TYPE_EQUAL(lock, spinlock_t))			\
		_spin##op((spinlock_t *)(lock), flags);		\
	else __bad_spinlock_type();				\
} while (0)

#define PICK_OP2_RET(type, optype, op, lock, flags)		\
({								\
	unsigned long __ret;					\
								\
	if (TYPE_EQUAL((lock), type))				\
		__ret = _raw_##optype##op((type *)(lock), flags);\
	else if (TYPE_EQUAL(lock, spinlock_t))			\
		__ret = _spin##op((spinlock_t *)(lock), flags);	\
	else __bad_spinlock_type();				\
								\
	__ret;							\
})


extern int __lockfunc _read_trylock(rwlock_t *rwlock);
extern int __lockfunc _write_trylock(rwlock_t *rwlock);
extern int _read_can_lock(rwlock_t *rwlock);
extern int _write_can_lock(rwlock_t *rwlock);
extern void __lockfunc _write_lock(rwlock_t *rwlock);
extern void __lockfunc _read_lock(rwlock_t *rwlock);
extern void __lockfunc _write_unlock(rwlock_t *rwlock);
extern void __lockfunc _read_unlock(rwlock_t *rwlock);
extern unsigned long __lockfunc _write_lock_irqsave(rwlock_t *rwlock);
extern unsigned long __lockfunc _read_lock_irqsave(rwlock_t *rwlock);
extern void __lockfunc _write_lock_irq(rwlock_t *rwlock);
extern void __lockfunc _read_lock_irq(rwlock_t *rwlock);
extern void __lockfunc _write_lock_bh(rwlock_t *rwlock);
extern void __lockfunc _read_lock_bh(rwlock_t *rwlock);
extern void __lockfunc _write_unlock_irq(rwlock_t *rwlock);
extern void __lockfunc _read_unlock_irq(rwlock_t *rwlock);
extern void __lockfunc _write_unlock_bh(rwlock_t *rwlock);
extern void __lockfunc _read_unlock_bh(rwlock_t *rwlock);
extern void __lockfunc _write_unlock_irqrestore(rwlock_t *rwlock, unsigned long flags);
extern void __lockfunc _read_unlock_irqrestore(rwlock_t *rwlock, unsigned long flags);
extern void _rwlock_init(rwlock_t *rwlock, char *name, char *file, int line);

#define __PICK_RW_OP(type, optype, op, lock)				\
do {									\
	if (TYPE_EQUAL((lock), type))					\
		_raw_##optype##op((type *)(lock));			\
	else if (TYPE_EQUAL(lock, rwlock_t))				\
		##op((rwlock_t *)(lock));				\
	else __bad_spinlock_type();					\
} while (0)

#define PICK_RW_OP(type, optype, op, lock)				\
do {									\
	if (TYPE_EQUAL((lock), type))					\
		_raw_##optype##op((type *)(lock));			\
	else if (TYPE_EQUAL(lock, rwlock_t))				\
		_##optype##op((rwlock_t *)(lock));			\
	else __bad_spinlock_type();					\
} while (0)

#define __PICK_RW_OP_RET(type, optype, op, lock...)			\
({									\
	unsigned long __ret;						\
									\
	if (TYPE_EQUAL((lock), type))	  				\
		__ret = _raw_##optype##op((type *)(lock));		\
	else if (TYPE_EQUAL(lock, rwlock_t))				\
		__ret = _##optype##op((rwlock_t *)(lock));		\
	else __ret = __bad_spinlock_type();				\
									\
	__ret;								\
})

#define PICK_RW_OP_RET(type, optype, op, lock...)			\
({									\
	unsigned long __ret;						\
									\
	if (TYPE_EQUAL((lock), type))	  				\
		__ret = _raw_##optype##op((type *)(lock));		\
	else if (TYPE_EQUAL(lock, rwlock_t))				\
		__ret = _##optype##op((rwlock_t *)(lock));		\
	else __ret = __bad_spinlock_type();				\
									\
	__ret;								\
})

#define PICK_RW_OP2(type, optype, op, lock, flags)			\
do {									\
	if (TYPE_EQUAL((lock), type))					\
		_raw_##optype##op((type *)(lock), flags);		\
	else if (TYPE_EQUAL(lock, rwlock_t))				\
		_##optype##op((rwlock_t *)(lock), flags);		\
	else __bad_spinlock_type();					\
} while (0)

#define _raw_spin_lock_init __raw_spin_lock_init

#define PICK_OP_INIT(type, optype, op, lock)				\
do {									\
	if (TYPE_EQUAL((lock), type))					\
		_raw_##optype##op((type *)(lock));			\
	else if (TYPE_EQUAL(lock, spinlock_t))				\
		_spin##op((spinlock_t *)(lock), #lock, __FILE__, __LINE__); \
	else __bad_spinlock_type();					\
} while (0)


#define spin_lock_init(lock) \
		PICK_OP_INIT(raw_spinlock_t, spin, _lock_init, lock)

#define _raw_rwlock_init __raw_rwlock_init

#define __PICK_RW_OP_INIT(type, optype, op, lock)			\
do {									\
	if (TYPE_EQUAL((lock), type))					\
		_raw_##optype##op((type *)(lock));			\
	else if (TYPE_EQUAL(lock, rwlock_t))				\
		_##optype##op((rwlock_t *)(lock), #lock, __FILE__, __LINE__);\
	else __bad_spinlock_type();					\
} while (0)


#define rwlock_init(lock) \
		__PICK_RW_OP_INIT(raw_rwlock_t, rwlock, _init, lock)

#define _raw_spin_is_locked(lock) __raw_spin_is_locked(&(lock)->raw_lock)

#define spin_is_locked(lock) \
		PICK_OP_RET(raw_spinlock_t, spin, _is_locked, lock)

#define _raw_spin_unlock_wait(lock) __raw_spin_unlock_wait(&(lock)->raw_lock)

#define spin_unlock_wait(lock) \
		PICK_OP(raw_spinlock_t, spin, _unlock_wait, lock)
/*
 * Define the various spin_lock and rw_lock methods.  Note we define these
 * regardless of whether CONFIG_SMP or CONFIG_PREEMPT are set. The various
 * methods are defined as nops in the case they are not required.
 */
// #define spin_trylock(lock)	_spin_trylock(lock)
#define spin_trylock(lock)	__cond_lock(PICK_OP_RET(raw_spinlock_t, spin, _trylock, lock))

//#define read_trylock(lock)	_read_trylock(lock)
#define read_trylock(lock)	__cond_lock(PICK_RW_OP_RET(raw_rwlock_t, read, _trylock, lock))

//#define write_trylock(lock)	_write_trylock(lock)
#define write_trylock(lock)	__cond_lock(PICK_RW_OP_RET(raw_rwlock_t, write, _trylock, lock))

#define _raw_spin_can_lock(lock) __raw_spin_can_lock(&(lock)->raw_lock)
#define _raw_read_can_lock(lock) __raw_read_can_lock(&(lock)->raw_lock)
#define _raw_write_can_lock(lock) __raw_write_can_lock(&(lock)->raw_lock)

#define spin_can_lock(lock)	__cond_lock(PICK_OP_RET(raw_spinlock_t, spin, _can_lock, lock))
#define read_can_lock(lock)	__cond_lock(PICK_RW_OP_RET(raw_rwlock_t, read, _can_lock, lock))
#define write_can_lock(lock)	__cond_lock(PICK_RW_OP_RET(raw_rwlock_t, write, _can_lock, lock))

// #define spin_lock(lock)	_spin_lock(lock)
#define spin_lock(lock)		PICK_OP(raw_spinlock_t, spin, _lock, lock)

//#define write_lock(lock)	_write_lock(lock)
#define write_lock(lock)	PICK_RW_OP(raw_rwlock_t, write, _lock, lock)

// #define read_lock(lock)		_read_lock(lock)
#define read_lock(lock)		PICK_RW_OP(raw_rwlock_t, read, _lock, lock)

#ifdef CONFIG_SMP
// #define spin_lock_irqsave(lock, flags)	flags = _spin_lock_irqsave(lock)
// #define read_lock_irqsave(lock, flags)	flags = _read_lock_irqsave(lock)
// #define write_lock_irqsave(lock, flags)	flags = _write_lock_irqsave(lock)
#else
// #define spin_lock_irqsave(lock, flags)	_spin_lock_irqsave(lock, flags)
// #define read_lock_irqsave(lock, flags)	_read_lock_irqsave(lock, flags)
// #define write_lock_irqsave(lock, flags)	_write_lock_irqsave(lock, flags)
#endif

# define spin_lock_irqsave(lock, flags) \
	flags = PICK_OP_RET(raw_spinlock_t, spin, _lock_irqsave, lock)
# define read_lock_irqsave(lock, flags) \
	flags = PICK_RW_OP_RET(raw_rwlock_t, read, _lock_irqsave, lock)
# define write_lock_irqsave(lock, flags) \
	flags = PICK_RW_OP_RET(raw_rwlock_t, write, _lock_irqsave, lock)

// #define spin_lock_irq(lock)	_spin_lock_irq(lock)
// #define spin_lock_bh(lock)	_spin_lock_bh(lock)
#define spin_lock_irq(lock)	PICK_OP(raw_spinlock_t, spin, _lock_irq, lock)
#define spin_lock_bh(lock)	PICK_OP(raw_spinlock_t, spin, _lock_bh, lock)

// #define read_lock_irq(lock)	_read_lock_irq(lock)
// #define read_lock_bh(lock)	_read_lock_bh(lock)
#define read_lock_irq(lock)	PICK_RW_OP(raw_rwlock_t, read, _lock_irq, lock)
#define read_lock_bh(lock)	PICK_RW_OP(raw_rwlock_t, read, _lock_bh, lock)

// #define write_lock_irq(lock)		_write_lock_irq(lock)
// #define write_lock_bh(lock)		_write_lock_bh(lock)
#define write_lock_irq(lock)	PICK_RW_OP(raw_rwlock_t, write, _lock_irq, lock)
#define write_lock_bh(lock)	PICK_RW_OP(raw_rwlock_t, write, _lock_bh, lock)

// #define spin_unlock(lock)	_spin_unlock(lock)
// #define write_unlock(lock)	_write_unlock(lock)
// #define read_unlock(lock)	_read_unlock(lock)
#define spin_unlock(lock)	PICK_OP(raw_spinlock_t, spin, _unlock, lock)
#define read_unlock(lock)	PICK_RW_OP(raw_rwlock_t, read, _unlock, lock)
#define write_unlock(lock)	PICK_RW_OP(raw_rwlock_t, write, _unlock, lock)

// #define spin_unlock(lock)	_spin_unlock_no_resched(lock)
#define spin_unlock_no_resched(lock) \
			PICK_OP(raw_spinlock_t, spin, _unlock_no_resched, lock)

//#define spin_unlock_irqrestore(lock, flags)
//		_spin_unlock_irqrestore(lock, flags)
//#define spin_unlock_irq(lock)	_spin_unlock_irq(lock)
//#define spin_unlock_bh(lock)	_spin_unlock_bh(lock)
#define spin_unlock_irqrestore(lock, flags) \
	PICK_OP2(raw_spinlock_t, spin, _unlock_irqrestore, lock, flags)
#define spin_unlock_irq(lock)	PICK_OP(raw_spinlock_t, spin, _unlock_irq, lock)
#define spin_unlock_bh(lock)	PICK_OP(raw_spinlock_t, spin, _unlock_bh, lock)

// #define read_unlock_irqrestore(lock, flags)
// 		_read_unlock_irqrestore(lock, flags)
// #define read_unlock_irq(lock)	_read_unlock_irq(lock)
// #define read_unlock_bh(lock)	_read_unlock_bh(lock)
#define read_unlock_irqrestore(lock, flags) \
		PICK_RW_OP2(raw_rwlock_t, read, _unlock_irqrestore, lock, flags)
#define read_unlock_irq(lock) PICK_RW_OP(raw_rwlock_t, read, _unlock_irq, lock)
#define read_unlock_bh(lock) PICK_RW_OP(raw_rwlock_t, read, _unlock_bh, lock)

// #define write_unlock_irqrestore(lock, flags)
// 	_write_unlock_irqrestore(lock, flags)
// #define write_unlock_irq(lock)			_write_unlock_irq(lock)
// #define write_unlock_bh(lock)			_write_unlock_bh(lock)
#define write_unlock_irqrestore(lock, flags) \
	PICK_RW_OP2(raw_rwlock_t, write, _unlock_irqrestore, lock, flags)
#define write_unlock_irq(lock) PICK_RW_OP(raw_rwlock_t, write, _unlock_irq, lock)
#define write_unlock_bh(lock) PICK_RW_OP(raw_rwlock_t, write, _unlock_bh, lock)

// #define spin_trylock_bh(lock)	_spin_trylock_bh(lock)
#define spin_trylock_bh(lock)	__cond_lock(PICK_OP_RET(raw_spinlock_t, spin, _trylock_bh, lock))

// #define spin_trylock_irq(lock)

#define spin_trylock_irq(lock)	__cond_lock(PICK_OP_RET(raw_spinlock_t, spin, _trylock_irq, lock))

// #define spin_trylock_irqsave(lock, flags)

#define spin_trylock_irqsave(lock, flags)	__cond_lock(PICK_OP2_RET(raw_spinlock_t, spin, _trylock_irqsave, lock, &flags))

/* "lock on reference count zero" */
#ifndef ATOMIC_DEC_AND_LOCK
# include <asm/atomic.h>
  extern int _atomic_dec_and_raw_spin_lock(atomic_t *atomic, raw_spinlock_t *lock);
#endif

#define atomic_dec_and_lock(atomic, lock)				\
__cond_lock(({								\
	unsigned long __ret;						\
									\
	if (TYPE_EQUAL(lock, raw_spinlock_t))				\
		__ret = _atomic_dec_and_raw_spin_lock(atomic,		\
					(raw_spinlock_t *)(lock));	\
	else if (TYPE_EQUAL(lock, spinlock_t))				\
		__ret = atomic_dec_and_spin_lock(atomic,		\
					(spinlock_t *)(lock));		\
	else __ret = __bad_spinlock_type();				\
									\
	__ret;								\
}))


/*
 *  bit-based spin_lock()
 *
 * Don't use this unless you really need to: spin_lock() and spin_unlock()
 * are significantly faster.
 */
static inline void bit_spin_lock(int bitnum, unsigned long *addr)
{
	/*
	 * Assuming the lock is uncontended, this never enters
	 * the body of the outer loop. If it is contended, then
	 * within the inner loop a non-atomic test is used to
	 * busywait with less bus contention for a good time to
	 * attempt to acquire the lock bit.
	 */
#if defined(CONFIG_SMP) || defined(CONFIG_DEBUG_SPINLOCK) || defined(CONFIG_PREEMPT)
	while (test_and_set_bit(bitnum, addr))
		while (test_bit(bitnum, addr))
			cpu_relax();
#endif
	__acquire(bitlock);
}

/*
 * Return true if it was acquired
 */
static inline int bit_spin_trylock(int bitnum, unsigned long *addr)
{
#if defined(CONFIG_SMP) || defined(CONFIG_DEBUG_SPINLOCK) || defined(CONFIG_PREEMPT)
	if (test_and_set_bit(bitnum, addr))
		return 0;
#endif
	__acquire(bitlock);
	return 1;
}

/*
 *  bit-based spin_unlock()
 */
static inline void bit_spin_unlock(int bitnum, unsigned long *addr)
{
#if defined(CONFIG_SMP) || defined(CONFIG_DEBUG_SPINLOCK) || defined(CONFIG_PREEMPT)
	BUG_ON(!test_bit(bitnum, addr));
	smp_mb__before_clear_bit();
	clear_bit(bitnum, addr);
#endif
	__release(bitlock);
}

/*
 * Return true if the lock is held.
 */
static inline int bit_spin_is_locked(int bitnum, unsigned long *addr)
{
#if defined(CONFIG_SMP) || defined(CONFIG_DEBUG_SPINLOCK) || defined(CONFIG_PREEMPT)
	return test_bit(bitnum, addr);
#else
	return 1;
#endif
}

/**
 * __raw_spin_can_lock - would __raw_spin_trylock() succeed?
 * @lock: the spinlock in question.
 */
#define __raw_spin_can_lock(lock)            (!__raw_spin_is_locked(lock))

#endif /* __LINUX_SPINLOCK_H */

