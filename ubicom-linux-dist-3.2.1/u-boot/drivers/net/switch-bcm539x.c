/*
 *   BCM539X switch driver, SPI mode
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
#include <malloc.h>
#include <spi.h>
#include <asm/errno.h>
#include <linux/mtd/compat.h>
#define static
//#include <linux/platform_device.h>
//#include <linux/spi/spi.h>
//#include <linux/gpio.h>
//#include <linux/delay.h>

#include <asm/switch-bcm539x.h>
#include "switch-core.h"
#include "switch-bcm539x-reg.h"

#define DRIVER_NAME "bcm539x-spi"
#define DRIVER_VERSION "1.0"

#undef BCM539X_DEBUG
#define BCM539X_SPI_RETRIES	100

struct bcm539x_data {
	struct switch_device			*switch_dev;

	/*
	 * Our private data
	 */
	struct spi_device			*spi;
	struct switch_bcm539x_platform_data	*pdata;

	/*
	 * Last page we accessed
	 */
	u8_t					last_page;

	/*
	 * 539x Device ID
	 */
	u8_t					device_id;
};

/*
 * bcm539x_wait_status
 *	Waits for the specified bit in the status register to be set/cleared.
 */
static int bcm539x_wait_status(struct bcm539x_data *bd, u8_t mask, int set)
{
	u8_t txbuf[2];
	u8_t rxbuf;
	int i;
	int ret;

	txbuf[0] = BCM539X_CMD_READ;
	txbuf[1] = BCM539X_GLOBAL_SPI_STATUS;
	for (i = 0; i < BCM539X_SPI_RETRIES; i++) {
		ret = spi_write_then_read(bd->spi, txbuf, 2, &rxbuf, 1);
		rxbuf &= mask;
		if ((set && rxbuf) || (!set && !rxbuf)) {
			return 0;
		}
		udelay(1);
	}

	return -EIO;
}

/*
 * bcm539x_set_page
 *	Sets the register page for access (only if necessary)
 */
static int bcm539x_set_page(struct bcm539x_data *bd, u8_t page)
{
	u8_t txbuf[3];

	if (page == bd->last_page) {
		return 0;
	}

	bd->last_page = page;

	txbuf[0] = BCM539X_CMD_WRITE;
	txbuf[1] = BCM539X_GLOBAL_PAGE;
	txbuf[2] = page;

	return spi_write(bd->spi, txbuf, 3);
}

/*
 * bcm539x_write_bytes
 *	Writes a number of bytes to a given page and register
 */
static int bcm539x_write_bytes(struct bcm539x_data *bd, u8_t page,
			       u8_t reg, void *buf, u8_t len)
{
	int ret;
	u8_t *txbuf;

	txbuf = kmalloc(2 + len, GFP_KERNEL);
	if (!txbuf) {
		return -ENOMEM;
	}

	/*
	 * Make sure the chip has finished processing our previous request
	 */
	ret = bcm539x_wait_status(bd, BCM539X_GLOBAL_SPI_ST_SPIF, 0);
	if (ret) {
		goto done;
	}

	/*
	 * Set the page
	 */
	ret = bcm539x_set_page(bd, page);
	if (ret) {
		goto done;
	}

	/*
	 * Read the data
	 */
	txbuf[0] = BCM539X_CMD_WRITE;
	txbuf[1] = reg;
	memcpy(&txbuf[2], buf, len);

#ifdef BCM539X_DEBUG
	{
		int i;
		printk("write page %02x reg %02x len=%d buf=", page, reg, len);
		for (i = 0; i < len + 2; i++) {
			printk("%02x ", txbuf[i]);
		}
		printk("\n");
	}
#endif

	ret = spi_write(bd->spi, txbuf, 2 + len);

done:
	kfree(txbuf);
	return ret;
}

/*
 * bcm539x_write_32
 *	Writes 32 bits of data to the given page and register
 */
static inline int bcm539x_write_32(struct bcm539x_data *bd, u8_t page,
				   u8_t reg, u32_t data)
{
	data = cpu_to_le32(data);
	return bcm539x_write_bytes(bd, page, reg, &data, 4);
}

/*
 * bcm539x_write_16
 *	Writes 16 bits of data to the given page and register
 */
static inline int bcm539x_write_16(struct bcm539x_data *bd, u8_t page,
				   u8_t reg, u16_t data)
{
	data = cpu_to_le16(data);
	return bcm539x_write_bytes(bd, page, reg, &data, 2);
}

