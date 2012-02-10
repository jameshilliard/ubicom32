/*
 * storage.h
 *
 * (C) Copyright 2010
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

#ifndef __STROAGE_H__
#define __STORAGE_H__

struct storage_node {
	struct devtree_node	node;

	unsigned int		version;
	unsigned int		spi_er_nand_switch_port;
	unsigned int		spi_er_nand_switch_pin;
};
#define STORAGE_NODE_VERSION	1

#endif
