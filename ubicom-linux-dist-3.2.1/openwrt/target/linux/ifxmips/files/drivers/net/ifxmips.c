/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2005 Wu Qi Ming <Qi-Ming.Wu@infineon.com>
 *   Copyright (C) 2008 John Crispin <blogic@openwrt.org>
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/phy.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>
#include <linux/init.h>
#include <linux/delay.h>

#include <asm/checksum.h>

#include <ifxmips.h>
#include <ifxmips_dma.h>
#include <ifxmips_pmu.h>
#include <ifxmips_platform.h>

struct ifxmips_mii_priv {
	struct net_device_stats stats;
	struct dma_device_info *dma_device;
	struct sk_buff *skb;

	struct mii_bus *mii_bus;
	struct phy_device *phydev;
	int oldlink, oldspeed, oldduplex;
};

static struct net_device *ifxmips_mii0_dev;
static unsigned char mac_addr[MAX_ADDR_LEN];

static int ifxmips_mdiobus_write(struct mii_bus *bus, int phy_addr,
				int phy_reg, u16 phy_data)
{
	u32 val = MDIO_ACC_REQUEST |
		((phy_addr & MDIO_ACC_ADDR_MASK) << MDIO_ACC_ADDR_OFFSET) |
		((phy_reg & MDIO_ACC_REG_MASK) << MDIO_ACC_REG_OFFSET) |
		phy_data;

	while (ifxmips_r32(IFXMIPS_PPE32_MDIO_ACC) & MDIO_ACC_REQUEST)
		;
	ifxmips_w32(val, IFXMIPS_PPE32_MDIO_ACC);

	return 0;
}

static int ifxmips_mdiobus_read(struct mii_bus *bus, int phy_addr, int phy_reg)
{
	u32 val = MDIO_ACC_REQUEST | MDIO_ACC_READ |
		((phy_addr & MDIO_ACC_ADDR_MASK) << MDIO_ACC_ADDR_OFFSET) |
		((phy_reg & MDIO_ACC_REG_MASK) << MDIO_ACC_REG_OFFSET);

	while (ifxmips_r32(IFXMIPS_PPE32_MDIO_ACC) & MDIO_ACC_REQUEST)
		;
	ifxmips_w32(val, IFXMIPS_PPE32_MDIO_ACC);
	while (ifxmips_r32(IFXMIPS_PPE32_MDIO_ACC) & MDIO_ACC_REQUEST)
		;
	val = ifxmips_r32(IFXMIPS_PPE32_MDIO_ACC) & MDIO_ACC_VAL_MASK;
	return val;
}

int ifxmips_ifxmips_mii_open(struct net_device *dev)
{
	struct ifxmips_mii_priv *priv = (struct ifxmips_mii_priv *)netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;
	int i;

	for (i = 0; i < dma_dev->max_rx_chan_num; i++) {
		if ((dma_dev->rx_chan[i])->control == IFXMIPS_DMA_CH_ON)
			(dma_dev->rx_chan[i])->open(dma_dev->rx_chan[i]);
	}
	netif_start_queue(dev);
	return 0;
}

int ifxmips_mii_release(struct net_device *dev)
{
	struct ifxmips_mii_priv *priv = (struct ifxmips_mii_priv *)netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;
	int i;

	for (i = 0; i < dma_dev->max_rx_chan_num; i++)
		dma_dev->rx_chan[i]->close(dma_dev->rx_chan[i]);
	netif_stop_queue(dev);
	return 0;
}