/*
 * bcm539x_write_8
 *	Writes 8 bits of data to the given page and register
 */
static inline int bcm539x_write_8(struct bcm539x_data *bd, u8_t page,
				  u8_t reg, u8_t data)
{
	return bcm539x_write_bytes(bd, page, reg, &data, 1);
}

/*
 * bcm539x_read_bytes
 *	Reads a number of bytes from a given page and register
 */
static int bcm539x_read_bytes(struct bcm539x_data *bd, u8_t page,
			      u8_t reg, void *buf, u8_t len)
{
	u8_t txbuf[2];
	int ret;

	/*
	 * (1) Make sure the chip has finished processing our previous request
	 */
	ret = bcm539x_wait_status(bd, BCM539X_GLOBAL_SPI_ST_SPIF, 0);
	if (ret) {
		return ret;
	}

	/*
	 * (2) Set the page
	 */
	ret = bcm539x_set_page(bd, page);
	if (ret) {
		return ret;
	}

	/*
	 * (3) Kick off the register read
	 */
	txbuf[0] = BCM539X_CMD_READ;
	txbuf[1] = reg;
	ret = spi_write_then_read(bd->spi, txbuf, 2, txbuf, 1);
	if (ret) {
		return ret;
	}

	/*
	 * (4) Wait for RACK
	 */
	ret = bcm539x_wait_status(bd, BCM539X_GLOBAL_SPI_ST_RACK, 1);
	if (ret) {
		return ret;
	}

	/*
	 * (5) Read the data
	 */
	txbuf[0] = BCM539X_CMD_READ;
	txbuf[1] = BCM539X_GLOBAL_SPI_DATA0;

	ret = spi_write_then_read(bd->spi, txbuf, 2, buf, len);

#ifdef BCM539X_DEBUG
	{
		int i;
		printk("read page %02x reg %02x len=%d rxbuf=",
		       page, reg, len);
		for (i = 0; i < len; i++) {
			printk("%02x ", ((u8_t *)buf)[i]);
		}
		printk("\n");
	}
#endif

	return ret;
}

/*
 * bcm539x_read_32
 *	Reads an 32 bit number from a given page and register
 */
static int bcm539x_read_32(struct bcm539x_data *bd, u8_t page,
			   u8_t reg, u32_t *buf)
{
	int ret = bcm539x_read_bytes(bd, page, reg, buf, 4);
	*buf = le32_to_cpu(*buf);
	return ret;
}

/*
 * bcm539x_read_16
 *	Reads an 16 bit number from a given page and register
 */
static int bcm539x_read_16(struct bcm539x_data *bd, u8_t page,
			   u8_t reg, u16_t *buf)
{
	int ret = bcm539x_read_bytes(bd, page, reg, buf, 2);
	*buf = le16_to_cpu(*buf);
	return ret;
}

/*
 * bcm539x_read_8
 *	Reads an 8 bit number from a given page and register
 */
static int bcm539x_read_8(struct bcm539x_data *bd, u8_t page,
			  u8_t reg, u8_t *buf)
{
	return bcm539x_read_bytes(bd, page, reg, buf, 1);
}

/*
 * bcm539x_set_mode
 */
static int bcm539x_set_mode(struct bcm539x_data *bd, int state)
{
	u8_t buf;
	int ret;

	ret = bcm539x_read_8(bd, PAGE_PORT_TC, REG_CTRL_MODE, &buf);
	if (ret) {
		return ret;
	}

	buf &= ~(1 << 1);
	buf |= state ? (1 << 1) : 0;

	ret = bcm539x_write_8(bd, PAGE_PORT_TC, REG_CTRL_MODE, buf);
	return ret;
}

/*
 * bcm539x_handle_reset
 */
static int bcm539x_handle_reset(struct switch_device *dev, char *buf, int inst)
{
	struct bcm539x_data *bd =
		(struct bcm539x_data *)switch_get_drvdata(dev);
	int ret;

	ret = bcm539x_write_8(bd, PAGE_PORT_TC, REG_CTRL_SRST,
			      (1 << 7) | (1 << 4));
	if (ret) {
		return ret;
	}

	udelay(20);

	ret = bcm539x_write_8(bd, PAGE_PORT_TC, REG_CTRL_SRST, 0);
	return ret;
}

/*
 * bcm539x_handle_vlan_ports_read
 */
