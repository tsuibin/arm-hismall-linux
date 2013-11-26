/*
 * Descending-priority-sorted double-linked list
 *
 * (C) 2002-2003 Intel Corp
 * Inaky Perez-Gonzalez <inaky.perez-gonzalez@intel.com>.
 *
 * 2001-2005 (c) MontaVista Software, Inc.
 * Daniel Walker <dwalker@mvista.com>
 *
 * (C) 2005 Thomas Gleixner <tglx@linutronix.de>
 * Tested and made it functional.
 *
 * Licensed under the FSF's GNU Public License v2 or later.
 *
 * Based on simple lists (include/linux/list.h).
 *
 *
 * This is a priority-sorted list of nodes; each node has a >= 0
 * priority from 0 (highest) to INT_MAX (lowest). The list itself has
 * a priority too (the highest of all the nodes), stored in the head
 * of the list (that is a node itself).
 *
 * Addition is O(K), removal is O(1), change of priority of a node is
 * O(K) and K is the number of RT priority levels used in the system.
 * (1 <= K <= 99)
 *
 * This list is really a list of lists:
 *
 *  - The tier 1 list is the dp list (Different Priority)
 *
 *  - The tier 2 list is the sp list (Serialized Priority)
 *
 * Simple ASCII art explanation:
 *
 * |HEAD   |
 * |       |
 * |dp.prev|<------------------------------------|
 * |dp.next|<->|dp|<->|dp|<--------------->|dp|<-|
 * |10     |   |10|   |21|   |21|   |21|   |40|   (prio)
 * |       |   |  |   |  |   |  |   |  |   |  |
 * |       |   |  |   |  |   |  |   |  |   |  |
 * |sp.next|<->|sp|<->|sp|<->|sp|<->|sp|<->|sp|<-|
 * |sp.prev|<------------------------------------|
 *
 * The nodes on the dp list are sorted by priority to simplify
 * the insertion of new nodes. There are no nodes with duplicate
 * priorites on the list.
 *
 * The nodes on the sp list are ordered by priority and can contain
 * entries which have the same priority. Those entries are ordered
 * FIFO
 *
 * Addition means: look for the dp node in the dp list for the
 * priority of the node and insert it before the sp entry of the next
 * dp node. If it is the first node of that priority, add it to the
 * dp list in the right position and insert it into the serialized
 * sp list
 *
 * Removal means remove it from the sp list and remove it from the dp
 * list if the dp list_head is non empty. In case of removal from the
 * dp list it must be checked whether other entries of the same
 * priority are on the list or not. If there is another entry of
 * the same priority then this entry has to replace the
 * removed entry on the dp list. If the entry which is removed is
 * the only entry of this priority then a simple remove from both
 * list is sufficient.
 *
 * INT_MIN is the highest priority, 0 is the medium highest, INT_MAX
 * is lowest priority.
 *
 * No locking is done, up to the caller.
 *
 * NOTE: This implementation does not offer as many interfaces as
 *       linux/list.h does -- it is lazily minimal. You are welcome to
 *       add them.
 */

#ifndef _LINUX_PLIST_H_
#define _LINUX_PLIST_H_

#include <linux/kernel.h>
#include <linux/list.h>

/* Priority-sorted list */
struct plist {
	int prio;
	struct list_head dp_node;
	struct list_head sp_node;
};

#define PLIST_INIT(p,__prio)				\
{							\
	.prio = __prio,					\
	.dp_node = LIST_HEAD_INIT((p).dp_node),	\
	.sp_node = LIST_HEAD_INIT((p).sp_node),	\
}

/**
 * plist_entry - get the struct for this entry
 * @ptr:        the &struct plist pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 */
#define plist_entry(ptr, type, member) \
        container_of(ptr, type, member)

/**
 * plist_first_entry - get the struct for the first entry
 * @ptr:        the &struct plist pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 */
#define plist_first_entry(ptr, type, member) \
        container_of(plist_first(ptr), type, member)

/**
 * plist_for_each  -       iterate over the plist
 * @pos1:        the type * to use as a loop counter.
 * @head:       the head for your list.
 */
#define plist_for_each(pos1, head)	\
	list_for_each_entry(pos1, &((head)->sp_node), sp_node)
/**
 * plist_for_each_entry_safe - iterate over a plist of given type safe against removal of list entry
 * @pos1:        the type * to use as a loop counter.
 * @n1:          another type * to use as temporary storage
 * @head:       the head for your list.
 */
#define plist_for_each_safe(pos1, n1, head)			\
	list_for_each_entry_safe(pos1, n1, &((head)->sp_node), sp_node)

/* Initialize a pl */
extern void plist_init(struct plist *pl, int prio);

/* Return the first node (and thus, highest priority)
 *
 * Assumes the plist is _not_ empty.
 */
static inline
struct plist * plist_first(struct plist *plist)
{
	return list_entry(plist->dp_node.next, struct plist, dp_node);
}

/* Return if the plist is empty. */
static inline
unsigned plist_empty(struct plist *plist)
{
	return list_empty(&plist->sp_node);
}

/* Update the maximum priority of the whole list
 *
 * @returns !0 if the plist prio changed, 0 otherwise.
 */
extern unsigned plist_update_prio(struct plist *plist);

/**
 * Add node @pl to @plist @returns !0 if the plist prio changed, 0
 * otherwise.
 */
extern unsigned plist_add(struct plist *pl, struct plist *plist);

/**
 * Remove a node @pl from @plist. @returns !0 if the plist prio
 * changed, 0 otherwise.
 */
extern unsigned plist_del(struct plist *pl, struct plist *plist);

/**
 * plist_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
extern void plist_del_init(struct plist *pl, struct plist *plist);

/* Return the priority a pl node */
static inline int plist_prio(struct plist *pl)
{
	return pl->prio;
}

/**
 * Change the priority of node @pl in @plist (updating the list's max
 * priority).  @returns !0 if the plist's maximum priority changes
 */
extern unsigned plist_chprio(struct plist *plist, struct plist *pl, int new_prio);

#endif /* #ifndef _LINUX_PLIST_H_ */