int ifxmips_mii_hw_receive(struct net_device *dev, struct dma_device_info *dma_dev)
{
	struct ifxmips_mii_priv *priv = (struct ifxmips_mii_priv *)netdev_priv(dev);
	unsigned char *buf = NULL;
	struct sk_buff *skb = NULL;
	int len = 0;

	len = dma_device_read(dma_dev, &buf, (void **)&skb);

	if (len >= ETHERNET_PACKET_DMA_BUFFER_SIZE) {
		printk(KERN_INFO "ifxmips_mii0: packet too large %d\n", len);
		goto ifxmips_mii_hw_receive_err_exit;
	}

	/* remove CRC */
	len -= 4;
	if (skb == NULL) {
		printk(KERN_INFO "ifxmips_mii0: cannot restore pointer\n");
		goto ifxmips_mii_hw_receive_err_exit;
	}

	if (len > (skb->end - skb->tail)) {
		printk(KERN_INFO "ifxmips_mii0: BUG, len:%d end:%p tail:%p\n",
			(len+4), skb->end, skb->tail);
		goto ifxmips_mii_hw_receive_err_exit;
	}

	skb_put(skb, len);
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	netif_rx(skb);

	priv->stats.rx_packets++;
	priv->stats.rx_bytes += len;
	return 0;

ifxmips_mii_hw_receive_err_exit:
	if (len == 0) {
		if (skb)
			dev_kfree_skb_any(skb);
		priv->stats.rx_errors++;
		priv->stats.rx_dropped++;
		return -EIO;
	} else {
		return len;
	}
}

int ifxmips_mii_hw_tx(char *buf, int len, struct net_device *dev)
{
	int ret = 0;
	struct ifxmips_mii_priv *priv = netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;
	ret = dma_device_write(dma_dev, buf, len, priv->skb);
	return ret;
}

int ifxmips_mii_tx(struct sk_buff *skb, struct net_device *dev)
{
	int len;
	char *data;
	struct ifxmips_mii_priv *priv = netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;

	len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
	data = skb->data;
	priv->skb = skb;
	dev->trans_start = jiffies;
	/* TODO: we got more than 1 dma channel,
	   so we should do something intelligent here to select one */
	dma_dev->current_tx_chan = 0;

	wmb();

	if (ifxmips_mii_hw_tx(data, len, dev) != len) {
		dev_kfree_skb_any(skb);
		priv->stats.tx_errors++;
		priv->stats.tx_dropped++;
	} else {
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += len;
	}

	return 0;
}

void ifxmips_mii_tx_timeout(struct net_device *dev)
{
	int i;
	struct ifxmips_mii_priv *priv = (struct ifxmips_mii_priv *)netdev_priv(dev);

	priv->stats.tx_errors++;
	for (i = 0; i < priv->dma_device->max_tx_chan_num; i++)
		priv->dma_device->tx_chan[i]->disable_irq(priv->dma_device->tx_chan[i]);
	netif_wake_queue(dev);
	return;
}

int dma_intr_handler(struct dma_device_info *dma_dev, int status)
{
	int i;

	switch (status) {
	case RCV_INT:
		ifxmips_mii_hw_receive(ifxmips_mii0_dev, dma_dev);
		break;

	case TX_BUF_FULL_INT:
		printk(KERN_INFO "ifxmips_mii0: tx buffer full\n");
		netif_stop_queue(ifxmips_mii0_dev);
		for (i = 0; i < dma_dev->max_tx_chan_num; i++) {
			if ((dma_dev->tx_chan[i])->control == IFXMIPS_DMA_CH_ON)
				dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
		}
		break;

	case TRANSMIT_CPT_INT:
		for (i = 0; i < dma_dev->max_tx_chan_num; i++)
			dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);

		netif_wake_queue(ifxmips_mii0_dev);
		break;
	}

	return 0;
}

unsigned char *ifxmips_etop_dma_buffer_alloc(int len, int *byte_offset, void **opt)
{
	unsigned char *buffer = NULL;
	struct sk_buff *skb = NULL;

	skb = dev_alloc_skb(ETHERNET_PACKET_DMA_BUFFER_SIZE);
	if (skb == NULL)
		return NULL;

	buffer = (unsigned char *)(skb->data);
	skb_reserve(skb, 2);
	*(int *)opt = (int)skb;
	*byte_offset = 2;

	return buffer;
}

void ifxmips_etop_dma_buffer_free(unsigned char *dataptr, void *opt)
{
	struct sk_buff *skb = NULL;

	if (opt == NULL) {
		kfree(dataptr);
	} else {
		skb = (struct sk_buff *)opt;
		dev_kfree_skb_any(skb);
	}
}