static int bcm539x_handle_vlan_ports_read(struct switch_device *dev,
					  char *buf, int inst)
{
	struct bcm539x_data *bd =
		(struct bcm539x_data *)switch_get_drvdata(dev);
	int j;
	int len = 0;
	u8_t rxbuf8;
	u32_t rxbuf32;
	int ret;

	ret = bcm539x_write_16(bd, PAGE_VTBL, REG_VTBL_INDX_5395, inst);
	if (ret) {
		return ret;
	}

	ret = bcm539x_write_8(bd, PAGE_VTBL, REG_VTBL_ACCESS_5395,
			      (1 << 7) | 1);
	if (ret) {
		return ret;
	}

	/*
	 * Wait for completion
	 */
	for (j = 0; j < BCM539X_SPI_RETRIES; j++) {
		ret = bcm539x_read_8(bd, PAGE_VTBL, REG_VTBL_ACCESS_5395,
				     &rxbuf8);
		if (ret) {
			return ret;
		}
		if (!(rxbuf8 & (1 << 7))) {
			break;
		}
	}

	if (j == BCM539X_SPI_RETRIES) {
		return -EIO;
	}

	/*
	 * Read the table entry
	 */
	ret = bcm539x_read_32(bd, PAGE_VTBL, REG_VTBL_ENTRY_5395, &rxbuf32);
	if (ret) {
		return ret;
	}

	for (j = 0; j < 9; j++) {
		if (rxbuf32 & (1 << j)) {
			u16_t rxbuf16;
			len += sprintf(buf + len, "%d", j);
			if (rxbuf32 & (1 << (j + 9))) {
				buf[len++] = 'u';
			} else {
				buf[len++] = 't';
			}
			ret = bcm539x_read_16(bd, PAGE_VLAN,
					      REG_VLAN_PTAG0 + (j << 1),
					      &rxbuf16);
			if (ret) {
				return ret;
			}
			if (rxbuf16 == inst) {
				buf[len++] = '*';
			}
			buf[len++] = '\t';
		}
	}

	len += sprintf(buf + len, "\n");
	buf[len] = '\0';

	return len;
}

/*
 * bcm539x_handle_vlan_ports_write
 */
static int bcm539x_handle_vlan_ports_write(struct switch_device *dev,
					   char *buf, int inst)
{
	struct bcm539x_data *bd =
		(struct bcm539x_data *)switch_get_drvdata(dev);
	int j;
	u32_t untag;
	u32_t ports;
	u32_t def;

	u8_t rxbuf8;
	u16_t rxbuf16;
	int ret;

	switch_parse_vlan_ports(dev, buf, &untag, &ports, &def);

#ifdef BCM539X_DEBUG
	printk(KERN_DEBUG "'%s' inst=%d untag=%08x ports=%08x def=%08x\n",
	       buf, inst, untag, ports, def);
#endif

	if (!ports) {
		return 0;
	}

	/*
	 * Change default vlan tag
	 */
	for (j = 0; j < 9; j++) {
		if ((untag | def) & (1 << j)) {
			ret = bcm539x_write_16(bd, PAGE_VLAN,
					       REG_VLAN_PTAG0 + (j << 1),
					       inst);
			if (ret) {
				return ret;
			}
			continue;
		}

		if (!(dev->port_mask[0] & (1 << j))) {
			continue;
		}

		/*
		 * Remove any ports which are not listed anymore as members of
		 * this vlan
		 */
		ret = bcm539x_read_16(bd, PAGE_VLAN,
				      REG_VLAN_PTAG0 + (j << 1), &rxbuf16);
		if (ret) {
			return ret;
		}
		if (rxbuf16 == inst) {
			ret = bcm539x_write_16(bd, PAGE_VLAN,
					       REG_VLAN_PTAG0 + (j << 1), 0);
			if (ret) {
				return ret;
			}
		}
	}

	/*
	 * Write the VLAN table
	 */
	ret = bcm539x_write_16(bd, PAGE_VTBL, REG_VTBL_INDX_5395, inst);
	if (ret) {
		return ret;
	}

	ret = bcm539x_write_32(bd, PAGE_VTBL, REG_VTBL_ENTRY_5395,
			       (untag << 9) | ports);
	if (ret) {
		return ret;
	}

	ret = bcm539x_write_8(bd, PAGE_VTBL, REG_VTBL_ACCESS_5395,
			      (1 << 7) | 0);
	if (ret) {
		return ret;
	}

	/*
	 * Wait for completion
	 */
	for (j = 0; j < BCM539X_SPI_RETRIES; j++) {
		ret = bcm539x_read_bytes(bd, PAGE_VTBL, REG_VTBL_ACCESS_5395,
					 &rxbuf8, 1);
		if (ret) {
			return ret;
		}
		if (!(rxbuf8 & (1 << 7))) {
			break;
		}
	}

	return (j < BCM539X_SPI_RETRIES) ? 0 : -EIO;
}
#if 0
/*
 * Handlers for <this_driver>/vlan/<vlan_id>
 */
