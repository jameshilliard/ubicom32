/*
 * drivers/net/ubi32-eth.c
 *   Ubicom32 ethernet TIO interface driver.
 *
 * (C) Copyright 2009-2010, Ubicom, Inc.
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

#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/ip5000.h>
#include <asm/errno.h>
#include <asm/spinlock.h>
#include <asm/devtree.h>

#include "./sk98lin/u-boot_compat.h"
#include "ubi32_eth.h"

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)	(x)
#define unlikely(x)	(x)
#endif

#ifndef CONFIG_ETH_DEV
# define CONFIG_ETH_DEV 0
#endif

#define NETDEV_TX_OK 0
#define NETDEV_TX_BUSY 1	/* driver tx path was busy*/
#define wmb()  asm volatile (""   : : :"memory")

extern int eth_send(volatile void *packet, int length);
extern int eth_rx(void);
extern void eth_halt(void);
extern int eth_init(bd_t *bd);
extern void cleanup_skb(void);

int eth_init_done;
static const char *eth_if_name[UBI32_ETH_NUM_OF_DEVICES] =
	{"eth_lan", "eth_wan"};
static struct eth_device *ubi32_eth_devices[UBI32_ETH_NUM_OF_DEVICES] =
	{NULL, NULL};

struct ubi32_eth_private *netdev_priv(struct eth_device *dev) {
	return(dev->priv);
}

static inline struct sk_buff *ubi32_alloc_skb(struct eth_device *dev, unsigned int length)
{
	return alloc_skb(length, GFP_ATOMIC);
}

static void ubi32_eth_vp_rxtx_enable(struct eth_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	priv->regs->command = UBI32_ETH_VP_CMD_RX_ENABLE | UBI32_ETH_VP_CMD_TX_ENABLE;
	priv->regs->int_mask = (UBI32_ETH_VP_INT_RX | UBI32_ETH_VP_INT_TX);
	ubicom32_set_interrupt(priv->vp_int_bit);
}

static void ubi32_eth_vp_rxtx_stop(struct eth_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	priv->regs->command = 0;
	priv->regs->int_mask = 0;
	ubicom32_set_interrupt(priv->vp_int_bit);

	/* Wait for graceful shutdown */
	while (priv->regs->status & (UBI32_ETH_VP_STATUS_RX_STATE | UBI32_ETH_VP_STATUS_TX_STATE));
}

/*
 * ubi32_eth_tx_done()
 */
static int ubi32_eth_tx_done(struct eth_device *dev)
{
	struct ubi32_eth_private *priv;
	struct sk_buff *skb;
	volatile void *pdata;
	struct ubi32_eth_dma_desc *desc;
	u32_t 	count = 0;

	priv = netdev_priv(dev);

 	priv->regs->int_status &= ~UBI32_ETH_VP_INT_TX;
	while (priv->tx_tail != priv->regs->tx_out) {
		pdata = priv->regs->tx_dma_ring[priv->tx_tail];
		BUG_ON(pdata == NULL);

		skb = container_of((void *)pdata, struct sk_buff, cb);
		desc = (struct ubi32_eth_dma_desc *)pdata;
		if (unlikely(!(desc->status & UBI32_ETH_VP_TX_OK))) {
			priv->stats.tx_errors++;
		} else {
			priv->stats.tx_packets++;
			priv->stats.tx_bytes += skb->len;
		}
		dev_kfree_skb_any(skb);
		priv->regs->tx_dma_ring[priv->tx_tail] = NULL;
		priv->tx_tail = (priv->tx_tail + 1) & TX_DMA_RING_MASK;
		count++;
	}

	if (unlikely(priv->regs->status & UBI32_ETH_VP_STATUS_TX_Q_FULL)) {
		ubi32_spin_lock(&priv->lock);
		if ((count != 0) && (priv->regs->status & UBI32_ETH_VP_STATUS_TX_Q_FULL)) {
			priv->regs->status &= ~UBI32_ETH_VP_STATUS_TX_Q_FULL;
			ubicom32_set_interrupt(priv->vp_int_bit);
		}
		ubi32_spin_unlock(&priv->lock);
	}
	return count;
}

/*
 * ubi32_eth_receive()
 *	To avoid locking overhead, this is called only
 *	by tasklet when not using NAPI, or
 *	by NAPI poll when using NAPI.
 *	return number of frames processed
 */