static void
ifxmips_adjust_link(struct net_device *dev)
{
	struct ifxmips_mii_priv *priv = netdev_priv(dev);
	struct phy_device *phydev = priv->phydev;
	int new_state = 0;

	/* Did anything change? */
	if (priv->oldlink != phydev->link ||
		priv->oldduplex != phydev->duplex ||
		priv->oldspeed != phydev->speed) {
		/* Yes, so update status and mark as changed */
		new_state = 1;
		priv->oldduplex = phydev->duplex;
		priv->oldspeed = phydev->speed;
		priv->oldlink = phydev->link;
	}

	/* If link status changed, show new status */
	if (new_state)
		phy_print_status(phydev);
}

static int mii_probe(struct net_device *dev)
{
	struct ifxmips_mii_priv *priv = netdev_priv(dev);
	struct phy_device *phydev = NULL;
	int phy_addr;

	priv->oldlink = 0;
	priv->oldspeed = 0;
	priv->oldduplex = -1;

	/* find the first (lowest address) PHY on the current MAC's MII bus */
	for (phy_addr = 0; phy_addr < PHY_MAX_ADDR; phy_addr++) {
		if (priv->mii_bus->phy_map[phy_addr]) {
			phydev = priv->mii_bus->phy_map[phy_addr];
			break; /* break out with first one found */
		}
	}

	if (!phydev) {
		printk (KERN_ERR "%s: no PHY found\n", dev->name);
		return -ENODEV;
	}

	/* now we are supposed to have a proper phydev, to attach to... */
	BUG_ON(!phydev);
	BUG_ON(phydev->attached_dev);

	phydev = phy_connect(dev, dev_name(&phydev->dev), &ifxmips_adjust_link,
			0, PHY_INTERFACE_MODE_MII);

	if (IS_ERR(phydev)) {
		printk(KERN_ERR "%s: Could not attach to PHY\n", dev->name);
		return PTR_ERR(phydev);
	}

	/* mask with MAC supported features */
	phydev->supported &= (SUPPORTED_10baseT_Half
			      | SUPPORTED_10baseT_Full
			      | SUPPORTED_100baseT_Half
			      | SUPPORTED_100baseT_Full
			      | SUPPORTED_Autoneg
			      /* | SUPPORTED_Pause | SUPPORTED_Asym_Pause */
			      | SUPPORTED_MII
			      | SUPPORTED_TP);

	phydev->advertising = phydev->supported;

	priv->phydev = phydev;

	printk(KERN_INFO "%s: attached PHY driver [%s] "
	       "(mii_bus:phy_addr=%s, irq=%d)\n",
	       dev->name, phydev->drv->name, dev_name(&phydev->dev), phydev->irq);

	return 0;
}


static int ifxmips_mii_dev_init(struct net_device *dev)
{
	int i;
	struct ifxmips_mii_priv *priv = (struct ifxmips_mii_priv *)netdev_priv(dev);
	ether_setup(dev);
	dev->watchdog_timeo = 10 * HZ;
	dev->mtu = 1500;
	memset(priv, 0, sizeof(struct ifxmips_mii_priv));
	priv->dma_device = dma_device_reserve("PPE");
	if (!priv->dma_device) {
		BUG();
		return -ENODEV;
	}
	priv->dma_device->buffer_alloc = &ifxmips_etop_dma_buffer_alloc;
	priv->dma_device->buffer_free = &ifxmips_etop_dma_buffer_free;
	priv->dma_device->intr_handler = &dma_intr_handler;
	priv->dma_device->max_rx_chan_num = 4;

	for (i = 0; i < priv->dma_device->max_rx_chan_num; i++) {
		priv->dma_device->rx_chan[i]->packet_size = ETHERNET_PACKET_DMA_BUFFER_SIZE;
		priv->dma_device->rx_chan[i]->control = IFXMIPS_DMA_CH_ON;
	}

	for (i = 0; i < priv->dma_device->max_tx_chan_num; i++)
		if (i == 0)
			priv->dma_device->tx_chan[i]->control = IFXMIPS_DMA_CH_ON;
		else
			priv->dma_device->tx_chan[i]->control = IFXMIPS_DMA_CH_OFF;

	dma_device_register(priv->dma_device);

	printk(KERN_INFO "%s: using mac=", dev->name);
	for (i = 0; i < 6; i++) {
		dev->dev_addr[i] = mac_addr[i];
		printk("%02X%c", dev->dev_addr[i], (i == 5) ? ('\n') : (':'));
	}

	priv->mii_bus = mdiobus_alloc();
	if (priv->mii_bus == NULL)
		return -ENOMEM;

	priv->mii_bus->priv = dev;
	priv->mii_bus->read = ifxmips_mdiobus_read;
	priv->mii_bus->write = ifxmips_mdiobus_write;
	priv->mii_bus->name = "ifxmips_mii";
	snprintf(priv->mii_bus->id, MII_BUS_ID_SIZE, "%x", 0);
	priv->mii_bus->irq = kmalloc(sizeof(int) * PHY_MAX_ADDR, GFP_KERNEL);
	for(i = 0; i < PHY_MAX_ADDR; ++i)
		priv->mii_bus->irq[i] = PHY_POLL;

	mdiobus_register(priv->mii_bus);

	return mii_probe(dev);
}

