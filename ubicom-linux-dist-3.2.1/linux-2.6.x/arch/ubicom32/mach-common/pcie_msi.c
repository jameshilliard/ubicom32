/*
 * arch/ubicom32/mach-common/pcie_msi.c
 *      PCI Express interface management.
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
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/msi.h>
#include <asm/ubicom32.h>

#include "pcie.h"

/*
 * Find the corresponding pcie root complex port of an MSI irq number
 */
static inline struct pcie_port *msi_to_port(unsigned int irq)
{
	int diff = irq - pcie_ports[0].msi_irq_base;
	return ((diff >= 0) && (diff < PCIE_PORT_MSI_MAX)) ? &pcie_ports[0] : &pcie_ports[1];
}

/*
 * Dynamic irq allocate and deallocation
 */
static int pcie_msi_create_irq(struct pcie_port *pp)
{
	int irq, pos;

	/* Find/alloc a free MSI interrupt */
	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	for (pos = 0; pos < PCIE_PORT_MSI_MAX; pos++) {
		if (pp->msi_irq_base + pos >= NR_IRQS) {
			continue;
		}

		/* Take one slot if available */
		if ((pp->msi_irq_in_use & (1 << pos)) == 0) {
			pp->msi_irq_in_use |= (1 << pos);
			break;
		}
	}
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	if (pos >= PCIE_PORT_MSI_MAX) {
		printk(KERN_WARNING "pos %d too big\n", pos);
		return -ENOSPC;
	}

	irq = pp->msi_irq_base + pos;
	dynamic_irq_init(irq);

	PCIE_INFO("pcie port %d create irq = %d\n", pp->port, irq);
	return irq;
}

/*
 * Release the msi irq resource from a pcie controller
 */
static void pcie_msi_destroy_irq(struct pcie_port *pp, unsigned int irq)
{
	int pos = irq - pp->msi_irq_base;

	BUG_ON(!(pp->msi_irq_in_use & (1 << pos)));

	dynamic_irq_cleanup(irq);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	pp->msi_irq_in_use &= ~(1 << pos);
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	PCIE_INFO("pcie destry irq %d, pos %d\n", irq, pos);
}

/*
 * Msi does not need to ack the interrupt
 */
static void ubi32_msi_nop(unsigned int irq)
{
	return;
}

/*
 * Interrupt chip descriptor for the pcie subsystem
 */
static struct irq_chip ubi32_pcie_msi_chip = {
	.name = "PCIE-MSI",
	.ack = ubi32_msi_nop,
	.enable = unmask_msi_irq,
	.disable = mask_msi_irq,
	.mask = mask_msi_irq,
	.unmask = unmask_msi_irq,
};

/*
 * public hook interface to release an msi interrupt resource
 */
void arch_teardown_msi_irq(unsigned int irq)
{
	struct pcie_port *pp = msi_to_port(irq);
	pcie_msi_destroy_irq(pp, irq);
}

/*
 * public hook interface to allocate an msi interrupt
 */
int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc)
{
	struct pcie_port *pp = ((struct pci_sys_data *)pdev->bus->sysdata)->pport;
	int irq = pcie_msi_create_irq(pp);
	struct msi_msg msg;

	if (irq < 0)
		return irq;

	set_irq_msi(irq, desc);

	msg.address_hi = 0x0;
	msg.address_lo = (u32)pp->msi_addr;
	msg.data = (irq - pp->msi_irq_base) & 0x1f;
	PCIE_INFO("Setup msi msg(0, %x, %x), irq=%d\n", msg.address_lo, msg.data, irq);

	write_msi_msg(irq, &msg);
	set_irq_chip_and_handler(irq, &ubi32_pcie_msi_chip, handle_simple_irq);

	return 0;
}

