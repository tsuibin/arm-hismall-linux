#ifndef __ASM_ARM_IRQ_H
#define __ASM_ARM_IRQ_H

#include <asm/arch/irqs.h>

#ifndef irq_canonicalize
#define irq_canonicalize(i)	(i)
#endif

#ifndef NR_IRQS
#define NR_IRQS	128
#endif

/*
 * Use this value to indicate lack of interrupt
 * capability
 */
#ifndef NO_IRQ
#define NO_IRQ	((unsigned int)(-1))
#endif

#define __IRQT_FALEDGE	IRQ_TYPE_EDGEL
#define __IRQT_RISEDGE	IRQ_TYPE_EDGEH
#define __IRQT_LOWLVL	IRQ_TYPE_LEVELL
#define __IRQT_HIGHLVL	IRQ_TYPE_LEVELH

#define IRQT_NOEDGE	(0)
#define IRQT_RISING	(__IRQT_RISEDGE)
#define IRQT_FALLING	(__IRQT_FALEDGE)
#define IRQT_BOTHEDGE	(__IRQT_RISEDGE|__IRQT_FALEDGE)
#define IRQT_LOW	(__IRQT_LOWLVL)
#define IRQT_HIGH	(__IRQT_HIGHLVL)

/* FIXME_TGLX */
#define IRQT_PROBE	(1 << 7)

#endif

