/*
 *   Ubicom32 family device tree support
 *
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <asm/devtree.h>

/*
 * The device tree.
 */
struct devtree_node *devtree;

/*
 * devtree_print()
 * 	Print the device tree.
 */
void devtree_print(void)
{
	struct devtree_node *p = devtree;
	printf("Device Tree:\n");
	while (p) {
		if (p->magic != DEVTREE_NODE_MAGIC) {
			printf("device tree has improper node: %p\n", p);
			return;
		}
		printf("\t%p: sendirq=%03d, recvirq=%03d, name=%s\n", p, p->sendirq, p->recvirq, p->name);
		p = p->next;
	}
}

/*
 * devtree_irq()
 * 	Return the IRQ(s) associated with devtree node.
 */
int devtree_irq(struct devtree_node *dn, unsigned char *sendirq, unsigned char *recvirq)
{
	if (dn->magic != DEVTREE_NODE_MAGIC) {
		printf("improper node: %p\n", dn);
		if (sendirq) {
			*sendirq = DEVTREE_IRQ_NONE;
		}
		if (recvirq) {
			*recvirq = DEVTREE_IRQ_NONE;
		}
		return -1;
	}

	/*
	 * Copy the devtree irq(s) to the output parameters.
	 */
	if (sendirq) {
		*sendirq = dn->sendirq;
	}
	if (recvirq) {
		*recvirq = dn->recvirq;
	}
	return 0;
}

/*
 * devtree_find_node()
 * 	Find a node in the device tree by name.
 */
struct devtree_node *devtree_find_node(const char *str)
{
	struct devtree_node *p = devtree;
	while (p) {
		if (p->magic != DEVTREE_NODE_MAGIC) {
			printf("device tree has improper node: %p\n", p);
			return NULL;
		}
		if (strcmp(p->name, str) == 0) {
			return p;
		}
		p = p->next;
	}
	return NULL;
}
