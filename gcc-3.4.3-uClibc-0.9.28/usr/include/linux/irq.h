#ifndef __irq_h
#define __irq_h

#include <linux/config.h>

#if !defined(CONFIG_ARCH_S390)

#include <linux/linkage.h>
#include <linux/cache.h>
#include <linux/spinlock.h>
#include <linux/cpumask.h>
#include <linux/wait.h>

#include <asm/irq.h>
#include <asm/ptrace.h>
#include <asm/timex.h>

/*
 * IRQ line status.
 */
#define IRQ_INPROGRESS	1	/* IRQ handler active - do not enter! */
#define IRQ_DISABLED	2	/* IRQ disabled - do not enter! */
#define IRQ_PENDING	4	/* IRQ pending - replay on enable */
#define IRQ_REPLAY	8	/* IRQ has been replayed but not acked yet */
#define IRQ_AUTODETECT	16	/* IRQ is being autodetected */
#define IRQ_WAITING	32	/* IRQ not yet seen - for autodetection */
#define IRQ_LEVEL	64	/* IRQ level triggered */
#define IRQ_MASKED	128	/* IRQ masked - shouldn't be seen again */
#if defined(ARCH_HAS_IRQ_PER_CPU)
# define IRQ_PER_CPU	256	/* IRQ is per CPU */
# define CHECK_IRQ_PER_CPU(var) ((var) & IRQ_PER_CPU)
#else
# define CHECK_IRQ_PER_CPU(var) 0
#endif

#define IRQ_NOPROBE	512	/* IRQ is not valid for probing */
#define IRQ_NOREQUEST	1024	/* IRQ cannot be requested */

#define IRQ_NODELAY	2048     /* IRQ must run immediately */

/*
 * Not used on any of the architectures, but feel free to provide
 * your own per-arch one:
 */
#ifndef SA_NODELAY
# define SA_NODELAY 0x01000000
#endif

/*
 * IRQ types
 */
#define IRQ_TYPE_NONE		0x0000		/* Default, unspecified type */
#define IRQ_TYPE_EDGEL		0x0001		/* Edge low/falling type */
#define IRQ_TYPE_EDGEH		0x0002		/* Edge high/rising type */
#define IRQ_TYPE_EDGEB \
	(IRQ_TYPE_EDGEL | IRQ_TYPE_EDGEH)	/* Edge low+high/both type */
#define IRQ_TYPE_LEVELL		0x0004		/* Level low type */
#define IRQ_TYPE_LEVELH		0x0008		/* Level high type */
#define IRQ_TYPE_SIMPLE		0x0010		/* Simple type */


/*
 * IRQ wakeup control modes
 */
#define IRQ_WAKE_NORESUME	0x0000	/* Do not resume on this irq */
#define IRQ_WAKE_RESUME		0x0001	/* Enable resume on this irq */

/**
 * struct irq_chip - Low level interrupt controller hardware descriptor
 *
 * @ack:	acknowledge IRQ
 * @mask:	mask the IRQ
 * @mask_ack:	acknowledge and mask the IRQ
 * @unmask:	unmask the IRQ
 * @retrigger:	retrigger the IRQ in hardware, if possible. Return 0 on success.
 * @set_type:	set the IRQ type (level, edge[high,low,both])
 * @set_wake:	Set the IRQ PM-wakeup function
 * @options:	option field to store type, wake information
 * @lock:	locking for SMP
 * @chip_data:	platform-specific private data for the chip
 */
struct irq_chip {
	spinlock_t	lock;
	void		(*ack)(unsigned int irq);
	void		(*mask)(unsigned int irq);
	void		(*mask_ack)(unsigned int irq);
	void		(*unmask)(unsigned int irq);
	int		(*retrigger)(unsigned int irq);
	int		(*set_type)(unsigned int irq, unsigned int hw_type);
	int		(*set_wake)(unsigned int irq, unsigned int mode);
	unsigned long	options;
	void		*chip_data;
};

struct irq_desc;
struct irq_type;

/**
 * struct irq_type - high level hardware interrupt type descriptor
 *
 * @typename:		name for /proc/interrupts
 * @startup:		start up the interrupt (defaults to ->enable if NULL)
 * @shutdown:		shut down the interrupt (defaults to ->disable if NULL)
 * @enable:		enable the interrupt (defaults to chip->unmask if NULL)
 * @disable:		disable the interrupt (defaults to chip->mask if NULL)
 * @handle_irq:		irq flow handler called from the arch IRQ glue code
 * @ack:		start of new interrupt.	(Note: This will be renamed to 'start')
 * @hold:		same interrupt while the handler is running
 * @end:		end of interrupt
 * @set_affinity:	set the CPU affinity on SMP machines
 * @set_type:		set the interrupt type (level, edge[high,low,both]),
 *			returns a pointer to the irq_type structure which can
 *			handle the requested type or NULL, if the type cannot
 *			be handled.
 */
