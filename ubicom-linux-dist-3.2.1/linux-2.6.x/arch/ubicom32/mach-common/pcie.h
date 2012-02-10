/*
 * arch/ubicom32/mach-common/pcie.h
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

#ifndef __UBICOM32_PCIE_H__
#define __UBICOM32_PCIE_H__

#include <linux/kernel.h>
#include <asm/ubicom32.h>
#include <asm/delay.h>

/*
 * FIXME: For now, we need to swap endianess when we set the core to work
 * with big endian, kind of weird.
 */
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
#define PCIE_AHB_BIGENDIAN
#else
#define PCIE_AXI_BIGENDIAN
#endif

#ifdef PCIE_AHB_BIGENDIAN
#define PCIE_SWAP_ENDIAN
#endif
#ifdef PCIE_AXI_BIGENDIAN
//#define PCIE_SWAP_ENDIAN
#endif

#define PCIE_BR_ACCESS_DELAY	8
#define PCIE_BR_SWITCH_DELAY	{asm volatile ("nops 5\n" "jmpt.f .+4\n" : :);}

/* Debug level macros */
#ifdef CONFIG_PCI_DEBUG

#define PCIE_TRACE(fmt, ...)			//printk(KERN_INFO fmt, ##__VA_ARGS__)
#define PCIE_INFO(fmt, ...)			printk(KERN_INFO fmt, ##__VA_ARGS__)
#define PCIE_WARNING(fmt, ...)			printk(KERN_WARNING fmt, ##__VA_ARGS__)