static int ubi32_eth_receive(struct eth_device *dev, int quota)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	unsigned short rx_in = priv->regs->rx_in;
	struct sk_buff *skb;
	struct ubi32_eth_dma_desc *desc = NULL;
	volatile void *pdata;

	int extra_reserve_adj;
	int extra_alloc = UBI32_ETH_RESERVE_SPACE + UBI32_ETH_TRASHED_MEMORY;
	int replenish_cnt, count = 0;
	int replenish_max = RX_DMA_MAX_QUEUE_SIZE;

	if (unlikely(rx_in == priv->regs->rx_out))
		priv->vp_stats.rx_q_full_cnt++;

	priv->regs->int_status &= ~UBI32_ETH_VP_INT_RX;
	while (priv->rx_tail != priv->regs->rx_out) {
		if (unlikely(count == quota)) {
			/*
			 * There are still frames pending to be processed
			 */
			priv->vp_stats.rx_throttle++;
			break;
		}

		pdata = priv->regs->rx_dma_ring[priv->rx_tail];
		BUG_ON(pdata == NULL);

		desc = (struct ubi32_eth_dma_desc *)pdata;
		skb = container_of((void *)pdata, struct sk_buff, cb);
		count++;
		priv->regs->rx_dma_ring[priv->rx_tail] = NULL;
		priv->rx_tail = ((priv->rx_tail + 1) & RX_DMA_RING_MASK);

		/*
		 * Check only RX_OK bit here.
		 * The rest of status word is used as timestamp
		 */
		if (unlikely(!(desc->status & UBI32_ETH_VP_RX_OK))) {
			priv->stats.rx_errors++;
			dev_kfree_skb_any(skb);
			continue;
		}

		skb_put(skb, desc->data_len);
		NetReceive(skb->data, skb->len);
		dev_kfree_skb_any(skb);
	}

	/*
	 * Fill in more descripor for VP
	 */
	replenish_cnt =  replenish_max -
		((RX_DMA_RING_SIZE + rx_in - priv->rx_tail) & RX_DMA_RING_MASK);
	if (replenish_cnt > 0) {
		while (replenish_cnt-- > 0) {
			skb = ubi32_alloc_skb(dev, RX_BUF_SIZE + extra_alloc);
			if (!skb) {
				priv->vp_stats.rx_alloc_err++;
				break;
			}

			desc = (struct ubi32_eth_dma_desc *)skb->cb;
			extra_reserve_adj =
				((u32)skb->data + UBI32_ETH_RESERVE_SPACE + ETH_HLEN) &
				(CACHE_LINE_SIZE - 1);
			skb_reserve(skb, UBI32_ETH_RESERVE_SPACE - extra_reserve_adj);
			desc->data_pointer = skb->data;
			desc->buffer_len = RX_BUF_SIZE + UBI32_ETH_TRASHED_MEMORY;
			desc->data_len = 0;
			desc->status = 0;
			priv->regs->rx_dma_ring[rx_in] = desc;
			rx_in = (rx_in + 1) & RX_DMA_RING_MASK;
		}

		wmb();
		priv->regs->rx_in = rx_in;
		ubicom32_set_interrupt(priv->vp_int_bit);
	}

	return count;
}

static void ubi32_eth_do_tasklet(unsigned long arg)
{
	struct eth_device *dev = (struct eth_device *)arg;
	struct ubi32_eth_private *priv = netdev_priv(dev);

	if (priv->tx_tail != priv->regs->tx_out) {
		ubi32_eth_tx_done(dev);
	}

	/*
	 * Always call receive to process new RX frame as well as replenish RX buffers
	 */
	ubi32_eth_receive(dev, UBI32_RX_BOUND);

	priv->regs->int_mask |= (UBI32_ETH_VP_INT_RX | UBI32_ETH_VP_INT_TX);
	if ((priv->rx_tail != priv->regs->rx_out) || (priv->tx_tail != priv->regs->tx_out)) {
		priv->regs->int_mask = 0;
	}
}

static int ubi32_eth_start_xmit(struct sk_buff *skb, struct eth_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	struct ubi32_eth_dma_desc *desc = NULL;
	unsigned short space, tx_in;

	tx_in = priv->regs->tx_in;

	space = TX_DMA_RING_MASK - ((TX_DMA_RING_SIZE + tx_in - priv->tx_tail) & TX_DMA_RING_MASK);

	if (unlikely(space == 0)) {
		if (!(priv->regs->status & UBI32_ETH_VP_STATUS_TX_Q_FULL)) {
			ubi32_spin_lock(&priv->lock);
			if (!(priv->regs->status & UBI32_ETH_VP_STATUS_TX_Q_FULL)) {
				priv->regs->status |= UBI32_ETH_VP_STATUS_TX_Q_FULL;
				priv->vp_stats.tx_q_full_cnt++;
			}
			ubi32_spin_unlock(&priv->lock);
		}
		ubicom32_set_interrupt(priv->vp_int_bit);

		return NETDEV_TX_BUSY;
	}

	/*
	 * still have room 
	 */
	desc = (struct ubi32_eth_dma_desc *)skb->cb;
	desc->data_pointer = skb->data;
	desc->data_len = skb->len;
	priv->regs->tx_dma_ring[tx_in] = desc;
	tx_in = ((tx_in + 1) & TX_DMA_RING_MASK);
	wmb();
	priv->regs->tx_in = tx_in;

	/*
	 * Kick the HRT
	 */
	ubicom32_set_interrupt(priv->vp_int_bit);

	return NETDEV_TX_OK;
}

/*
 * ubi32_eth_cleanup: unload the module
 */
