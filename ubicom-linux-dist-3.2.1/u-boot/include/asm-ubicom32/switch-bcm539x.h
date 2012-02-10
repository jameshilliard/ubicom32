/*
 * Broadcom bcm539x platform data definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _ASM_UBICOM32_SWITCH_BCM539X_H
#define _ASM_UBICOM32_SWITCH_BCM539X_H

#define SWITCH_BCM539X_FLAG_HW_RESET	0x01
#define SWITCH_BCM539X_FLAG_SW_RESET	0x02

struct switch_bcm539x_platform_data {
	/*
	 * See flags above
	 */
	u32_t		flags;

	/*
	 * GPIO to use for nReset
	 */
	int		pin_reset;


	/*
	 * GPIO to use for chip select
	 */
	int		pin_cs;

	/*
	 * Name of this switch
	 */
	const char	*name;
};

struct bcm539x_data;
struct switch_device;
struct switch_device *bcm539x_probe(struct spi_device *spi);
void bcm539x_remove(struct switch_device *bd);
int bcm539x_handle_enable_read(struct switch_device *dev,
				      char *buf, int inst);
int bcm539x_handle_enable_write(struct switch_device *dev,
				       char *buf, int inst);
int bcm539x_handle_enable_vlan_read(struct switch_device *dev,
					   char *buf, int inst);
int bcm539x_handle_enable_vlan_write(struct switch_device *dev,
					    char *buf, int inst);
int bcm539x_handle_reset(struct switch_device *dev, char *buf, int inst);
int bcm539x_handle_vlan_ports_read(struct switch_device *dev,
					  char *buf, int inst);
int bcm539x_handle_vlan_ports_write(struct switch_device *dev,
					   char *buf, int inst);

#endif /* _ASM_UBICOM32_SWITCH_BCM539X_H */
