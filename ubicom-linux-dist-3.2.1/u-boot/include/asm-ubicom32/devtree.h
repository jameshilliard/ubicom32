/*
 * Device Tree Header File
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

#ifndef __UBICOM32_DEVTREE_H__
#define __UBICOM32_DEVTREE_H__

#define DEVTREE_MAX_NAME 32
#define DEVTREE_IRQ_NONE 0xff
#define DEVTREE_NODE_MAGIC 0x10203040

struct devtree_node {
	struct devtree_node *next;
	unsigned char sendirq;
	unsigned char recvirq;
	char name[DEVTREE_MAX_NAME];
	unsigned int magic;
};

extern struct devtree_node *devtree;
extern struct devtree_node *devtree_find_node(const char *str);
extern int devtree_irq(struct devtree_node *dn, unsigned char *sendirq, unsigned char *recvirq);
extern void devtree_print(void);

#define BOARDNODE_VERSION 4
struct boardnode {
	struct devtree_node dn;
	const char *revision;
	const char *identity_string;
	int version;
};

struct bootargsnode {
	struct devtree_node dn;
	char cmdline[512];
};

#endif /* __UBICOM32_DEVTREE_H__ */