static void ifxmips_mii_chip_init(int mode)
{
	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_DMA);
	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_PPE);

	if (mode == REV_MII_MODE)
		ifxmips_w32_mask(PPE32_MII_MASK, PPE32_MII_REVERSE, IFXMIPS_PPE32_CFG);
	else if (mode == MII_MODE)
		ifxmips_w32_mask(PPE32_MII_MASK, PPE32_MII_NORMAL, IFXMIPS_PPE32_CFG);
	ifxmips_w32(PPE32_PLEN_UNDER | PPE32_PLEN_OVER, IFXMIPS_PPE32_IG_PLEN_CTRL);
	ifxmips_w32(PPE32_CGEN, IFXMIPS_PPE32_ENET_MAC_CFG);
	wmb();
}

static const struct net_device_ops ifxmips_eth_netdev_ops = {
	.ndo_init				= ifxmips_mii_dev_init,
	.ndo_open				= ifxmips_ifxmips_mii_open,
	.ndo_stop				= ifxmips_mii_release,
	.ndo_start_xmit			= ifxmips_mii_tx,
	.ndo_tx_timeout			= ifxmips_mii_tx_timeout,
	.ndo_change_mtu			= eth_change_mtu,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr		= eth_validate_addr,
};

static int
ifxmips_mii_probe(struct platform_device *dev)
{
	int result = 0;
	struct ifxmips_eth_data *eth = (struct ifxmips_eth_data*)dev->dev.platform_data;
	ifxmips_mii0_dev = alloc_etherdev(sizeof(struct ifxmips_mii_priv));
	ifxmips_mii0_dev->netdev_ops = &ifxmips_eth_netdev_ops;
	memcpy(mac_addr, eth->mac, 6);
	strcpy(ifxmips_mii0_dev->name, "eth%d");
	ifxmips_mii_chip_init(eth->mii_mode);
	result = register_netdev(ifxmips_mii0_dev);
	if (result) {
		printk(KERN_INFO "ifxmips_mii0: error %i registering device \"%s\"\n", result, ifxmips_mii0_dev->name);
		goto out;
	}

	printk(KERN_INFO "ifxmips_mii0: driver loaded!\n");

out:
	return result;
}

static int ifxmips_mii_remove(struct platform_device *dev)
{
	struct ifxmips_mii_priv *priv = (struct ifxmips_mii_priv *)netdev_priv(ifxmips_mii0_dev);

	printk(KERN_INFO "ifxmips_mii0: ifxmips_mii0 cleanup\n");

	dma_device_unregister(priv->dma_device);
	dma_device_release(priv->dma_device);
	kfree(priv->dma_device);
	unregister_netdev(ifxmips_mii0_dev);
	return 0;
}

static struct platform_driver ifxmips_mii_driver = {
	.probe = ifxmips_mii_probe,
	.remove = ifxmips_mii_remove,
	.driver = {
		.name = "ifxmips_mii0",
		.owner = THIS_MODULE,
	},
};

int __init ifxmips_mii_init(void)
{
	int ret = platform_driver_register(&ifxmips_mii_driver);
	if (ret)
		printk(KERN_INFO "ifxmips_mii0: Error registering platfom driver!");
	return ret;
}

static void __exit ifxmips_mii_cleanup(void)
{
	platform_driver_unregister(&ifxmips_mii_driver);
}

module_init(ifxmips_mii_init);
module_exit(ifxmips_mii_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("ethernet driver for IFXMIPS boards");
