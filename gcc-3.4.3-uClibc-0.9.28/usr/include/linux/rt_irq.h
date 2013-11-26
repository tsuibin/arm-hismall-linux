#ifndef __LINUX_RT_IRQ_H
#define __LINUX_RT_IRQ_H

/*
 * Soft IRQ flag support on PREEMPT_RT kernels:
 */
#ifdef CONFIG_PREEMPT_RT

extern void local_irq_enable(void);
extern void local_irq_disable(void);
extern void local_irq_restore(unsigned long flags);
extern void __local_save_flags(unsigned long *flags);
extern void __local_irq_save(unsigned long *flags);
extern int irqs_disabled(void);
extern int irqs_disabled_flags(unsigned long flags);

# define local_save_flags(flags)	__local_save_flags(&(flags))
# define local_irq_save(flags)		__local_irq_save(&(flags))

# define RAW_LOCAL_ILLEGAL_MASK		__RAW_LOCAL_ILLEGAL_MASK
# ifdef CONFIG_DEBUG_IRQ_FLAGS
#  define LOCAL_ILLEGAL_MASK		0x40000000UL
   void check_raw_flags(unsigned long flags);
# else
#  define check_raw_flags(flags)	do { } while (0)
# endif

/* soft state does not follow the hard state */
# define raw_local_irq_enable()		do { trace_irqs_on(); __raw_local_irq_enable(); } while (0)
# define raw_local_irq_disable()	do { __raw_local_irq_disable(); trace_irqs_off(); } while (0)
# define raw_local_irq_save(flags)	do { __raw_local_irq_save(flags); trace_irqs_off(); } while (0)
# define raw_local_irq_restore(flags) \
	do { check_raw_flags(flags); if (!__raw_irqs_disabled_flags(flags)) { trace_irqs_on(); } \
			__raw_local_irq_restore(flags); } while (0)
# define raw_safe_halt()		__raw_safe_halt()
#else
# define RAW_LOCAL_ILLEGAL_MASK		0UL
# define LOCAL_ILLEGAL_MASK		0UL
# define raw_local_irq_enable		__raw_local_irq_enable
# define raw_local_irq_disable		__raw_local_irq_disable
# define raw_local_irq_save		__raw_local_irq_save
# define raw_local_irq_restore		__raw_local_irq_restore
# define raw_safe_halt			__raw_safe_halt
# define safe_halt			raw_safe_halt
# define local_save_flags		__raw_local_save_flags
# define local_irq_enable		__raw_local_irq_enable
# define local_irq_disable		__raw_local_irq_disable
# define local_irq_save			__raw_local_irq_save
# define local_irq_restore		__raw_local_irq_restore
# define irqs_disabled			__raw_irqs_disabled
# define irqs_disabled_flags		__raw_irqs_disabled_flags
#endif

#define raw_local_save_flags		__raw_local_save_flags
#define raw_irqs_disabled		__raw_irqs_disabled
#define raw_irqs_disabled_flags		__raw_irqs_disabled_flags

#ifdef CONFIG_CRITICAL_IRQSOFF_TIMING
  extern void notrace trace_irqs_off_lowlevel(void);
  extern void notrace trace_irqs_off(void);
  extern void notrace trace_irqs_on(void);
#else
# define trace_irqs_off_lowlevel()	do { } while (0)
# define trace_irqs_off()		do { } while (0)
# define trace_irqs_on()		do { } while (0)
#endif

#endif /* __LINUX_RT_IRQ_H */