static const struct switch_handler bcm539x_switch_handlers_vlan_dir[] = {
	{
		.name	= "ports",
		.read	= bcm539x_handle_vlan_ports_read,
		.write	= bcm539x_handle_vlan_ports_write,
	},
	{
	},
};

/*
 * bcm539x_handle_vlan_delete_write
 */
static int bcm539x_handle_vlan_delete_write(struct switch_device *dev,
					    char *buf, int inst)
{
	struct bcm539x_data *bd =
		(struct bcm539x_data *)switch_get_drvdata(dev);
	int vid;
	u8_t rxbuf8;
	u32_t txbuf;
	int j;
	int ret;

	vid = simple_strtoul(buf, NULL, 0);
	if (!vid) {
		return -EINVAL;
	}

	/*
	 * Disable this VLAN
	 *
	 * Go through the port-based vlan registers and clear the appropriate
	 * ones out
	 */
	for (j = 0; j < 9; j++) {
		u16_t rxbuf16;
		ret = bcm539x_read_16(bd, PAGE_VLAN, REG_VLAN_PTAG0 + (j << 1),
				      &rxbuf16);
		if (ret) {
			return ret;
		}
		if (rxbuf16 == vid) {
			txbuf = 0;
			ret = bcm539x_write_16(bd, PAGE_VLAN,
					       REG_VLAN_PTAG0 + (j << 1),
					       txbuf);
			if (ret) {
				return ret;
			}
		}
	}

	/*
	 * Write the VLAN table
	 */
	txbuf = vid;
	ret = bcm539x_write_16(bd, PAGE_VTBL, REG_VTBL_INDX_5395, txbuf);
	if (ret) {
		return ret;
	}

	txbuf = 0;
	ret = bcm539x_write_32(bd, PAGE_VTBL, REG_VTBL_ENTRY_5395, txbuf);
	if (ret) {
		return ret;
	}

	txbuf = (1 << 7) | (0);
	ret = bcm539x_write_8(bd, PAGE_VTBL, REG_VTBL_ACCESS_5395, txbuf);
	if (ret) {
		return ret;
	}

	/*
	 * Wait for completion
	 */
	for (j = 0; j < BCM539X_SPI_RETRIES; j++) {
		ret = bcm539x_read_bytes(bd, PAGE_VTBL, REG_VTBL_ACCESS_5395,
					 &rxbuf8, 1);
		if (ret) {
			return ret;
		}
		if (!(rxbuf8 & (1 << 7))) {
			break;
		}
	}

	if (j == BCM539X_SPI_RETRIES) {
		return -EIO;
	}

	return switch_remove_vlan_dir(dev, vid);
}

/*
 * bcm539x_handle_vlan_create_write
 */
static int bcm539x_handle_vlan_create_write(struct switch_device *dev,
					    char *buf, int inst)
{
	int vid;

	vid = simple_strtoul(buf, NULL, 0);
	if (!vid) {
		return -EINVAL;
	}

	return switch_create_vlan_dir(dev, vid,
				      bcm539x_switch_handlers_vlan_dir);
}
#endif
/*
 * bcm539x_handle_enable_read
 */
static int bcm539x_handle_enable_read(struct switch_device *dev,
				      char *buf, int inst)
{
	struct bcm539x_data *bd =
		(struct bcm539x_data *)switch_get_drvdata(dev);
	u8_t rxbuf;
	int ret;

	ret = bcm539x_read_8(bd, PAGE_PORT_TC, REG_CTRL_MODE, &rxbuf);
	if (ret) {
		return ret;
	}
	rxbuf = (rxbuf & (1 << 1)) ? 1 : 0;

	return sprintf(buf, "%d\n", rxbuf);
}

/*
 * bcm539x_handle_enable_write
 */