#define PCIE_ASSERT(x, fmt, ...) \
{ \
	if (!(x)) { \
		printk(KERN_ERR "%s:%d " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
		BUG(); \
	} \
}

#else // PCI_DEBUG

#define PCIE_TRACE(fmt, ...)
#define PCIE_INFO(fmt, ...)
#define PCIE_WARNING(fmt, ...)
#define PCIE_ASSERT(x, fmt, ...)		//BUG_ON(!(x))

#endif // PCI_DEBUG

/* Profiler support */
//#define PCIE_UBICOM_PROFILER
#ifdef PCIE_UBICOM_PROFILER
#define PCIE_UBICOM_PROFILER_VCOUNT_INC(x) ((x)++)
#else
#define PCIE_UBICOM_PROFILER_VCOUNT_INC(x)
#endif

/* Up to two PCI instances */
#define PCIE_PORT_MAX 2
#define PCIE_PORT_MSI_MAX 16

#define PCIE_PHY0_BASE				IO_PORT_RL
#define PCIE_PHY1_BASE				IO_PORT_RM
#define PCIE_PHY_FUNC_BASE(inst)		((inst) ? PCIE_PHY1_BASE : PCIE_PHY0_BASE)
#define PCIE_PHY_BR_BASE(inst)			(PCIE_PHY_FUNC_BASE(inst) + IO_PORT_BR_OFFSET)

#define PCIE_CTL0_BASE				IO_PORT_RJ
#define PCIE_CTL1_BASE			  	IO_PORT_RK
#define PCIE_CTL_FUNC_BASE(inst)		((inst) ? PCIE_CTL1_BASE : PCIE_CTL0_BASE)
#define PCIE_CTL_BR_BASE(inst)			(PCIE_CTL_FUNC_BASE(inst) + IO_PORT_BR_OFFSET)

/* bit of wcmdq_empty in status 0*/
#define WCMDQ_EMPTY_BIT				(1 << 0)

/* bit of wstall in ctl3 */
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
#define WSTALL_BIT				(1 << 17)
#else
#define WSTALL_BIT				(1 << 0)
#endif

/*
 * Bitmap definition for imask and istatus
 */
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
#define PCIE_INTR_WR_DMA_END 			(1 << 0)
#define PCIE_INTR_WR_PCI_ERR 			(1 << 1)
#define PCIE_INTR_WR_AHB_ERR 			(1 << 2)
#define PCIE_INTR_WR_DMA_MSK 			(PCIE_INTR_WR_DMA_END | PCIE_INTR_WR_PCI_ERR | PCIE_INTR_WR_AHB_ERR)

#define PCIE_INTR_RD_DMA_END 			(1 << 4)
#define PCIE_INTR_RD_PCI_ERR 			(1 << 5)
#define PCIE_INTR_RD_AHB_ERR 			(1 << 6)
#define PCIE_INTR_RD_DMA_MSK 			(PCIE_INTR_RD_DMA_END | PCIE_INTR_RD_PCI_ERR | PCIE_INTR_RD_AHB_ERR)

#define PCIE_INTR_WIN_FETCH			(1 << 13)
#define PCIE_INTR_WIN_DISCARD 			(1 << 14)
#define PCIE_INTR_WIN_ERR_MSK			(PCIE_INTR_WIN_FETCH | PCIE_INTR_WIN_DISCARD)

#define PCIE_INTR_INTA				(1 << 16)
#define PCIE_INTR_INTB				(1 << 17)
#define PCIE_INTR_INTC				(1 << 18)
#define PCIE_INTR_INTD				(1 << 19)
#define PCIE_INTR_INTX				(0xf << 16)

#define PCIE_INTR_SYSERR			(1 << 20)
#define PCIE_INTR_PMHOTPLUG			(1 << 21)
#define PCIE_INTR_AER				(1 << 22)
#define PCIE_INTR_MSI				(1 << 23)
#define PCIE_INTR_LEGACY_PWR			(1 << 29)
#define PCIE_INTR_CTL_MSK			(PCIE_INTR_INTX | PCIE_INTR_SYSERR | PCIE_INTR_PMHOTPLUG | PCIE_INTR_AER | PCIE_INTR_MSI | PCIE_INTR_LEGACY_PWR)

#else	/* CONFIG_UBICOM32_IP8K_SHUTTLE */
#define PCIE_INTR_INTA				(1 << 0)
#define PCIE_INTR_INTB				(1 << 1)
#define PCIE_INTR_INTC				(1 << 2)
#define PCIE_INTR_INTD				(1 << 3)
#define PCIE_INTR_INTX				(0xf << 0)

#define PCIE_INTR_AER				(1 << 4)
#define PCIE_INTR_PMHOTPLUG			(1 << 5)
#define PCIE_INTR_MSI				(1 << 6)
#define PCIE_INTR_SYSERR			(1 << 7)

#define PCIE_INTR_CFG_END			(1 << 9)
#define PCIE_INTR_CFG_ERROR			(1 << 10)
#define PCIE_INTR_IO_END			(1 << 11)
#define PCIE_INTR_IO_ERROR			(1 << 12)
#define PCIE_INTR_CTL_MSK			(PCIE_INTR_INTX | PCIE_INTR_SYSERR | PCIE_INTR_PMHOTPLUG | PCIE_INTR_AER | PCIE_INTR_MSI)

#endif	/* CONFIG_UBICOM32_IP8K_SHUTTLE */

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
/*
 * Macros related to DMA control (P25 of the PLDA ref manual)
 * (size, byte_enable, command, stop, start)
 */
#define PCIE_DMA_RD_IO				0
#define PCIE_DMA_WR_IO				1
#define PCIE_DMA_RD_MEM_DW			2
#define PCIE_DMA_WR_MEM_DW			3
#define PCIE_DMA_RD_CFG_REG			4
#define PCIE_DMA_WR_CFG_REG			5
#define PCIE_DMA_RD_MEM_BRST			6
#define PCIE_DMA_WR_MEM_BRST			7

#define PCIE_DMA_CTL(sz, be, cmd, stp, strt)	(((sz) << 12) | ((be) << 8) | ((cmd) << 4) | ((stp) << 1) | (strt))
#define PCIE_DMA_CTL_CFG_RD()			PCIE_DMA_CTL(4, 0xf, PCIE_DMA_RD_CFG_REG, 0, 1)
#define PCIE_DMA_CTL_CFG_WR()			PCIE_DMA_CTL(4, 0xf, PCIE_DMA_WR_CFG_REG, 0, 1)
#define PCIE_DMA_CTL_DW_RD(be)			PCIE_DMA_CTL(4, be, PCIE_DMA_RD_MEM_DW, 0, 1)
#define PCIE_DMA_CTL_DW_WR(be)			PCIE_DMA_CTL(4, be, PCIE_DMA_WR_MEM_DW, 0, 1)

/*
 * Some PCI config space register offset
 */
#define PCIE_CFG_OFFSET_BAR0			0x10
#define PCIE_CFG_OFFSET_BAR1			0x14

#define PCIE_STATUS2_RESET_COMPLETE		(1 << 5)

#define PCIE_BRIDGE_CTL_AER			(1 << 2)
#define PCIE_BRIDGE_CTL_ENDPOINT		(1 << 3)
#define PCIE_BRIDGE_ROOTPORT			(2 << 3)
#define PCIE_BRIDGE_BIG_ENDIAN			(1 << 7)

#ifdef PCIE_AHB_BIGENDIAN
#define PCIE_BRIDGE_ENDIAN_RP_AER		(PCIE_BRIDGE_BIG_ENDIAN | PCIE_BRIDGE_ROOTPORT | PCIE_BRIDGE_CTL_AER)
#else
#define PCIE_BRIDGE_ENDIAN_RP_AER		(PCIE_BRIDGE_ROOTPORT | PCIE_BRIDGE_CTL_AER)
#endif

/* Just copy whatever is in the verification code, improve later */
#ifdef CONFIG_PCIE_GEN2
#define PCIE_GEN2_CONF_DATA			0x10082020
//#define PCIE_ASPM_CONF_DATA			0x32492020
#define PCIE_ASPM_CONF_DATA			0x32493f3f
#define PCIE_AHBPCIE_TIMER_DATA			0x00000100
#define PCIE_GEN2_LINK_SPEED_5G			(1 << 28)
#define PCIE_GEN2_FTS_SEP_CLOCK			5
#else
#define PCIE_GEN2_CONF_DATA			0x00000000
//#define PCIE_GEN2_CONF_DATA			0x10003f3f
#define PCIE_ASPM_CONF_DATA			0x32490501
#define PCIE_AHBPCIE_TIMER_DATA			0x00000200
#endif

/* PCIe capability register offset */
#define PCIE_CFG_LINK_CTL_REG			0x90
#define PCIE_CFG_LINK_CTL_DATA			0x00000020

#define PCIE_PCIAHB_CTL(ahb_base, enable, no_prefetch, size) \
	((ahb_base & ~((1 << 12) - 1)) | (enable << 7) | (no_prefetch << 6) | size)
#define PCIE_AHBPCI_CTL_LOW(pci_base, no_prefetch, size) \
	((pci_base & ~((1 << 7) - 1)) | (no_prefetch << 6) | size)

#define PCIE_BR_ACCESS_NONE			0
#define PCIE_BR_ACCESS_CORE_REG			1
#define PCIE_BR_ACCESS_AHBPCI_WIN0		2
#define PCIE_BR_ACCESS_AHBPCI_WIN1		3

#else	/* CONFIG_UBICOM32_IP8K_SHUTTLE */
/*
 * Macros related to DMA control (P25 of the PLDA ref manual)
 * (size, byte_enable, command, stop, start)
 */
#define PCIE_REG_RD				0x00000002
#define PCIE_REG_WR				0x00000006

/*
 * Some PCI config space register offset
 */
#define PCIE_CFG_OFFSET_BAR0			0x10
#define PCIE_CFG_OFFSET_BAR1			0x14

//#define PCIE_STATUS2_RESET_COMPLETE		(1 << 1)

#define PCIE_BRIDGE_CTL_GEN2			(1 << 0)
#define PCIE_BRIDGE_CTL_CLKREQ			(1 << 1)
#define PCIE_BRIDGE_CTL_AER			(0x7 << 2)
#define PCIE_BRIDGE_CTL_MSI			(0x1 << 7)
#define PCIE_BRIDGE_BIG_ENDIAN			(1 << 13)
#define PCIE_BRIDGE_ENDPOINT			(1 << 14)
#define PCIE_BRIDGE_ROOTPORT			(2 << 14)

#ifdef PCIE_AXI_BIGENDIAN
#define PCIE_BRIDGE_ENDIAN_RP_AER		(0x00021001 | PCIE_BRIDGE_BIG_ENDIAN | PCIE_BRIDGE_ROOTPORT | PCIE_BRIDGE_CTL_AER | PCIE_BRIDGE_CTL_MSI)
#else
#define PCIE_BRIDGE_ENDIAN_RP_AER		(0x00021001 | PCIE_BRIDGE_ROOTPORT | PCIE_BRIDGE_CTL_AER | PCIE_BRIDGE_CTL_MSI)
#endif

/* Just copy whatever is in the verification code, improve later */
//Register Values for each registers
#define CORE_WRITTING_ALL_1              0xFFFFFFFF // Writting all 1's
#define CORE_WRITTING_ALL_0              0x00000000 // Writting all 0's
#define CORE_DEV_VEN_ID_VAL              0x000119A3 // DEV-0001, VEN-19A3
#define CORE_STS_CMD_REG_VAL             0x00000106 // CMD-Bus master enable, IO and MEM response and SERR enable and INTX is not disable
#define CORE_CLASS_CODE_REG_VAL          0x00112233 // Class Code
#define CORE_BASE_ADDR_0_VAL             0xC2000000 // Base address for BAR0
#define CORE_BASE_ADDR_1_VAL             0xC4004000 // BAR1 base addr
#define CORE_SUB_SEC_BUS_VAL             0x00010100 // Subordinate bus = 1, secondary bus = 1
#define CORE_SUB_SYS_ID_VAL              0x00FF0100 // Lat=00, Subord bus=FF, Sec bus nu=01 and Prim bus num =00
#define CORE_MSI_CAPABILITY_VAL          0x00FF0000 // 31-16 is for MSI Control- 64 bit ans MSI enable
#define CORE_MSI_MSG_ADDR_VAL            0xBBBB4000
#define CORE_MSI_MSG_UP_ADDR_VAL         0x00000000
#define CORE_MSI_MSG_DATA_VAL            0x00000001
#define CORE_MSIX_CAPABILITY_VAL         0x00000068
#define CORE_MSI_TABLE_OFFSET_VAL        0x0000006C
#define CORE_MSI_PBA_OFFSET_VAL          0x00000070
#define CORE_PWR_MNGMNT_STS_CNT_VAL      0x00000100 // Enabling PME support
#define CORE_PCIE_EXP_DEV_STS_CNT_VAL    0x0000102F // All err type enable, Max payload-2KB,
//#define CORE_PCIE_EXP_DEV_STS_CNT_VAL    0x0000408F // All err type enable, Max payload-2KB,
#define CORE_PCIE_EXP_LNK_STS_CNT_VAL    0x00000000 // Enabling L0s and L1 entry bit
#define CORE_PCIE_EXP_ROOT_CNT_VAL       0x0000000E // all errors and expect COR
#define CORE_PCIE_DEV_CNT_REG2_VAL       0x00000001 // 50us to 10 ms
#define CORE_VC_STATUS_CONTROL_VAL       0x00000002 // VC Arbitration- Defined by HW
#define CORE_VC0_CONTROL_REG_VAL         0x800100FF // TC0-7 mapped
#define CORE_VC1_CONTROL_REG_VAL         0x00000000 // VC1 enabled and TC0-7 are mapped  // 0'd mt 1/5
#define CORE_UC_ERR_MASK_REG_VAL         0x003FF030 // Enabling all kind of errors
#define CORE_UC_ERR_SEVR_REG_VAL         0x003FF030 // Enabling all err as Fatal err
#define CORE_CR_ERR_MASK_REG_VAL         0x00003FC1 // enabling all Correctable errors
#define CORE_ROOT_ERR_CAP_CNT_VAL        0x00000140 // Enabling ECRC Gen and check
#define CORE_ROOT_ERR_CMD_REG_VAL        0x00000007 // enabling Fatal, Non Fatal and Correctable errors
#define CORE_RC_LINK_CNT_REG_VAL         0x00000063
#define CORE_LTSSM_REG_VAL               0x00000001 // enabeling PME Ack
#define CORE_AER_ECRC_REG_VAL            0x00000007 // AER and ECRC ceck and Gen enable
#define CORE_VC1_CAP_REG_VAL             0x00000000 // VC1 is not enabled
#define CORE_MAX_PAYLOAD_REG_VAL         0x00000100 // 2KB Max payload
#define CORE_COMP_TIMEOUT_REG_VAL        0x00000001 // range 50us to 10ms
#define CORE_VC0_P_CREDIT_REG_VAL        0x00600006 // PH=6 and PD=96  // 8
#define CORE_VC0_NP_CREDIT_REG_VAL       0x00400006 // NPH=16 and NPD=64   // 8
#define CORE_AER_CAPABILITY_STRUC_VAL    0x00000140 // ECRC Gen and Check enable

#define CORE_KCONT_CONFIG0_REG_VAL       0x00000000 // rcomended value given is specs
#define CORE_KCONT_CONFIG1_REG_VAL       0x15600CC0 // rcomended value given is specs
#define CORE_KCONT_CONFIG2_REG_VAL       0x0001E55A // rcomended value given is specs
#define CORE_KCONT_CONFIG3_REG_VAL       0xC81DFFD6 // rcomended value given is specs
#define CORE_KCONT_CONFIG4_REG_VAL       0x00000000 // rcomended value given is specs
#define CORE_KCONT_CONFIG5_REG_VAL       0x00000000 // rcomended value given is specs

// Gen-2 pcie configuration register
#define CORE_PCIE_CONFIG0_REG_VAL        0x00000002 // Core compliant to Gen-2 and -6DB de-emph selection
#define CORE_PCIE_CONFIG1_REG_VAL        0x00000012 // Core compliant to Gen-2 and -6DB de-emph selection

#define CORE_PCIE_ASPML0_CAP_VAL         0x0F10000C // NFTS_SPCLK =NFTS_COMCLK= FF
#define CORE_PCIE_ASPML0_CAP_GEN2_VAL    0x0F100008 // EIE =8 and NFTS_SPCLK =NFTS_COMCLK= FF
#define CORE_PCIE_PM_MNGMT_CAP_VAL       0xFE000000 // EIE =8 and NFTS_SPCLK =NFTS_COMCLK= FF
#define CORE_PCIE_ASPML1_CAP_VAL         0x00010000 // L1 exit latency on common clock = less then 64 ns and separated clock = 128 ns

#define CORE_BRIDGE_CNT_REG_VAL          0x00030000 // Enabling parity err and SERR

#define PCIE_BR_ACCESS_CORE_REG			0	// PCIe core APB slave
#define PCIE_BR_ACCESS_PCIE_REG			1	// PCIe AXI slave

#endif	/* CONFIG_UBICOM32_IP8K_SHUTTLE */

/*
 * Configuration space read / write may fail during enumeration phase. We are not sure
 * if the controller will set PCI or AHB error in this case now, so set a limit for now.
 */
#define PCIE_DMA_MAX_LOOP 4000

#define PCI_VENDOR_ID_UBICOM  0x6563
#define PCIE_RC_UBICOM_DEV_ID 0x01

/*
 * pcie_nbr_readl
 *	Read a 32 bit value from the non-blocking-region
 */
static inline u32 pcie_nbr_readl(volatile u32 * addr)
{
	volatile u32 val = *addr;
	//PCIE_TRACE("\tread from NBR reg %p, val=%x\n", addr, val);
	return val;
}

/*
 * pcie_nbr_writel
 *	Write a 32 bit value into non-blocking-region
 */
static inline void pcie_nbr_writel(volatile u32 * addr, u32 val)
{
	//PCIE_TRACE("\twrite %x to NBR reg %p\n", val, addr);
	*addr = val;
}

/*
 * The first tape-out has a bug in blocking region access when doing blocking
 * region access directly through DRAM. The workaround is to use a register or
 * OCM and wait for 5 cycles. The limitation will be gone in the production
 * release.
 */

/*
 * BR read needs dst to be a non-DDR address
 *
 * NOTE: this function should not be called directly without selecting the BR region
 * using ctl->ctl1
 */
static inline u32 pcie_br_readl(struct pcie_port *pp, volatile u32 * addr)
{
	u32 ret;

#ifdef CONFIG_SMP
	PCIE_ASSERT(((in_irq() || irqs_disabled()) && spin_is_locked(&pp->conf_lock)),
		"PCIe BR access unlocked\n");
#endif

	asm volatile (
#ifdef PCIE_SWAP_ENDIAN
		" swapb.4	%[ret], 0(%[addr])		\n"
#else
		" move.4	%[ret], 0(%[addr])		\n"
#endif
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
		" cycles	"D(PCIE_BR_ACCESS_DELAY)"	\n"
#endif
		: [ret] "=d" (ret)
		: [addr] "a" (addr)
	);
	//PCIE_TRACE("\tread from BR reg %p, val=%x\n", addr, ret);
	return ret;
}

/*
 * BR write needs dst to be a non-DDR address
 *
 * NOTE: this function should not be called directly without selecting the BR region
 * using ctl->ctl1
 */
static inline void pcie_br_writel(struct pcie_port *pp, volatile u32 * addr, u32 val)
{
#ifdef CONFIG_SMP
	PCIE_ASSERT(((in_irq() || irqs_disabled()) && spin_is_locked(&pp->conf_lock)),
		"PCIe BR access unlocked\n");
#endif

	asm volatile (
#ifdef PCIE_SWAP_ENDIAN
		" swapb.4	0(%[addr]), %[val]		\n"
#else
		" move.4	0(%[addr]), %[val]		\n"
#endif
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
		" cycles	"D(PCIE_BR_ACCESS_DELAY)"	\n"
#endif
		:
		: [addr] "a" (addr), [val] "d" (val)
	);
	//PCIE_TRACE("write BR reg %p, value=%x\n", addr, val);
}

/*
 * Make sure config space is valid
 */
static inline int pcie_valid_config(struct pcie_port *pp, int bus, int dev)
{
	/*
	 * The root complex bridge includes bus 0 and 1 which has only device 0 (point-to-point).
	 * Don't go out when trying to access nonexisting devices on these two buese.
	 */
	if (bus <= 1 && dev > 0)
		return 0;

	return 1;
}

extern struct pcie_port pcie_ports[PCIE_PORT_MAX];
extern void __init pcie_phy_config(struct ubicom32_io_port *phy);

extern u32 __pcie_cfg_writel(struct pcie_port *pp, u32 cfg_addr, u32 val);
extern u32 __pcie_cfg_readl(struct pcie_port *pp, u32 cfg_addr, u32 *val);

#endif

