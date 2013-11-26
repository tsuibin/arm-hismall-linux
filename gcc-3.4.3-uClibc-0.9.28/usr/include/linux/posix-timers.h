#ifndef _linux_POSIX_TIMERS_H
#define _linux_POSIX_TIMERS_H

#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/sched.h>

union cpu_time_count {
	cputime_t cpu;
	unsigned long long sched;
};

struct cpu_timer_list {
	struct list_head entry;
	union cpu_time_count expires, incr;
	struct task_struct *task;
	int firing;
};

#define CPUCLOCK_PID(clock)		((pid_t) ~((clock) >> 3))
#define CPUCLOCK_PERTHREAD(clock) \
	(((clock) & (clockid_t) CPUCLOCK_PERTHREAD_MASK) != 0)
#define CPUCLOCK_PID_MASK	7
#define CPUCLOCK_PERTHREAD_MASK	4
#define CPUCLOCK_WHICH(clock)	((clock) & (clockid_t) CPUCLOCK_CLOCK_MASK)
#define CPUCLOCK_CLOCK_MASK	3
#define CPUCLOCK_PROF		0
#define CPUCLOCK_VIRT		1
#define CPUCLOCK_SCHED		2
#define CPUCLOCK_MAX		3

#define MAKE_PROCESS_CPUCLOCK(pid, clock) \
	((~(clockid_t) (pid) << 3) | (clockid_t) (clock))
#define MAKE_THREAD_CPUCLOCK(tid, clock) \
	MAKE_PROCESS_CPUCLOCK((tid), (clock) | CPUCLOCK_PERTHREAD_MASK)

/* POSIX.1b interval timer structure. */
struct k_itimer {
	struct list_head list;		/* free/ allocate list */
	spinlock_t it_lock;
	clockid_t it_clock;		/* which timer type */
	timer_t it_id;			/* timer id */
	int it_overrun;			/* overrun on pending signal  */
	int it_overrun_last;		/* overrun on last delivered signal */
	int it_requeue_pending;         /* waiting to requeue this timer */
#define REQUEUE_PENDING 1
	int it_sigev_notify;		/* notify word of sigevent struct */
	int it_sigev_signo;		/* signo word of sigevent struct */
	sigval_t it_sigev_value;	/* value word of sigevent struct */
	struct task_struct *it_process;	/* process to send signal to */
	struct sigqueue *sigq;		/* signal queue entry. */
	union {
		struct {
			struct ktimer timer;
			ktime_t incr;
			int overrun;
		} real;
		struct cpu_timer_list cpu;
		struct {
			unsigned int clock;
			unsigned int node;
			unsigned long incr;
			unsigned long expires;
		} mmtimer;
	} it;
};

struct k_clock {
	int res;		/* in nano seconds */
	int (*clock_getres) (clockid_t which_clock, struct timespec *tp);
	struct k_clock_abs *abs_struct;
	int (*clock_set) (clockid_t which_clock, struct timespec * tp);
	int (*clock_get) (clockid_t which_clock, struct timespec * tp);
	int (*timer_create) (struct k_itimer *timer);
	int (*nsleep) (clockid_t which_clock, int flags, struct timespec *, struct timespec __user *);
	int (*timer_set) (struct k_itimer * timr, int flags,
			  struct itimerspec * new_setting,
			  struct itimerspec * old_setting);
	int (*timer_del) (struct k_itimer * timr);
#define TIMER_RETRY 1
	void (*timer_get) (struct k_itimer * timr,
			   struct itimerspec * cur_setting);
};

void register_posix_clock(clockid_t clock_id, struct k_clock *new_clock);

/* Error handlers for timer_create, nanosleep and settime */
int do_posix_clock_notimer_create(struct k_itimer *timer);
int do_posix_clock_nonanosleep(clockid_t, int flags, struct timespec *, struct timespec __user *);
int do_posix_clock_nosettime(clockid_t, struct timespec *tp);