struct irq_type {
	const char	*typename;
	unsigned int 	(*startup)(unsigned int irq);
	void		(*shutdown)(unsigned int irq);
	void		(*enable)(unsigned int irq);
	void		(*disable)(unsigned int irq);

	void		(*handle_irq)(unsigned int irq, struct irq_desc *desc,
				      struct pt_regs *regs);

			/* (*start) Will be renamed */
	void		(*ack)(unsigned int irq);
	void		(*hold)(unsigned int irq);
	void		(*end)(unsigned int irq);
	void		(*set_affinity)(unsigned int irq, cpumask_t dest);
	struct irq_type *(*set_type)(unsigned int irq, unsigned int type);
	/* Currently used only by UML, might disappear one day.*/
#ifdef CONFIG_IRQ_RELEASE_METHOD
	void (*release)(unsigned int irq, void *dev_id);
#endif
};

/**
 * struct irq_desc - interrupt descriptor
 *
 * @handler:		interrupt type dependent handler functions,
 * 			(this should be renamed to 'type')
 * @handler_data:	data for the type handlers
 * @chip:		low level hardware access functions - comes from type
 * @action:		the irq action chain
 * @status:		status information
 * @depth:		disable-depth, for nested irq_disable() calls
 * @irq_count:		stats field to detect stalled irqs
 * @irqs_unhandled:	stats field for spurious unhandled interrupts
 * @thread:		Thread pointer for threaded preemptible irq handling
 * @wait_for_handler:	Waitqueue to wait for a running preemptible handler
 * @lock:		locking for SMP
 * @move_irq:		Flag need to re-target interrupt destination
 *
 * Pad this out to 32 bytes for cache and indexing reasons.
 */
typedef struct irq_desc {
	struct irq_type		*handler;
	void			*handler_data;
	struct irq_chip		*chip;
	struct irqaction	*action;
	unsigned int		status;
				
	unsigned int		depth;
	unsigned int		irq_count;
	unsigned int		irqs_unhandled;
 	struct task_struct	*thread;
 	wait_queue_head_t	wait_for_handler;
	raw_spinlock_t		lock;
#if defined (CONFIG_GENERIC_PENDING_IRQ) || defined (CONFIG_IRQBALANCE)
	unsigned int		move_irq;
#endif
} ____cacheline_aligned irq_desc_t;


/*
 * Migration helpers for obsolete names, they will go away:
 */
#define irqdesc			irq_desc
#define irqchip			irq_chip
#define hw_interrupt_type	irq_type
#define set_irq_type		set_hwirq_type
typedef struct irq_type hw_irq_controller;

extern irq_desc_t irq_desc [NR_IRQS];

/* Return a pointer to the irq descriptor for IRQ.  */
static inline irq_desc_t *
irq_descp (int irq)
{
	return irq_desc + irq;
}

#include <asm/hw_irq.h> /* the arch dependent stuff */

extern int setup_irq(unsigned int irq, struct irqaction * new);

#ifdef CONFIG_GENERIC_HARDIRQS
extern cpumask_t irq_affinity[NR_IRQS];

#ifdef CONFIG_SMP
static inline void set_native_irq_info(int irq, cpumask_t mask)
{
	irq_affinity[irq] = mask;
}
#else
static inline void set_native_irq_info(int irq, cpumask_t mask)
{
}
#endif

#ifdef CONFIG_SMP

#if defined (CONFIG_GENERIC_PENDING_IRQ) || defined (CONFIG_IRQBALANCE)
extern cpumask_t pending_irq_cpumask[NR_IRQS];

static inline void set_pending_irq(unsigned int irq, cpumask_t mask)
{
	irq_desc_t *desc = irq_desc + irq;
	unsigned long flags;

	spin_lock_irqsave(&desc->lock, flags);
	desc->move_irq = 1;
	pending_irq_cpumask[irq] = mask;
	spin_unlock_irqrestore(&desc->lock, flags);
}

static inline void
move_native_irq(int irq)
{
	cpumask_t tmp;
	irq_desc_t *desc = irq_descp(irq);

	if (likely (!desc->move_irq))
		return;

	desc->move_irq = 0;

	if (likely(cpus_empty(pending_irq_cpumask[irq])))
		return;

	if (!desc->handler->set_affinity)
		return;

	/* note - we hold the desc->lock */
	cpus_and(tmp, pending_irq_cpumask[irq], cpu_online_map);

	/*
	 * If there was a valid mask to work with, please
	 * do the disable, re-program, enable sequence.
	 * This is *not* particularly important for level triggered
	 * but in a edge trigger case, we might be setting rte
	 * when an active trigger is comming in. This could
	 * cause some ioapics to mal-function.
	 * Being paranoid i guess!
	 */
	if (unlikely(!cpus_empty(tmp))) {
		desc->handler->disable(irq);
		desc->handler->set_affinity(irq,tmp);
		desc->handler->enable(irq);
	}
	cpus_clear(pending_irq_cpumask[irq]);
}

