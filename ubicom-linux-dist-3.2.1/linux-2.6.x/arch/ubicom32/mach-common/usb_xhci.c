/*
 * arch/ubicom32/mach-common/usb_xhci.c
 *      XHCI USB interface management.
 *
 * (C) Copyright 2010, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/arm
 *   arch/mips
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <asm/ubicom32.h>
#include <asm/ubicom32-io.h>

#include "usb_xhci.h"

#ifdef CONFIG_ATOMIC_INDIRECT_ACCESS
#define BR_INDIRECT_LOCK(flags) __atomic_lock_acquire()
#define BR_INDIRECT_UNLOCK(flags) __atomic_lock_release()
#else
#include <asm/processor.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
static spinlock_t br_indirect_lock = SPIN_LOCK_UNLOCKED;
#define BR_INDIRECT_LOCK(flags) spin_lock_irqsave(&br_indirect_lock, flags)
#define BR_INDIRECT_UNLOCK(flags) spin_unlock_irqrestore(&br_indirect_lock, flags)
#endif

struct xhci_port xhci_ports[XHCI_PORT_MAX];

/*
 * Reset the port's phy and controller. After this function
 * the controller core is ready to be programed.
 */
static void __init xhci_port_reset(struct xhci_port *pp)
{
	struct ubicom32_io_port *phy = pp->phy;
	struct ubicom32_io_port *ctl = pp->ctl;
        u32 reg;

	/*
	 * Enable the clock to 125 MHz
	 */
	XHCI_TRACE("enable clock[0x%08x]\n", (u32)(GENERAL_CFG_BASE + GEN_CLK_IO_SELECT));
	reg = xhci_nbr_readl((volatile u32 *)(GENERAL_CFG_BASE + GEN_CLK_IO_SELECT));
	if (pp->port == 0) {
#if !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_HSSIO_CONFIG) &= ~((1<<27)|(1<<22)|(1<<20)|(1<<18));
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_HSSIO_CONFIG) |= (1<<26)|(1<<4)|(1<<2);
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_IO_SELECT) |= (1<<2);
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_IO_DISABLE) &= ~(1<<2);
#endif
	} else {
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_IO_SELECT) |= (1<<28)|(1<<3)|(1<<1);
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_IO_DISABLE) &= ~((1<<3)|(1<<1));
#else
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_HSSIO_CONFIG) &= ~((1<<29)|(1<<23)|(1<<21)|(1<<19));
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_HSSIO_CONFIG) |= (1<<28)|(1<<5)|(1<<3);
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_IO_SELECT) |= (1<<1);
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_IO_DISABLE) &= ~(1<<1);
#endif
	}

	mdelay(1);

	/*
	 * Select the function of XHCI controller but keep it in reset state
	 */
	XHCI_TRACE("select function[0x%08x]\n",(u32)&ctl->function);
	xhci_nbr_writel(&ctl->function, IO_PORTX_FUNC_USB3_0 | XHCI_FN_RESET_BIT);

	/*
	 * Set core variables and srst
	 */
	XHCI_TRACE("Set func_ctl1[0x%08x] core variables and srst\n", (u32)&ctl->ctl1);
	xhci_nbr_writel(&ctl->ctl1, 
		       XHCI_SRST |
		       XHCI_CFG_DW_CAP |
		       /* disable warm reset because RTL does not support this */
		       USB3_CFG_NO_SS_INACTIVE
		       /* chip does not support u1 and u2 */
		      //|XHCI_CFG_U1_ENABLE|XHCI_CFG_U2_ENABLE
		       );
	printk(KERN_INFO "func_ctl1[0x%08x] = 0x%08x\n", (u32)&ctl->ctl1, ctl->ctl1);

	/*
	 * Set AHB DMA bigendian
	 */
	XHCI_TRACE("Set func_ctl6[0x%08x] core variables and srst\n", (u32)&ctl->ctl6);
	xhci_nbr_writel(&ctl->ctl6, XHCI_CTL6_DMA_AHB_BIGENDIAN);

	/* 
	 * Select and reset the phy 
	 */
	XHCI_TRACE("sel and reset phy_func[0x%08x]\n", (u32)&phy->function);
        xhci_nbr_writel(&phy->function, IO_FUNC_SELECT(XHCI_PHY_FN_SEL) | 
					(IO_FUNC_FUNCTION_RESET(XHCI_PHY_FN_SEL) << 1));
	/* 
	 * Take the phy port out of reset
	 */
	XHCI_TRACE("remove reset phy_func[0x%08x]\n", (u32)&phy->function);
	xhci_nbr_writel(&phy->function, IO_FUNC_SELECT(XHCI_PHY_FN_SEL));

	/*
	 * Configure phy
	 */
	xhci_phy_config(phy);

	/* 
	 * Wait for phy_status to deassert
	 */
	XHCI_TRACE("waiting for phy_status to deassert\n");
        while (phy->status0 & XHCI_PHY_STATUS0_MASK);

	/*
	 * Bring the XHCI function out of reset
	 */
	XHCI_TRACE("bring xhci ctl_func[[0x%08x] out of reset\n", (u32)&ctl->function);
	xhci_nbr_writel(&ctl->function, IO_PORTX_FUNC_USB3_0);

	/*
	 * Clear srst to register configuration variables into XHCI 	
	 */
	XHCI_TRACE("clear srst ctl_ctl1[0x%08x]\n", (u32)&ctl->ctl1);
	ctl->ctl1 &= ~XHCI_SRST;
}

