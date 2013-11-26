#ifndef __LINUX_RT_LOCK_H
#define __LINUX_RT_LOCK_H

/*
 * Real-Time Preemption Support
 *
 * started by Ingo Molnar:
 *
 *  Copyright (C) 2004, 2005 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * This file contains the main data structure definitions.
 */
#include <linux/config.h>
#include <linux/list.h>
#include <linux/plist.h>
#include <asm/atomic.h>
#include <linux/spinlock_types.h>

/*
 * This is the core locking object used by PREEMPT_RT.
 * This one handles all the logic necessary, the other locking
 * objects (spinlocks, rwlocks, semaphores and rw-semaphores)
 * all use this synchronization object internally:
 */
struct rt_mutex {
	raw_spinlock_t		wait_lock;
	struct plist		wait_list;
	struct thread_info	*owner;
# ifdef CONFIG_DEBUG_RT_LOCKING_MODE
	raw_spinlock_t		debug_slock;
	raw_rwlock_t		debug_rwlock;
# endif
# ifdef CONFIG_RT_DEADLOCK_DETECT
	int			save_state;
	struct list_head	held_list;
	unsigned long		acquire_eip;
	char 			*name, *file;
	int			line;
# endif
# ifdef CONFIG_DEBUG_PREEMPT
	int			was_preempt_off;
# endif
	unsigned int		mutex_attr;
};

/*
 * This is the control structure for tasks blocked on an
 * RT mutex:
 */
struct rt_mutex_waiter {
	struct rt_mutex		*lock;
	struct plist		list;
	struct plist		pi_list;
	struct thread_info	*ti;
#ifdef CONFIG_RT_DEADLOCK_DETECT
	unsigned long eip;
#endif
};

#ifdef CONFIG_PREEMPT_RT

#ifdef CONFIG_DEBUG_PREEMPT
# define __WAS_PREEMPT_OFF(x)	, .was_preempt_off = x
#else
# define __WAS_PREEMPT_OFF(x)
#endif

#ifdef CONFIG_RT_DEADLOCK_DETECT
# define __RT_MUTEX_DEADLOCK_DETECT_INITIALIZER(lockname) \
	, .name = #lockname, .file = __FILE__, .line = __LINE__
#else
# define __RT_MUTEX_DEADLOCK_DETECT_INITIALIZER(lockname)
#endif

#ifdef CONFIG_DEBUG_RT_LOCKING_MODE
# define __RT_MUTEX_DEBUG_RT_LOCKING_MODE_INITIALIZER \
	, .debug_slock = _RAW_SPIN_LOCK_UNLOCKED \
	, .debug_rwlock = _RAW_RW_LOCK_UNLOCKED
#else
# define __RT_MUTEX_DEBUG_RT_LOCKING_MODE_INITIALIZER
#endif

/*
 * FIXME: on SMP it's hard to initialize plists in the percpu.data area
 */
#ifdef CONFIG_SMP
# define __PLIST_INIT(lockname)
#else
# define __PLIST_INIT(lockname) \
	, .wait_list = PLIST_INIT((lockname).wait_list, 140 /*MAX_PRIO*/)
#endif

#define __RT_MUTEX_INITIALIZER(lockname) \
	{ .wait_lock = _RAW_SPIN_LOCK_UNLOCKED \
	__PLIST_INIT(lockname) \
	__WAS_PREEMPT_OFF(0) \
	__RT_MUTEX_DEADLOCK_DETECT_INITIALIZER(lockname) \
	__RT_MUTEX_DEBUG_RT_LOCKING_MODE_INITIALIZER }

/*
 * RW-semaphores are an RT mutex plus a reader-depth count.
 *
 * Note that the semantics are different from the usual
 * Linux rw-sems, in PREEMPT_RT mode we do not allow
 * multiple readers to hold the lock at once, we only allow
 * a read-lock owner to read-lock recursively. This is
 * better for latency, makes the implementation inherently
 * fair and makes it simpler as well:
 */
struct rw_semaphore {
	struct rt_mutex		lock;
	int			read_depth;
};

/*
 * rwlocks - an RW semaphore plus lock-break field:
 */
typedef struct {
	struct rw_semaphore	lock;
	unsigned int		break_lock;
} rwlock_t;