/* function to call to trigger timer event */
int posix_timer_event(struct k_itimer *timr, int si_private);

#if (BITS_PER_LONG < 64)
static inline ktime_t forward_posix_timer(struct k_itimer *t, ktime_t now)
{
	ktime_t delta = ktime_sub(now, t->it.real.timer.expires);
	unsigned long orun = 1;

	if (ktime_cmp_val(delta, <, KTIME_ZERO))
		goto out;

	if (unlikely(ktime_cmp(delta, >, t->it.real.incr))) {

		int sft = 0;
		u64 div, dclc, inc, dns;

		dclc = dns = ktime_to_ns(delta);
		div = inc = ktime_to_ns(t->it.real.incr);
		/* Make sure the divisor is less than 2^32 */
		while(div >> 32) {
			sft++;
			div >>= 1;
		}
		dclc >>= sft;
		do_div(dclc, (unsigned long) div);
		orun = (unsigned long) dclc;
		if (likely(!(inc >> 32)))
			dclc *= (unsigned long) inc;
		else
			dclc *= inc;
		t->it.real.timer.expires = ktime_add_ns(t->it.real.timer.expires,
							dclc);
	} else {
		t->it.real.timer.expires = ktime_add(t->it.real.timer.expires,
						     t->it.real.incr);
	}
	/*
	 * Here is the correction for exact.  Also covers delta == incr
	 * which is the else clause above.
	 */
	if (ktime_cmp(t->it.real.timer.expires, <=, now)) {
		t->it.real.timer.expires = ktime_add(t->it.real.timer.expires,
						     t->it.real.incr);
		orun++;
	}
	t->it_overrun += orun;

 out:
	return ktime_sub(t->it.real.timer.expires, now);
}
#else
static inline ktime_t forward_posix_timer(struct k_itimer *t, ktime_t now)
{
	ktime_t delta = ktime_sub(now, t->it.real.timer.expires);
	unsigned long orun = 1;

	if (ktime_cmp_val(delta, <, KTIME_ZERO))
		goto out;

	if (unlikely(ktime_cmp(delta, >, t->it.real.incr))) {

		u64 dns, inc;

		dns = ktime_to_ns(delta);
		inc = ktime_to_ns(t->it.real.incr);

		orun = dns / inc;
		t->it.real.timer.expires = ktime_add_ns(t->it.real.timer.expires,
							orun * inc);
	} else {
		t->it.real.timer.expires = ktime_add(t->it.real.timer.expires,
						     t->it.real.incr);
	}
	/*
	 * Here is the correction for exact.  Also covers delta == incr
	 * which is the else clause above.
	 */
	if (ktime_cmp(t->it.real.timer.expires, <=, now)) {
		t->it.real.timer.expires = ktime_add(t->it.real.timer.expires,
						     t->it.real.incr);
		orun++;
	}
	t->it_overrun += orun;
 out:
	return ktime_sub(t->it.real.timer.expires, now);
}
#endif

int posix_cpu_clock_getres(clockid_t which_clock, struct timespec *);
int posix_cpu_clock_get(clockid_t which_clock, struct timespec *);
int posix_cpu_clock_set(clockid_t which_clock, const struct timespec *tp);
int posix_cpu_timer_create(struct k_itimer *);
int posix_cpu_nsleep(clockid_t, int, struct timespec *,
		     struct timespec __user *);
int posix_cpu_timer_set(struct k_itimer *, int,
			struct itimerspec *, struct itimerspec *);
int posix_cpu_timer_del(struct k_itimer *);
void posix_cpu_timer_get(struct k_itimer *, struct itimerspec *);

void posix_cpu_timer_schedule(struct k_itimer *);

void run_posix_cpu_timers(struct task_struct *);
void posix_cpu_timers_exit(struct task_struct *);
void posix_cpu_timers_exit_group(struct task_struct *);

void set_process_cpu_timer(struct task_struct *, unsigned int,
			   cputime_t *, cputime_t *);

#endif