/*
 * The main entrance to setup and enable a xhci controller port.
 */
static void __init xhci_port_init(int port)
{
	struct xhci_port *pp = &xhci_ports[port];

	XHCI_TRACE("%s port %d\n", __func__, port);
	pp->port = port;

	/* Cache the starting address of port control registers */
	pp->phy = (struct ubicom32_io_port *)XHCI_PHY_FUNC_BASE(port);
	pp->ctl = (struct ubicom32_io_port *)XHCI_CTL_FUNC_BASE(port);
	pp->core = (u32 *)XHCI_CTL_BR_BASE(port);

	XHCI_TRACE("phy[0x%08x], ctl[0x%08x], core[0x%08x]\n",
		(u32)pp->phy, (u32)pp->ctl, (u32)pp->core);

	/* Init phy, core and make sure link is up */ 
	xhci_port_reset(pp);	

	/* Enable ctl interrupts */
	xhci_nbr_writel(&pp->ctl->int_mask, XHCI_INTR_CTL_MASK);
}

void ubi32_xhci_dma_ready(volatile u32 *base)
{
        struct ubicom32_io_port *ctl = (struct ubicom32_io_port *)base;

#ifdef DEBUG
	XHCI_TRACE("dma_ready wcmdq(%x, %p), stall(%x, %p)\n", fbase + IO_STATUS1,
		&ctl->status0, fbase + IO_CTL1, &ctl->ctl1);
#endif
        /* polls dma_wcmdq_empty bit of STATUS0 */
        if (likely(ctl->status0 & WCMDQ_EMPTY_BIT))
                return;

#ifdef XHCI_DMA_STALL
        /* tell device stop feeding till the current written data is ready */
        ctl->ctl1 |= WSTALL_BIT;
#endif

        /* keep polling till it is ready */
        while (!(ctl->status0 & WCMDQ_EMPTY_BIT)) ;

#ifdef XHCI_DMA_STALL
        /* clear the stall bit */
        ctl->ctl1 &= ~WSTALL_BIT;
#endif
}
EXPORT_SYMBOL(ubi32_xhci_dma_ready);

void ubi32_xhci_int_ack(volatile void __iomem *base)
{
	struct ubicom32_io_port *ctl = (struct ubicom32_io_port *)base;

	xhci_nbr_writel(&ctl->int_set, XHCI_PCI_INTX_ACK);
}
EXPORT_SYMBOL(ubi32_xhci_int_ack);