static int bcm539x_handle_enable_write(struct switch_device *dev,
				       char *buf, int inst)
{
	struct bcm539x_data *bd =
		(struct bcm539x_data *)switch_get_drvdata(dev);

	return bcm539x_set_mode(bd, buf[0] == '1');
}

/*
 * bcm539x_handle_enable_vlan_read
 */
static int bcm539x_handle_enable_vlan_read(struct switch_device *dev,
					   char *buf, int inst)
{
	struct bcm539x_data *bd =
		(struct bcm539x_data *)switch_get_drvdata(dev);
	u8_t rxbuf;
	int ret;

	ret = bcm539x_read_8(bd, PAGE_VLAN, REG_VLAN_CTRL0, &rxbuf);
	if (ret) {
		return ret;
	}
	rxbuf = (rxbuf & (1 << 7)) ? 1 : 0;

	return sprintf(buf, "%d\n", rxbuf);
}

/*
 * bcm539x_handle_enable_vlan_write
 */
static int bcm539x_handle_enable_vlan_write(struct switch_device *dev,
					    char *buf, int inst)
{
	struct bcm539x_data *bd =
		(struct bcm539x_data *)switch_get_drvdata(dev);
	int ret;

	/*
	 * disable 802.1Q VLANs
	 */
	if (buf[0] != '1') {
		ret = bcm539x_write_8(bd, PAGE_VLAN, REG_VLAN_CTRL0, 0);
		return ret;
	}

	/*
	 * enable 802.1Q VLANs
	 *
	 * Enable 802.1Q | IVL learning
	 */
	ret = bcm539x_write_8(bd, PAGE_VLAN, REG_VLAN_CTRL0,
			      (1 << 7) | (3 << 5));
	if (ret) {
		return ret;
	}

	/*
	 * RSV multicast fwd | RSV multicast chk
	 */
	ret = bcm539x_write_8(bd, PAGE_VLAN, REG_VLAN_CTRL1,
			      (1 << 2) | (1 << 3));
	if (ret) {
		return ret;
	}
#if 0
	/*
	 * Drop invalid VID
	 */
	ret = bcm539x_write_16(bd, PAGE_VLAN, REG_VLAN_CTRL3, 0x00FF);
	if (ret) {
		return ret;
	}
#endif
	return 0;
}

/*
 * proc_fs entries for this switch
 */
static const struct switch_handler bcm539x_switch_handlers[] = {
	{
		.name	= "enable",
		.read	= bcm539x_handle_enable_read,
		.write	= bcm539x_handle_enable_write,
	},
	{
		.name	= "enable_vlan",
		.read	= bcm539x_handle_enable_vlan_read,
		.write	= bcm539x_handle_enable_vlan_write,
	},
	{
		.name	= "reset",
		.write	= bcm539x_handle_reset,
	},
	{
	},
};
#if 0
/*
 * Handlers for <this_driver>/vlan
 */
static const struct switch_handler bcm539x_switch_handlers_vlan[] = {
	{
		.name	= "delete",
		.write	= bcm539x_handle_vlan_delete_write,
	},
	{
		.name	= "create",
		.write	= bcm539x_handle_vlan_create_write,
	},
	{
	},
};
#endif
/*
 * bcm539x_probe
 */
static struct switch_device *bcm539x_probe(struct spi_device *spi)
{
	int i, ret;
	struct bcm539x_data *bd;
#if 0
	struct switch_device *switch_dev = NULL;
	struct switch_bcm539x_platform_data *pdata;
	u8_t txbuf[2];

	pdata = spi->dev.platform_data;
	if (!pdata) {
		return -EINVAL;
	}

	ret = spi_setup(spi);
	if (ret < 0) {
		return ret;
	}

	/*
	 * Request our GPIOs (and reset the chip if requested)
	 */
	ret = gpio_request(pdata->pin_cs, "switch-bcm539x-cs");
	if (ret) {
		printk(KERN_WARNING "Could not request cs\n");
		return -EINVAL;
	}
	gpio_direction_output(pdata->pin_cs, 1);

	if (pdata->flags & SWITCH_BCM539X_FLAG_HW_RESET) {
		ret = gpio_request(pdata->pin_reset, "switch-bcm539x-reset");
		if (ret) {
			printk(KERN_WARNING "Could not request reset\n");
			gpio_free(pdata->pin_cs);
			return -EINVAL;
		}

		gpio_direction_output(pdata->pin_reset, 0);
		udelay(10);
		gpio_set_value(pdata->pin_reset, 1);
		udelay(20);
	}