void ubi32_eth_cleanup(void)
{
	struct ubi32_eth_private *priv;
	struct eth_device *dev;
	int i;

	for (i = 0; i < UBI32_ETH_NUM_OF_DEVICES; i++) {
		dev = ubi32_eth_devices[i];
		if (dev) {
			ubi32_eth_vp_rxtx_stop(dev);			
			priv = netdev_priv(dev);
			if (priv) {
				if (priv->regs->tx_dma_ring) {
					free(priv->regs->tx_dma_ring);
				}
				free(priv);
			}
			free(dev);
			ubi32_eth_devices[i] = NULL;
		}
	}
	
	cleanup_skb();
}

int ubi32_eth_init_module(void)
{
	struct ethtionode *eth_node;
 	struct eth_device *dev;
	struct ubi32_eth_private *priv;
	int i, err;
	
	cleanup_skb();
	
	/*
	 * Device allocation.
	 */
	err = 0;
	for (i = 0; i < UBI32_ETH_NUM_OF_DEVICES; i++) {
		/*
		 * See if the eth_vp is in the device tree.
		 */
		eth_node = (struct ethtionode *)devtree_find_node(eth_if_name[i]);
		if (!eth_node) {
			printk(KERN_INFO "%s does not exist\n", eth_if_name[i]);
			continue;
		}

		eth_node->tx_dma_ring = (struct ubi32_eth_dma_desc **)malloc(
				sizeof(struct ubi32_eth_dma_desc *) *
				(TX_DMA_RING_SIZE + RX_DMA_RING_SIZE));
		if (!eth_node->tx_dma_ring) {
			err = -ENOMEM;
			break;
		}
		eth_node->rx_dma_ring = eth_node->tx_dma_ring + TX_DMA_RING_SIZE;
		eth_node->tx_sz = TX_DMA_RING_SIZE - 1;
		eth_node->rx_sz = RX_DMA_RING_SIZE - 1;

		dev = malloc(sizeof(struct eth_device));
		if (!dev) {
			free(eth_node->tx_dma_ring);
			err = -ENOMEM;
			break;
		}

		priv = malloc(sizeof(struct ubi32_eth_private));
		if (!dev) {
			free(eth_node->tx_dma_ring);
			free(dev);
			err = -ENOMEM;
			break;
		}

		memcpy(dev->name, eth_if_name[i], sizeof dev->name - 1);
		dev->priv = priv;
		priv->dev = dev;

		/*
		 * This just fill in some default Ubicom MAC address
		 */
		eth_getenv_enetaddr("ethaddr", dev->dev_addr);

		priv->regs = eth_node;
		priv->regs->command = 0;
		priv->regs->int_mask = 0;
		priv->regs->int_status = 0;
		priv->regs->tx_out = 0;
		priv->regs->rx_out = 0;
		priv->regs->tx_in = 0;
		priv->regs->rx_in = 0;
		priv->rx_tail = 0;
		priv->tx_tail = 0;

		priv->vp_int_bit = eth_node->dn.sendirq;
		priv->irq = eth_node->dn.recvirq;
	
		ubi32_spin_lock_init(&priv->lock);

		ubi32_eth_devices[i] = dev;
		printk(KERN_INFO "%s vp_base:0x%p, tio_int:%d irq:%d\n",
			dev->name, priv->regs, eth_node->dn.sendirq, eth_node->dn.recvirq);
	}

	if (err) {
		ubi32_eth_cleanup();
		return err;
	}

	if (!ubi32_eth_devices[0] && !ubi32_eth_devices[1]) {
		return -ENODEV;
	}

	printf("ubi32_eth_init_module completed\n");

	return 0;
}

int eth_send(volatile void *packet, int len)
{
	/* 
	 * use the WAN interface until we can configure the switch
	 */
	struct sk_buff *skb = ubi32_alloc_skb(ubi32_eth_devices[CONFIG_ETH_DEV], len);
	if (skb) {
		memcpy(skb->data, (void *)packet, len);
		ubi32_eth_start_xmit(skb, ubi32_eth_devices[CONFIG_ETH_DEV]);
		return 0;
	}

	return 1;
}

int eth_rx(void)
{
	struct eth_device *dev;

	dev = ubi32_eth_devices[CONFIG_ETH_DEV];	//i
	if (dev) {	// && (dev->flags & IFF_UP)) {
		ubi32_eth_do_tasklet((unsigned long)dev);
	}

	return 1;	
}

void eth_halt(void)
{
	/*
	 * We are not initialized yet, so nothing to stop
	 */
	if (!eth_init_done) {
		return;
	}

	ubi32_eth_vp_rxtx_stop(ubi32_eth_devices[CONFIG_ETH_DEV]);
}

int eth_init(bd_t *bd)
{
	if (!eth_init_done) {
		ubi32_eth_init_module();
		eth_init_done = 1;
	}

	ubi32_eth_vp_rxtx_enable(ubi32_eth_devices[CONFIG_ETH_DEV]);
	return 0;
}