void ubi32_xhci_clear_int(volatile void __iomem *base)
{
	struct ubicom32_io_port *ctl = (struct ubicom32_io_port *)base;

	while(ctl->int_status & XHCI_INTR_INTX);
	xhci_nbr_writel(&ctl->int_set, XHCI_PCI_INTX_ACK);
}
EXPORT_SYMBOL(ubi32_xhci_clear_int);

/*
 * Read a register indirectly from a blocking region of XHCI controller
 * 	addr: the register address
 * 	base: the PIO function base of the XHCI core
 */
u32 xhci_br_readl(volatile void __iomem *addr, volatile void __iomem *base)
{
	u32 val = 0;
	volatile u32 *ctl0 = FUNCTION_CTL0(base);
	volatile u32 *br_base = FUNCTION_BR_BASE(base);
	u32 br_offset = FUNCTION_BR_OFFSET(addr, br_base);
#ifndef CONFIG_ATOMIC_INDIRECT_ACCESS
	unsigned long flags;
#endif

	BR_INDIRECT_LOCK(flags);

	/* write BR offset into CTR0, wait for 5 cycles, then read the value */
	asm volatile (
		" move.4 (%[ctl0]), %[br_offset]			\n"
		" cycles 5					\n"

#ifdef XHCI_SWAP_ENDIAN
		" swapb.4 %[val], (%[br_base])			\n"
#else
		" move.4 %[val], (%[br_base])			\n"
#endif
		: [val] "=&d" (val)
		: [ctl0] "a" (ctl0), [br_base] "a" (br_base), [br_offset] "d" (br_offset)
		: "cc"
		);

	BR_INDIRECT_UNLOCK(flags);

	return val;
}
EXPORT_SYMBOL(xhci_br_readl);

/*
 * Write a new value indirectly to a blocking region register of xHC core
 * 	addr: the register address
 * 	base: the PIO function base of the xHC core
 */
void xhci_br_writel(u32 val, volatile void __iomem *addr, volatile void __iomem *base)
{
	volatile u32 *ctl0 = FUNCTION_CTL0(base);
	volatile u32 *br_base = FUNCTION_BR_BASE(base);
	u32 br_offset = FUNCTION_BR_OFFSET(addr, br_base);
#ifndef CONFIG_ATOMIC_INDIRECT_ACCESS
	unsigned long flags;
#endif

	BR_INDIRECT_LOCK(flags);

	/* write BR offset into CTR0, wait for 5 cycles, then write the value */
	asm volatile (
		" move.4 (%[ctl0]), %[br_offset]			\n"
		" cycles 5					\n"

#ifdef XHCI_SWAP_ENDIAN
		" swapb.4 (%[br_base]), %[val]			\n"
#else
		" move.4 (%[br_base]), %[val]			\n"
#endif
		:
		: [ctl0] "a" (ctl0), [br_base] "a" (br_base), [br_offset] "d" (br_offset), [val] "d" (val)
		: "cc"
		);

	BR_INDIRECT_UNLOCK(flags);
}
EXPORT_SYMBOL(xhci_br_writel);

