#ifndef _ASM_GENERIC_BUG_H
#define _ASM_GENERIC_BUG_H

#include <linux/compiler.h>
#include <linux/config.h>

#ifdef CONFIG_BUG
#ifndef HAVE_ARCH_BUG
#define BUG() do { \
	printk("kernel BUG at %s:%d!\n", __FILE__, __LINE__); \
	panic("BUG!"); \
} while (0)
#endif

#ifndef HAVE_ARCH_BUG_ON
#define BUG_ON(condition) do { if (unlikely((condition)!=0)) BUG(); } while(0)
#endif

extern void __WARN_ON(const char *func, const char *file, const int line);

#ifndef HAVE_ARCH_WARN_ON
#define WARN_ON(condition) do { \
	if (unlikely((condition)!=0)) \
		__WARN_ON(__FUNCTION__, __FILE__, __LINE__); \
} while (0)
#endif

#else /* !CONFIG_BUG */
#ifndef HAVE_ARCH_BUG
#define BUG()
#endif

#ifndef HAVE_ARCH_BUG_ON
#define BUG_ON(condition) do { if (condition) ; } while(0)
#endif

#ifndef HAVE_ARCH_WARN_ON
#define WARN_ON(condition) do { if (condition) ; } while(0)
#endif
#endif

#ifdef CONFIG_PREEMPT_RT
# define BUG_ON_RT(c)			BUG_ON(c)
# define BUG_ON_NONRT(c)		do { } while (0)
# define WARN_ON_RT(condition)		WARN_ON(condition)
# define WARN_ON_NONRT(condition)	do { } while (0)
#else
# define BUG_ON_RT(c)			do { } while (0)
# define BUG_ON_NONRT(c)		BUG_ON(c)
# define WARN_ON_RT(condition)		do { } while (0)
# define WARN_ON_NONRT(condition)	WARN_ON(condition)
#endif

#endif