#ifdef CONFIG_PCI_MSI
/*
 * Wonder why these are dummies?
 * For e.g the set_ioapic_affinity_vector() calls the set_ioapic_affinity_irq()
 * counter part after translating the vector to irq info. We need to perform
 * this operation on the real irq, when we dont use vector, i.e when
 * pci_use_vector() is false.
 */
static inline void move_irq(int irq)
{
}

static inline void set_irq_info(int irq, cpumask_t mask)
{
}

#else // CONFIG_PCI_MSI

static inline void move_irq(int irq)
{
	move_native_irq(irq);
}

static inline void set_irq_info(int irq, cpumask_t mask)
{
	set_native_irq_info(irq, mask);
}
#endif // CONFIG_PCI_MSI

#else	// CONFIG_GENERIC_PENDING_IRQ || CONFIG_IRQBALANCE

#define move_irq(x)
#define move_native_irq(x)
#define set_pending_irq(x,y)
static inline void set_irq_info(int irq, cpumask_t mask)
{
	set_native_irq_info(irq, mask);
}

#endif // CONFIG_GENERIC_PENDING_IRQ

#else // CONFIG_SMP

#define move_irq(x)
#define move_native_irq(x)

#endif // CONFIG_SMP

extern int no_irq_affinity;

/* Handle irq action chains */
extern fastcall int handle_IRQ_event(unsigned int irq, struct pt_regs *regs,
				       struct irqaction *action);

/*
 * Built-in IRQ handlers for various IRQ types,
 * callable via desc->handler->handle_irq()
 */
extern void handle_level_irq(unsigned int irq, struct irq_desc *desc, struct pt_regs *regs);
extern void handle_edge_irq(unsigned int irq, struct irq_desc *desc, struct pt_regs *regs);
extern void handle_simple_irq(unsigned int irq, struct irq_desc *desc,  struct pt_regs *regs);
extern void handle_percpu_irq(unsigned int irq, struct irq_desc *desc, struct pt_regs *regs);
extern void handle_bad_irq(unsigned int irq, struct irq_desc *desc, struct pt_regs *regs);

#define desc_handle_irq(irq, desc, regs)		\
do {							\
	spin_lock(&(desc)->lock);			\
	(desc)->handler->handle_irq(irq, (desc), regs);	\
	spin_unlock(&(desc)->lock);			\
} while(0)

/* Monolithic do_IRQ implementation */
extern fastcall unsigned int __do_IRQ(unsigned int irq, struct pt_regs *regs);

/* Handling of unhandled and spurious interrupts */
extern void note_interrupt(unsigned int irq, irq_desc_t *desc,
					int action_ret, struct pt_regs *regs);

/* Resending of interrupts */
void check_irq_resend(irq_desc_t *desc, unsigned int irq);

/* Proc filesystem */
extern void init_irq_proc(void);

/* Enable/disable irq debugging output */
extern int noirqdebug_setup(char *str);

/* Set/get irq type */
extern int set_irq_type(unsigned int irq, unsigned int type);
extern int get_irq_type(unsigned int irq, unsigned int type);

/* Irq wakeup (PM) control) */
extern int set_irq_wake(unsigned int irq, unsigned int mode);
#define enable_irq_wake(irq) set_irq_wake(irq, IRQ_WAKE_RESUME)
#define disable_irq_wake(irq) set_irq_wake(irq, IRQ_WAKE_NORESUME)

/* Checks whether the interrupt can be requested by request_irq() */
extern int can_request_irq(unsigned int irq, unsigned long irqflags);

/* Set type control/chip/data for an interrupt */
extern int generic_set_irq_type(unsigned int irq, struct irq_type *type);
extern int set_irq_data(unsigned int irq, void *data);
extern int set_irq_chip(unsigned int irq, struct irq_chip *chip);
extern int set_irq_chip_data(unsigned int irq, void *data);

/* Get chip/data for an interrupt */
#define get_irq_chip(irq) (irq_desc[irq].chip)
#define get_irq_chip_data(irq) (irq_desc[irq].chip->chip_data)

/* Interrupt type default implementations */
extern struct irq_type no_irq_type;
extern struct irq_type default_edge_type;
extern struct irq_type default_level_type;
extern struct irq_type default_simple_type;
extern struct irq_type default_percpu_type;

/* Early initialization of irqs */
extern void early_init_hardirqs(void);

#if defined(CONFIG_PREEMPT_HARDIRQS)
extern void init_hardirqs(void);
#else
static inline void init_hardirqs(void) { }
#endif

#else	/* GENERIC HARDIRQS */

static inline void early_init_hardirqs(void) { }
static inline void init_hardirqs(void) { }

#endif

extern hw_irq_controller no_irq_type;  /* needed in every arch ? */

#endif

#endif /* __irq_h */
