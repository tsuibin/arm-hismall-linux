#ifndef __LINUX_SPINLOCK_API_SMP_H
#define __LINUX_SPINLOCK_API_SMP_H

#ifndef __LINUX_SPINLOCK_H
# error "please don't include this file directly"
#endif

/*
 * include/linux/spinlock_api_smp.h
 *
 * spinlock API declarations on SMP (and debug)
 * (implemented in kernel/spinlock.c)
 *
 * portions Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 */

int in_lock_functions(unsigned long addr);

#define assert_spin_locked(x)	BUG_ON(!spin_is_locked(x))

void __lockfunc _raw_spin_lock(raw_spinlock_t *lock)		__acquires(raw_spinlock_t);
void __lockfunc _raw_read_lock(raw_rwlock_t *lock)		__acquires(raw_rwlock_t);
void __lockfunc _raw_write_lock(raw_rwlock_t *lock)		__acquires(raw_rwlock_t);
void __lockfunc _raw_spin_lock_bh(raw_spinlock_t *lock)		__acquires(raw_spinlock_t);
void __lockfunc _raw_read_lock_bh(raw_rwlock_t *lock)		__acquires(raw_rwlock_t);
void __lockfunc _raw_write_lock_bh(raw_rwlock_t *lock)		__acquires(raw_rwlock_t);
void __lockfunc _raw_spin_lock_irq(raw_spinlock_t *lock)	__acquires(raw_spinlock_t);
void __lockfunc _raw_read_lock_irq(raw_rwlock_t *lock)		__acquires(raw_rwlock_t);
void __lockfunc _raw_write_lock_irq(raw_rwlock_t *lock)		__acquires(raw_rwlock_t);
unsigned long __lockfunc _raw_spin_lock_irqsave(raw_spinlock_t *lock)
							__acquires(raw_spinlock_t);
unsigned long __lockfunc _raw_read_lock_irqsave(raw_rwlock_t *lock)
							__acquires(raw_rwlock_t);
unsigned long __lockfunc _raw_write_lock_irqsave(raw_rwlock_t *lock)
							__acquires(raw_rwlock_t);
int __lockfunc _raw_spin_trylock(raw_spinlock_t *lock);
int __lockfunc _raw_read_trylock(raw_rwlock_t *lock);
int __lockfunc _raw_write_trylock(raw_rwlock_t *lock);
int __lockfunc _raw_spin_trylock_irqsave(raw_spinlock_t *lock,
					 unsigned long *flags);
int __lockfunc _raw_spin_trylock_bh(raw_spinlock_t *lock);
void __lockfunc _raw_spin_unlock(raw_spinlock_t *lock)		__releases(raw_spinlock_t);
void __lockfunc _raw_spin_unlock_no_resched(raw_spinlock_t *lock) __releases(raw_spinlock_t);
void __lockfunc _raw_read_unlock(raw_rwlock_t *lock)		__releases(raw_rwlock_t);
void __lockfunc _raw_write_unlock(raw_rwlock_t *lock)		__releases(raw_rwlock_t);
void __lockfunc _raw_spin_unlock_bh(raw_spinlock_t *lock)	__releases(raw_spinlock_t);
void __lockfunc _raw_read_unlock_bh(raw_rwlock_t *lock)		__releases(raw_rwlock_t);
void __lockfunc _raw_write_unlock_bh(raw_rwlock_t *lock)	__releases(raw_rwlock_t);
void __lockfunc _raw_spin_unlock_irq(raw_spinlock_t *lock)	__releases(raw_spinlock_t);
void __lockfunc _raw_read_unlock_irq(raw_rwlock_t *lock)	__releases(raw_rwlock_t);
void __lockfunc _raw_write_unlock_irq(raw_rwlock_t *lock)	__releases(raw_rwlock_t);
void __lockfunc _raw_spin_unlock_irqrestore(raw_spinlock_t *lock, unsigned long flags)
							__releases(raw_spinlock_t);
void __lockfunc _raw_read_unlock_irqrestore(raw_rwlock_t *lock, unsigned long flags)
							__releases(raw_rwlock_t);
void __lockfunc _raw_write_unlock_irqrestore(raw_rwlock_t *lock, unsigned long flags)
							__releases(raw_rwlock_t);

#endif /* __LINUX_SPINLOCK_API_SMP_H */