	/*
	 * Allocate our private data structure
	 */
	bd = kzalloc(sizeof(struct bcm539x_data), GFP_KERNEL);
	if (!bd) {
		return -ENOMEM;
	}

	dev_set_drvdata(&spi->dev, bd);
	bd->pdata = pdata;
	bd->spi = spi;
	bd->last_page = 0xFF;

	/*
	 * First perform SW reset if needed
	 */
	if (pdata->flags & SWITCH_BCM539X_FLAG_SW_RESET) {
		txbuf[0] = (1 << 7) | (1 << 4);
		ret = bcm539x_write_bytes(bd, PAGE_PORT_TC,
					  REG_CTRL_SRST, txbuf, 1);
		if (ret) {
			goto fail;
		}

		udelay(20);

		txbuf[0] = 0;
		ret = bcm539x_write_bytes(bd, PAGE_PORT_TC,
					  REG_CTRL_SRST, txbuf, 1);
		if (ret) {
			goto fail;
		}
	}
#else
	bd = kzalloc(sizeof(struct bcm539x_data), GFP_KERNEL);
	if (!bd) {
		//return -ENOMEM;
		return NULL;
	}

	bd->spi = spi;
	bd->last_page = 0xFF;
#endif
	/*
	 * See if we can see the chip
	 */
	for (i = 0; i < 10; i++) {
		ret = bcm539x_read_bytes(bd, PAGE_MMR, REG_DEVICE_ID,
					 &bd->device_id, 1);
		if (!ret) {
			break;
		}
	}
	if (ret) {
		goto fail;
	}

	/*
	 * We only support 5395, 5397, 5398
	 */
	if ((bd->device_id != 0x95) && (bd->device_id != 0x97) &&
	    (bd->device_id != 0x98)) {
		ret = -ENODEV;
		goto fail;
	}
#if 0
	/*
	 * Setup the switch driver structure
	 */
	switch_dev = switch_alloc();
	if (!switch_dev) {
		ret = -ENOMEM;
		goto fail;
	}
	switch_dev->name = pdata->name;

	switch_dev->ports = (bd->device_id == 0x98) ? 9 : 6;
	switch_dev->port_mask[0] = (bd->device_id == 0x98) ? 0x1FF : 0x11F;
	switch_dev->driver_handlers = bcm539x_switch_handlers;
	switch_dev->vlan_handlers = bcm539x_switch_handlers_vlan;

	bd->switch_dev = switch_dev;
	switch_set_drvdata(switch_dev, (void *)bd);

	ret = switch_register(bd->switch_dev);
	if (ret < 0) {
		goto fail;
	}
#endif
	printk(KERN_INFO "bcm53%02x switch chip initialized\n", bd->device_id);
	return (struct switch_device *)bd;
fail:
	printk(KERN_INFO "bcm539x switch chip failure %d\n", ret);
	kfree(bd);
	return NULL;
#if 0
fail:
	if (switch_dev) {
		switch_release(switch_dev);
	}
	dev_set_drvdata(&spi->dev, NULL);
	kfree(bd);
	return ret;
#endif
}
#if 1
static void bcm539x_remove(struct switch_device *bd)
{
	kfree((struct bcm539x_data *)bd);
}
#else
static int __attribute__((unused)) bcm539x_remove(struct spi_device *spi)
{
	struct bcm539x_data *bd;

	bd = dev_get_drvdata(&spi->dev);

	gpio_free(bd->pdata->pin_cs);

	if (bd->pdata->flags & SWITCH_BCM539X_FLAG_HW_RESET) {
		gpio_free(bd->pdata->pin_reset);
	}

	if (bd->switch_dev) {
		switch_unregister(bd->switch_dev);
		switch_release(bd->switch_dev);
	}


	dev_set_drvdata(&spi->dev, NULL);

	kfree(bd);

	return 0;
}

static struct spi_driver bcm539x_driver = {
	.driver = {
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe		= bcm539x_probe,
	.remove		= __devexit_p(bcm539x_remove),
};

static int __init bcm539x_init(void)
{
	return spi_register_driver(&bcm539x_driver);
}

module_init(bcm539x_init);

static void __exit bcm539x_exit(void)
{
	spi_unregister_driver(&bcm539x_driver);
}
module_exit(bcm539x_exit);

MODULE_AUTHOR("Pat Tjin");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("bcm539x SPI switch chip driver");
#endif
