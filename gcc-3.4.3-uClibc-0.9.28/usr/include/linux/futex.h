#ifndef _LINUX_FUTEX_H
#define _LINUX_FUTEX_H

#include <linux/fs.h>

/* Second argument to futex syscall */

#define FUTEX_WAIT		0
#define FUTEX_WAKE		1
#define FUTEX_FD		2
#define FUTEX_REQUEUE		3
#define FUTEX_CMP_REQUEUE	4
#define FUTEX_WAKE_OP		5
#define FUTEX_WAIT_ROBUST	6
#define FUTEX_WAKE_ROBUST	7
#define FUTEX_REGISTER		8
#define FUTEX_DEREGISTER	9
#define FUTEX_RECOVER		10

#define FUTEX_ATTR_PRIORITY_QUEUING		0x10000000
#define FUTEX_ATTR_PRIORITY_INHERITANCE		0x20000000
#define FUTEX_ATTR_PRIORITY_PROTECT		0x40000000
#define FUTEX_ATTR_ROBUST			0x80000000
#define FUTEX_ATTR_SHARED			0x01000000
#define FUTEX_ATTR_MASK				0xff000000

#define FUTEX_WAITERS                         0x80000000
#define FUTEX_OWNER_DIED                      0x40000000
#define FUTEX_NOT_RECOVERABLE                 0x20000000
#define FUTEX_FLAGS (FUTEX_WAITERS | FUTEX_OWNER_DIED | FUTEX_NOT_RECOVERABLE)
#define FUTEX_PID                             ~(FUTEX_FLAGS)

#ifdef __KERNEL__

long do_futex(unsigned long uaddr, int op, int val,
		unsigned long timeout, unsigned long uaddr2, int val2,
		int val3);
#ifdef CONFIG_FUTEX
extern void futex_free_robust_list(struct inode *inode);
extern void exit_futex(struct task_struct *tsk);
#else
# define futex_free_robust_list(a)	do { } while (0)
# define exit_futex(b)			do { } while (0)
#endif
static inline void futex_init_inode(struct inode *inode)
{
	INIT_LIST_HEAD(&inode->i_data.robust_list);
	init_MUTEX(&inode->i_data.robust_sem);
}

#define FUTEX_OP_SET		0	/* *(int *)UADDR2 = OPARG; */
#define FUTEX_OP_ADD		1	/* *(int *)UADDR2 += OPARG; */
#define FUTEX_OP_OR		2	/* *(int *)UADDR2 |= OPARG; */
#define FUTEX_OP_ANDN		3	/* *(int *)UADDR2 &= ~OPARG; */
#define FUTEX_OP_XOR		4	/* *(int *)UADDR2 ^= OPARG; */

#define FUTEX_OP_OPARG_SHIFT	8	/* Use (1 << OPARG) instead of OPARG.  */

#define FUTEX_OP_CMP_EQ		0	/* if (oldval == CMPARG) wake */
#define FUTEX_OP_CMP_NE		1	/* if (oldval != CMPARG) wake */
#define FUTEX_OP_CMP_LT		2	/* if (oldval < CMPARG) wake */
#define FUTEX_OP_CMP_LE		3	/* if (oldval <= CMPARG) wake */
#define FUTEX_OP_CMP_GT		4	/* if (oldval > CMPARG) wake */
#define FUTEX_OP_CMP_GE		5	/* if (oldval >= CMPARG) wake */

/* FUTEX_WAKE_OP will perform atomically
   int oldval = *(int *)UADDR2;
   *(int *)UADDR2 = oldval OP OPARG;
   if (oldval CMP CMPARG)
     wake UADDR2;  */

#define FUTEX_OP(op, oparg, cmp, cmparg) \
  (((op & 0xf) << 28) | ((cmp & 0xf) << 24)		\
   | ((oparg & 0xfff) << 12) | (cmparg & 0xfff))

#endif
#endif