static struct resource ctl0_usb_resources[] = {
	[0] = {
		.start	= XHCI_CTL0_BASE + REG_SIZE,
		.end	= XHCI_CTL0_BASE + REG_SIZE * 2 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {	/* general IRQ */
		.start	= RW_USB30_INT,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource ctl1_usb_resources[] = {
	[0] = {
		.start	= XHCI_CTL1_BASE + REG_SIZE,
		.end	= XHCI_CTL1_BASE + REG_SIZE * 2 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {	/* general IRQ */
		.start	= RK_USB30PCIE_INT,
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 xhci_dmamask = 0xffffffffUL;

/*
 * The 'logical module' device list for USB3.0
 */
static struct platform_device xhci_devices[] = {
	{
		.name		= "ubi32_xhci_ctl",
		.id		= 0,
		.dev = {
			.platform_data		= NULL,
			.dma_mask		= &xhci_dmamask,
			.coherent_dma_mask	= 0xffffffff,
		},
		.resource	= ctl0_usb_resources,
		.num_resources	= ARRAY_SIZE(ctl0_usb_resources),
	 },

	{
		.name		= "ubi32_xhci_ctl",
		.id		= 1,
		.dev = {
			.platform_data		= NULL,
			.dma_mask		= &xhci_dmamask,
			.coherent_dma_mask	= 0xffffffff,
		},
		.resource	= ctl1_usb_resources,
		.num_resources	= ARRAY_SIZE(ctl1_usb_resources),
	 },
};

/*
 * Init and register a xhci device
 */
static void __init xhci_init_and_register_device(int inst)
{
        XHCI_TRACE("%s: %d\n", __func__, inst);
	xhci_port_init(inst);
	XHCI_INFO("USB 3.0 port %d is brought up\n", inst);

	if (platform_device_register(&xhci_devices[inst]) < 0) {
		printk(KERN_ERR "Unable to register SS-USB (XHCI) device\n");
	}
}

/*
 * Usb 3.0 subsystem init
 */
int ubi32_xhci_init(void)
{
        XHCI_INFO("%s:\n", __func__);
#ifdef CONFIG_XHCI_PORT0_ENABLE
	xhci_init_and_register_device(0);
#endif
#ifdef CONFIG_XHCI_PORT1_ENABLE
	xhci_init_and_register_device(1);
#endif
	return 0;
}

subsys_initcall(ubi32_xhci_init);

#ifdef CONFIG_UBI32_SS_USB2_MODE

static void xhci_port_shutdown(struct xhci_port *pp)
{
	struct ubicom32_io_port *phy = pp->phy;
	struct ubicom32_io_port *ctl = pp->ctl;

	XHCI_TRACE("Disable phy clock[0x%08x]\n", (u32)(GENERAL_CFG_BASE + GEN_CLK_IO_DISABLE));
	if (pp->port == 0) {
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_IO_DISABLE) |= (1<<0);
	} else {
		*(volatile u32_t *)(GENERAL_CFG_BASE + GEN_CLK_IO_DISABLE) |= (1<<1);
	}
	/*
	 * Select the function of XHCI controller but keep it in reset state
	 */
	XHCI_TRACE("select function[0x%08x]\n",(u32)&ctl->function);
	xhci_nbr_writel(&ctl->function, XHCI_FN_RESET_BIT);

	/*
	 * Select and reset the phy
	 */
	XHCI_TRACE("sel and reset phy_func[0x%08x]\n", (u32)&phy->function);
        xhci_nbr_writel(&phy->function, (IO_FUNC_FUNCTION_RESET(XHCI_PHY_FN_SEL) << 1));
}

static void xhci_device_shutdown(int port)
{
	struct xhci_port *pp = &xhci_ports[port];

        XHCI_TRACE("%s: %d\n", __func__, port);
	pp->ctl->int_mask &= ~XHCI_INTR_CTL_MASK;
	xhci_port_shutdown(pp);
	XHCI_TRACE("USB 3.0 port %d is shutdown\n", port);
}

void ubi32_xhci_pdev_unregister(void)
{
#ifdef CONFIG_XHCI_PORT0_ENABLE
	platform_device_unregister(&xhci_devices[0]);
#endif
#ifdef CONFIG_XHCI_PORT1_ENABLE
	platform_device_unregister(&xhci_devices[1]);
#endif
}
EXPORT_SYMBOL(ubi32_xhci_pdev_unregister);

void ubi32_xhci_port_shutdown(void)
{
        XHCI_TRACE("%s:\n", __func__);
#ifdef CONFIG_XHCI_PORT0_ENABLE
	xhci_device_shutdown(0);
#endif
#ifdef CONFIG_XHCI_PORT1_ENABLE
	xhci_device_shutdown(1);
#endif
}
EXPORT_SYMBOL(ubi32_xhci_port_shutdown);

int ubi32_xhci_port_init(void)
{
        XHCI_TRACE("%s:\n", __func__);
#ifdef CONFIG_XHCI_PORT0_ENABLE
	xhci_port_init(0);
#endif
#ifdef CONFIG_XHCI_PORT1_ENABLE
	xhci_port_init(1);
#endif
	return 0;
}
EXPORT_SYMBOL(ubi32_xhci_port_init);

#endif
