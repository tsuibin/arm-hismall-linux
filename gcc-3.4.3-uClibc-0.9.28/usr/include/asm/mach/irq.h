/*
 *  linux/include/asm-arm/mach/irq.h
 *
 *  Copyright (C) 1995-2000 Russell King.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_ARM_MACH_IRQ_H
#define __ASM_ARM_MACH_IRQ_H

#include <linux/irq.h>

struct seq_file;

/*
 * This is internal.  Do not use it.
 */
extern void (*init_arch_irq)(void);
extern void init_FIQ(void);
extern int show_fiq_list(struct seq_file *, void *);
void __set_irq_handler(unsigned int irq, struct irq_type *, int);

/*
 * External stuff.
 */
#define set_irq_handler(irq,handler)		__set_irq_handler(irq,handler,0)


#define set_irq_chipdata(irq,d)			set_irq_chip_data(irq, d)
#define get_irq_chipdata(irq)			get_irq_chip_data(irq)

void set_irq_flags(unsigned int irq, unsigned int flags);

#define IRQF_VALID	(1 << 0)
#define IRQF_PROBE	(1 << 1)
#define IRQF_NOAUTOEN	(1 << 2)

/* ARM uses the retrigger functions in desc->chip or software retrigger */
static inline void hw_resend_irq(struct irq_type *t, unsigned int i) {}

/*
 * Hack alert. This is for easy migration, but should be changed in the source
 */
#define do_level_IRQ	(&default_level_type)
#define do_edge_IRQ	(&default_edge_type)
#define do_simple_IRQ	(&default_simple_type)

/* Hack to get around set_irq_chained_handler(nr,NULL) problem */
#define irq_NULL_type no_irq_type
#define set_irq_chained_handler(irq,handler) \
	__set_irq_handler(irq,&irq_##handler##_type,1)

#define DEFINE_IRQ_CHAINED_TYPE(function)		\
struct irq_type irq_##function##_type = {		\
	.typename = #function"-chained_type",		\
	.handle_irq = function,				\
}

#define do_bad_IRQ(irq,desc,regs)			\
do {							\
	spin_lock(&desc->lock);				\
	handle_bad_irq(irq, desc, regs);		\
	spin_unlock(&desc->lock);			\
} while(0)

/* FIXME */
#define ack_bad_irq(irq) do {} while (0)

#endif