# ifdef CONFIG_RT_DEADLOCK_DETECT
#  define __RW_LOCK_UNLOCKED(lockname) \
	.wait_lock = _RAW_SPIN_LOCK_UNLOCKED, .save_state = 1 \
	__PLIST_INIT((lockname).lock.lock) \
	, .file = __FILE__, .line = __LINE__ \
	__WAS_PREEMPT_OFF(1) \
	__RT_MUTEX_DEBUG_RT_LOCKING_MODE_INITIALIZER
#  define _RW_LOCK_UNLOCKED(lockname) \
	(rwlock_t) { { { __RW_LOCK_UNLOCKED(lockname), .name = #lockname } } }
#  define RW_LOCK_UNLOCKED(lockname) \
	(rwlock_t) { { { __RW_LOCK_UNLOCKED(lockname) } } }
# else
#  define RW_LOCK_UNLOCKED(lockname) (rwlock_t) \
	{ { { .wait_lock = _RAW_SPIN_LOCK_UNLOCKED \
	__PLIST_INIT(((lockname).lock.lock)) \
	__RT_MUTEX_DEBUG_RT_LOCKING_MODE_INITIALIZER } } }
#  define _RW_LOCK_UNLOCKED(lockname) RW_LOCK_UNLOCKED(lockname)
# endif
#else /* !PREEMPT_RT */
  typedef raw_rwlock_t rwlock_t;
# ifdef CONFIG_DEBUG_SPINLOCK
# define _RW_LOCK_UNLOCKED(lockname)					\
	(rwlock_t)	{	.raw_lock = __RAW_RW_LOCK_UNLOCKED,	\
				.magic = RWLOCK_MAGIC,			\
				.owner = SPINLOCK_OWNER_INIT,		\
				.owner_cpu = -1 }
# else
#  define _RW_LOCK_UNLOCKED(lockname)					\
	(rwlock_t)	{	.raw_lock = __RAW_RW_LOCK_UNLOCKED }
# endif
# define RW_LOCK_UNLOCKED(lockname)	_RW_LOCK_UNLOCKED(lockname)
#endif

#ifdef CONFIG_PREEMPT_RT

/*
 * spinlocks - an RT mutex plus lock-break field:
 */
typedef struct {
	struct rt_mutex lock;
	unsigned int break_lock;
} spinlock_t;

#ifdef CONFIG_RT_DEADLOCK_DETECT
# define __SPIN_LOCK_UNLOCKED(lockname) \
	.wait_lock = _RAW_SPIN_LOCK_UNLOCKED \
	__PLIST_INIT(((lockname).lock)) \
	, .save_state = 1, .file = __FILE__, .line = __LINE__ \
	__WAS_PREEMPT_OFF(1) \
	__RT_MUTEX_DEBUG_RT_LOCKING_MODE_INITIALIZER
# define _SPIN_LOCK_UNLOCKED(lockname) \
	(spinlock_t) { { __SPIN_LOCK_UNLOCKED(lockname), .name = #lockname } }
# define SPIN_LOCK_UNLOCKED(lockname) \
	(spinlock_t) { { __SPIN_LOCK_UNLOCKED(lockname) } }
#else
# define SPIN_LOCK_UNLOCKED(lockname) \
	(spinlock_t) { { .wait_lock = _RAW_SPIN_LOCK_UNLOCKED \
	__PLIST_INIT(((lockname).lock)) \
	__RT_MUTEX_DEBUG_RT_LOCKING_MODE_INITIALIZER } }
# define _SPIN_LOCK_UNLOCKED(lockname) SPIN_LOCK_UNLOCKED(lockname)
#endif
#else /* !PREEMPT_RT */
  typedef raw_spinlock_t spinlock_t;
# ifdef CONFIG_DEBUG_SPINLOCK
#  define _SPIN_LOCK_UNLOCKED(lockname)					\
	(spinlock_t)	{	.raw_lock = __RAW_SPIN_LOCK_UNLOCKED,	\
				.magic = SPINLOCK_MAGIC,		\
				.owner = SPINLOCK_OWNER_INIT,		\
				.owner_cpu = -1 }
# else
#  define _SPIN_LOCK_UNLOCKED(lockname) \
	(spinlock_t)	{	.raw_lock = __RAW_SPIN_LOCK_UNLOCKED }
# endif
# define SPIN_LOCK_UNLOCKED(lockname) _SPIN_LOCK_UNLOCKED(lockname)
#endif

#define DEFINE_SPINLOCK(name) \
	spinlock_t name __cacheline_aligned_in_smp = _SPIN_LOCK_UNLOCKED(name)

#define DEFINE_RWLOCK(name) \
	rwlock_t name __cacheline_aligned_in_smp = _RW_LOCK_UNLOCKED(name)

#ifdef CONFIG_PREEMPT_RT

/*
 * Semaphores - an RT-mutex plus the semaphore count:
 */
struct semaphore {
	atomic_t count;
	struct rt_mutex lock;
};

#define DECLARE_MUTEX(name) \
struct semaphore name = \
	{ .count = { 1 }, .lock = __RT_MUTEX_INITIALIZER(name.lock) }

/*
 * DECLARE_MUTEX_LOCKED() is deprecated: very hard to initialize properly
 * and it also often signals abuse of semaphores. So we redirect it to
 * compat semaphores:
 */
#define DECLARE_MUTEX_LOCKED COMPAT_DECLARE_MUTEX_LOCKED

extern void FASTCALL(__sema_init(struct semaphore *sem, int val, char *name, char *file, int line));

#define rt_sema_init(sem, val) \
		__sema_init(sem, val, #sem, __FILE__, __LINE__)

extern void FASTCALL(__init_MUTEX(struct semaphore *sem, char *name, char *file, int line));
#define rt_init_MUTEX(sem) \
		__init_MUTEX(sem, #sem, __FILE__, __LINE__)

extern void there_is_no_init_MUTEX_LOCKED_for_RT_semaphores(void);

/*
 * No locked initialization for RT semaphores
 */
#define rt_init_MUTEX_LOCKED(sem) \
		there_is_no_init_MUTEX_LOCKED_for_RT_semaphores()
extern void FASTCALL(rt_down(struct semaphore *sem));
extern int FASTCALL(rt_down_interruptible(struct semaphore *sem));
extern int FASTCALL(rt_down_trylock(struct semaphore *sem));
extern void FASTCALL(rt_up(struct semaphore *sem));
extern int FASTCALL(rt_sem_is_locked(struct semaphore *sem));
extern int FASTCALL(rt_sema_count(struct semaphore *sem));


extern int __bad_func_type(void);

#undef TYPE_EQUAL
#define TYPE_EQUAL(var, type) \
		__builtin_types_compatible_p(typeof(var), type *)

#define PICK_FUNC_1ARG(type1, type2, func1, func2, arg)			\
do {									\
	if (TYPE_EQUAL((arg), type1))					\
		func1((type1 *)(arg));					\
	else if (TYPE_EQUAL((arg), type2))				\
		func2((type2 *)(arg));					\
	else __bad_func_type();						\
} while (0)

#define PICK_FUNC_1ARG_RET(type1, type2, func1, func2, arg)		\
({									\
	unsigned long __ret;						\
									\
	if (TYPE_EQUAL((arg), type1))					\
		__ret = func1((type1 *)(arg));				\
	else if (TYPE_EQUAL((arg), type2))				\
		__ret = func2((type2 *)(arg));				\
	else __ret = __bad_func_type();					\
									\
	__ret;								\
})

#define PICK_FUNC_2ARG(type1, type2, func1, func2, arg0, arg1)		\
do {									\
	if (TYPE_EQUAL((arg0), type1))					\
		func1((type1 *)(arg0), arg1);				\
	else if (TYPE_EQUAL((arg0), type2))				\
		func2((type2 *)(arg0), arg1);				\
	else __bad_func_type();						\
} while (0)

#define sema_init(sem, val) \
	PICK_FUNC_2ARG(struct compat_semaphore, struct semaphore, \
		compat_sema_init, rt_sema_init, sem, val)

#define init_MUTEX(sem) \
	PICK_FUNC_1ARG(struct compat_semaphore, struct semaphore, \
		compat_init_MUTEX, rt_init_MUTEX, sem)

#define init_MUTEX_LOCKED(sem) \
	PICK_FUNC_1ARG(struct compat_semaphore, struct semaphore, \
		compat_init_MUTEX_LOCKED, rt_init_MUTEX_LOCKED, sem)

#define down(sem) \
	PICK_FUNC_1ARG(struct compat_semaphore, struct semaphore, \
		compat_down, rt_down, sem)

#define down_interruptible(sem) \
	PICK_FUNC_1ARG_RET(struct compat_semaphore, struct semaphore, \
		compat_down_interruptible, rt_down_interruptible, sem)

#define down_trylock(sem) \
	PICK_FUNC_1ARG_RET(struct compat_semaphore, struct semaphore, \
		compat_down_trylock, rt_down_trylock, sem)

#define up(sem) \
	PICK_FUNC_1ARG(struct compat_semaphore, struct semaphore, \
		compat_up, rt_up, sem)

#define sem_is_locked(sem) \
	PICK_FUNC_1ARG_RET(struct compat_semaphore, struct semaphore, \
		compat_sem_is_locked, rt_sem_is_locked, sem)

#define sema_count(sem) \
	PICK_FUNC_1ARG_RET(struct compat_semaphore, struct semaphore, \
		compat_sema_count, rt_sema_count, sem)

/*
 * rwsems:
 */

#define __RWSEM_INITIALIZER(lockname) \
	{ .lock = __RT_MUTEX_INITIALIZER(lockname.lock) }

#define DECLARE_RWSEM(lockname) \
	struct rw_semaphore lockname = __RWSEM_INITIALIZER(lockname)

extern void FASTCALL(__init_rwsem(struct rw_semaphore *rwsem, int mutex,
				char *name, char *file, int line));

#define rt_init_rwsem(sem) __init_rwsem(sem, 0, #sem, __FILE__, __LINE__)

extern void FASTCALL(rt_down_read(struct rw_semaphore *rwsem));
extern int FASTCALL(rt_down_read_trylock(struct rw_semaphore *rwsem));
extern void FASTCALL(rt_down_write(struct rw_semaphore *rwsem));
extern int FASTCALL(rt_down_write_trylock(struct rw_semaphore *rwsem));
extern void FASTCALL(rt_up_read(struct rw_semaphore *rwsem));
extern void FASTCALL(rt_up_write(struct rw_semaphore *rwsem));
extern void FASTCALL(rt_downgrade_write(struct rw_semaphore *rwsem));
extern int FASTCALL(rt_rwsem_is_locked(struct rw_semaphore *rwsem));

#define init_rwsem(rwsem) \
	PICK_FUNC_1ARG(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_init_rwsem, rt_init_rwsem, rwsem)

#define down_read(rwsem) \
	PICK_FUNC_1ARG(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_down_read, rt_down_read, rwsem)

#define down_read_trylock(rwsem) \
	PICK_FUNC_1ARG_RET(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_down_read_trylock, rt_down_read_trylock, rwsem)

#define down_write(rwsem) \
	PICK_FUNC_1ARG(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_down_write, rt_down_write, rwsem)

#define down_write_trylock(rwsem) \
	PICK_FUNC_1ARG_RET(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_down_write_trylock, rt_down_write_trylock, rwsem)

#define up_read(rwsem) \
	PICK_FUNC_1ARG(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_up_read, rt_up_read, rwsem)

#define up_write(rwsem) \
	PICK_FUNC_1ARG(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_up_write, rt_up_write, rwsem)

#define downgrade_write(rwsem) \
	PICK_FUNC_1ARG(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_downgrade_write, rt_downgrade_write, rwsem)

#define rwsem_is_locked(rwsem) \
	PICK_FUNC_1ARG_RET(struct compat_rw_semaphore, struct rw_semaphore, \
		compat_rwsem_is_locked, rt_rwsem_is_locked, rwsem)

#endif /* CONFIG_PREEMPT_RT */

extern void FASTCALL(up_futex(struct rt_mutex *lock));
extern int FASTCALL(down_futex(struct rt_mutex *lock, unsigned long time, pid_t owner_pid));
extern int FASTCALL(rt_mutex_owned_by(struct rt_mutex *lock, struct thread_info *t));
extern int FASTCALL(rt_mutex_has_waiters(struct rt_mutex *lock));
extern struct thread_info *FASTCALL(rt_mutex_owner(struct rt_mutex *lock));
extern void FASTCALL(init_rt_mutex(struct rt_mutex *lock, int save_state,
				   char *name, char *file, int line));

#endif

