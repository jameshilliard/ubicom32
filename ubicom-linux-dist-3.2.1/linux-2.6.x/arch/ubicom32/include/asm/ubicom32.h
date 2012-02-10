/*
 * arch/ubicom32/include/asm/ubicom32.h
 *   Specific details for the UBICOM32 processor.
 *
 * (C) Copyright 2009-2011, Ubicom, Inc.
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
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#ifndef _ASM_UBICOM32_UBICOM32_H
#define _ASM_UBICOM32_UBICOM32_H

/*
 * Memory Size
 */
#define OCM_SECTOR_SIZE	0x00008000		/* 32K */

#if defined(IP5000)
# define OCMSIZE	0x00030000	/* 192K on-chip RAM for both program and data */
# define OCMSTART	0x3ffc0000	/* alias from 0x03000000 for easy jump to/from SDRAM */
# define SDRAMSTART	0x40000000
# define FLASHSTART	0x60000000
#elif defined(IP7000)
# define OCMSIZE	0x0003C000	/* 240K on-chip RAM for both program and data */
# define OCMSTART	0x3ffc0000	/* alias from 0x03000000 for easy jump to/from SDRAM */
# define SDRAMSTART	0x40000000
# define FLASHSTART	0x60000000
#elif defined(IP8000)
# define OCMSIZE	0x00040000	/* 256K on-chip RAM for both program and data */
# define OCMSTART	0xbffc0000
# define SDRAMSTART	0xc0000000
# define FLASHSTART	0xb0000000
# define VIRTUALSTART	0x00000000
# define VIRTUALEND	0xafffffff
#else
# error "Unknown UBICOM32 silicon"
#endif

#define OCMEND		(OCMSTART + OCMSIZE)

#define KERNELSTART	(SDRAMSTART + 0x00100000)

/*
 * CODELOADER / OS_SYSCALL OCM Reservations
 * Don't change these unless you know what you are doing.
 */
#define CODELOADER_SIZE  0x30
#define CODELOADER_BEGIN OCMSTART	/* Must be at OCM start for gdb to work. */
#define CODELOADER_END	 (CODELOADER_BEGIN + CODELOADER_SIZE)

#define OS_SYSCALL_BEGIN CODELOADER_END		/* system_call at this address */
#define OS_SYSCALL_SIZE  (512 - CODELOADER_SIZE)
#define OS_SYSCALL_END	 (OS_SYSCALL_BEGIN + OS_SYSCALL_SIZE)

/*
 * Inline assembly define
 */
#define S(arg) #arg
#define D(arg) S(arg)

/*
 * Assembler include file
 */
#include <asm/ubicom32-asm.h>

/*
 * Threads
 */
#if defined(IP5000)
# define THREAD_COUNT 10
#elif defined(IP7000) || defined(IP8000)
# define THREAD_COUNT 12
#else
# error "Unknown UBICOM32 silicon"
#endif

#define ALL_THREADS ((1 << THREAD_COUNT)-1)
#define NO_THREADS (0)

/*
 * Interrupts
 */
#if defined(IP5000) || defined(IP7000)
# define INTERRUPT_COUNT 64
# define SW_INTERRUPT_COUNT 24
#elif defined(IP8000)
# define INTERRUPT_COUNT 96
# define SW_INTERRUPT_COUNT 29
#endif

#define INT_CHIP(reg, bit)	(((reg) << 5) | (bit))
#define INT_REG(interrupt)	(((interrupt) >> 5) * 4)
#define INT_SET(interrupt)	0x0114 + INT_REG(interrupt)
#define INT_CLR(interrupt)	0x0124 + INT_REG(interrupt)
#define INT_STAT(interrupt)	0x0104 + INT_REG(interrupt)
#define INT_MASK(interrupt)	0x00C0 + INT_REG(interrupt)
#define INT_BIT(interrupt)	((interrupt) & 0x1F)
#define INT_BIT_MASK(interrupt) (1 << INT_BIT(interrupt))

/*
 * GLOBAL_CTRL
 */
#define GLOBAL_CTRL_HIPRI_DMA_EN	(1 << 10)	/* high priority DMA (IP8000+ only) */
#define GLOBAL_CTRL_TRAP_RST_EN		(1 << 9)
#define GLOBAL_CTRL_AERROR_RST_EN	(1 << 8)
#define GLOBAL_CTRL_MT_MIN_DELAY(x)	(((x) & 0xf) << 3)
#define GLOBAL_CTRL_HRT_BANK_SELECT	(1 << 2)
#define GLOBAL_CTRL_INT_EN		(1 << 0)

/*
 * HRT Tables
 */
#if defined(IP5000) || defined(IP7000)
# define HRT_TABLE0_BASE	0x0800
# define HRT_TABLE1_BASE	0x0900
# define HRT_TABLE_SIZE		64
#elif defined(IP8000)
# define HRT_TABLE0_BASE	0xb8000800
# define HRT_TABLE1_BASE	0xb8000900
# define HRT_TABLE_SIZE		64
# define NRT_TABLE_BASE		0xb8000a00
# define NRT_TABLE_SIZE		32
#endif

/*
 * Break Point Trap Register
 */
#define ASYNCERROR_INT INT_CHIP(0, 31)
#define BREAKPOINT_INT INT_CHIP(1, 31)

#if defined(IP5000) || defined(IP7000)
/*
 * Port interrupts for IP5000/IP7000
 *	The non-existing FIFO INTs are mapped to INT2 for the ports.
 */
# define IO_PORT_PTR_TO_NUM(port) (((port) & 0x0000ffff) >> 12)
# define RX_FIFO_INT(port) \
	((IO_PORT_PTR_TO_NUM(port) == 0) ? INT_CHIP(0, 25) : \
	((IO_PORT_PTR_TO_NUM(port) == 1) ? INT_CHIP(0, 26) : \
	((IO_PORT_PTR_TO_NUM(port) == 2) ? INT_CHIP(0, 29) : \
	((IO_PORT_PTR_TO_NUM(port) == 3) ? INT_CHIP(1, 24) : \
	((IO_PORT_PTR_TO_NUM(port) == 4) ? INT_CHIP(1, 27) : \
	((IO_PORT_PTR_TO_NUM(port) == 5) ? INT_CHIP(1, 16) : \
	((IO_PORT_PTR_TO_NUM(port) == 6) ? INT_CHIP(1, 19) : \
	((IO_PORT_PTR_TO_NUM(port) == 7) ? INT_CHIP(1, 20) : \
	((IO_PORT_PTR_TO_NUM(port) == 8) ? INT_CHIP(1, 21) : \
	INT_CHIP(1, 15))))))))))
# define TX_FIFO_INT(port) \
	((IO_PORT_PTR_TO_NUM(port) == 0) ? INT_CHIP(0, 24) : \
	((IO_PORT_PTR_TO_NUM(port) == 1) ? INT_CHIP(0, 27) : \
	((IO_PORT_PTR_TO_NUM(port) == 2) ? INT_CHIP(0, 29) : \
	((IO_PORT_PTR_TO_NUM(port) == 3) ? INT_CHIP(1, 25) : \
	((IO_PORT_PTR_TO_NUM(port) == 4) ? INT_CHIP(1, 28) : \
	((IO_PORT_PTR_TO_NUM(port) == 5) ? INT_CHIP(1, 17) : \
	((IO_PORT_PTR_TO_NUM(port) == 6) ? INT_CHIP(1, 19) : \
	((IO_PORT_PTR_TO_NUM(port) == 7) ? INT_CHIP(1, 20) : \
	((IO_PORT_PTR_TO_NUM(port) == 8) ? INT_CHIP(1, 22) : \
	INT_CHIP(1, 15))))))))))
# define PORT_OTHER_INT(port) \
	((IO_PORT_PTR_TO_NUM(port) == 0) ? INT_CHIP(0, 25) : \
	((IO_PORT_PTR_TO_NUM(port) == 1) ? INT_CHIP(0, 28) : \
	((IO_PORT_PTR_TO_NUM(port) == 2) ? INT_CHIP(0, 29) : \
	((IO_PORT_PTR_TO_NUM(port) == 3) ? INT_CHIP(1, 26) : \
	((IO_PORT_PTR_TO_NUM(port) == 4) ? INT_CHIP(1, 29) : \
	((IO_PORT_PTR_TO_NUM(port) == 5) ? INT_CHIP(1, 18) : \
	((IO_PORT_PTR_TO_NUM(port) == 6) ? INT_CHIP(1, 19) : \
	((IO_PORT_PTR_TO_NUM(port) == 7) ? INT_CHIP(1, 20) : \
	((IO_PORT_PTR_TO_NUM(port) == 8) ? INT_CHIP(1, 23) : \
	INT_CHIP(1, 15))))))))))
#elif defined(IP8000)
/*
 * Port interrupts for IP8000 - listed individually
 */
# define USB30PCIE_MSI_INT	INT_CHIP(1, 12)	// 12-15 are reserved
# define SHA2_INT		INT_CHIP(1, 29)
# define IDCT_INT		INT_CHIP(1, 28)
# define EXT3_INT		INT_CHIP(1, 26)
# define EXT2_INT		INT_CHIP(1, 25)
# define EXT1_INT		INT_CHIP(1, 24)
# define EXT0_INT		INT_CHIP(1, 23)
# define RV_I2S_INT		INT_CHIP(1, 18)
# define RW_USB30_INT		INT_CHIP(1, 17)
# define RW_USB30_PWM_INT	INT_CHIP(1, 16)
# define RK_USB30PCIE_INT	INT_CHIP(2, 31)
# define RK_USB30PCIE_PWM_INT	INT_CHIP(2, 30)
# define RJ_USB30PCIE_INT	INT_CHIP(2, 29)
# define RJ_USB30PCIE_PWM_INT	INT_CHIP(2, 28)
# define RC_PLIO_INT		INT_CHIP(2, 27)
# define RC_PLIO_TX_INT		INT_CHIP(2, 26)
# define RC_PLIO_RX_INT		INT_CHIP(2, 25)
# define RB_PLIO_INT		INT_CHIP(2, 24)
# define RB_PLIO_TX_INT		INT_CHIP(2, 23)
# define RB_PLIO_RX_INT		INT_CHIP(2, 22)
# define RP_SERDES_INT		INT_CHIP(2, 21)
# define RP_SERDES_RX_INT	INT_CHIP(2, 20)
# define RO_SERDES_INT		INT_CHIP(2, 19)
# define RO_SERDES_RX_INT	INT_CHIP(2, 18)
# define RE_SDIO_INT		INT_CHIP(2, 17)
# define RE_SDIO_TX_INT		INT_CHIP(2, 16)
# define RE_SDIO_RX_INT		INT_CHIP(2, 15)
# define RD_SDIO_INT		INT_CHIP(2, 14)
# define RD_SDIO_TX_INT		INT_CHIP(2, 13)
# define RD_SDIO_RX_INT		INT_CHIP(2, 12)
# define RI_GMAC_INT		INT_CHIP(2, 11)
# define RI_GMAC_PWM_INT	INT_CHIP(2, 10)
# define RH_GMAC_INT		INT_CHIP(2, 9)
# define RH_GMAC_PWM_INT	INT_CHIP(2, 8)
# define RG_USB20_INT		INT_CHIP(2, 7)
# define RF_USB20_INT		INT_CHIP(2, 6)
# define RT_PWM_INT		INT_CHIP(2, 5)
# define RS_PWM_INT		INT_CHIP(2, 4)
# define RR_PWM_INT		INT_CHIP(2, 3)
# define RQ_PWM_INT		INT_CHIP(2, 2)
# define RA_FLASH_INT		INT_CHIP(2, 1)
# define RA_FLASH_TX_INT	INT_CHIP(2, 0)

# define IO_PORT_PTR_TO_NUM(port) ((port - IO_PORT_RU) >> 5)
#endif

/*
 * On Chip Peripherals Base.
 */
#if defined(IP5000) || defined(IP7000)
# define OCP_BASE	0x01000000
#elif defined(IP8000)
# define OCP_BASE	0xb9000000
#endif

#define OCP_GENERAL	0x000
#define OCP_TIMERS	0x100
/*#define OCP_TRNG	0x200	   True Random Number Generator Control Reigsters */
#define OCP_DEBUG	0x300
#define OCP_SECURITY	0x400
#define OCP_ICCR	0x500	/* I-Cache Control Registers */
#define OCP_DCCR	0x600	/* D-Cache Control Registers */
#define OCP_OCMC	0x700	/* On Chip Memory Control Registers */
#define OCP_STATISTICS	0x800	/* Statistics Counters */
#define OCP_MTEST	0x900	/* Memory Test Registers */
#define OCP_MCFG	0xa00	/* Memory Configuration Registers -- IP7000+ */
#define OCP_IDCT	0xb00	/* HW IDCT block -- IP7000+ */
#define OCP_PIXP	0xd00	/* HW pixel accelerator -- IP7000+ */
#define OCP_MMU		0xe00	/* MMU -- IP8000+ */
#define OCP_DEBUG_INST	0x000	/* Up to 16M */

/*
 * General Configuration Registers (PLL)
 */
#define GENERAL_CFG_BASE (OCP_BASE + OCP_GENERAL)
#define GEN_CLK_CORE_CFG	0x00
#define GEN_CLK_IO_CFG		0x04
#define GEN_CLK_DDR_CFG		0x08
#if defined(IP5000) || defined(IP7000)
#define GEN_CLK_DDRDS_CFG	0x0c
#elif defined(IP8000)
#define GEN_CLK_I2S_CFG		0x0c
#endif
#define GEN_CLK_SLIP_CLR	0x10
#define GEN_CLK_SLIP_STATUS	0x14
#if defined(IP5000) || defined(IP7000)
# define GEN_CLK_SERDES_SEL	0x18	/* IP7000 only */
# define GEN_CLK_DDR_CFG2 	0x1c	/* IP7000 only */
# define GEN_DDR_CAL_CTRL 	0x30	/* IP5000 only */
# define GEN_DDR_CAL_STAT 	0x34	/* IP5000 only */
# define GEN_USB_DFT_CTRL 	0x38	/* IP5000 only */
# define GEN_USB_DFT_STAT 	0x3c	/* IP5000 only */
# define GEN_USB_PHY_CFG 	0x40	/* IP7000 only */
# define GEN_USB_PHY_TEST 	0x44	/* IP7000 only */
# define GEN_USB_PHY_STAT 	0x48	/* IP7000 only */
#elif defined(IP8000)
# define GEN_CLK_IO_ENABLE	0x18	/* IP8000 only: PIXL/iDCT clock enable */
# define GEN_CLK_HSSIO_CONFIG	0x1c	/* IP8000 production only: High speed I/O clock config */
# define GEN_CLK_IO_SELECT	0x20	/* IP8000 only: I/O per-block clock selection */
# define GEN_CLK_IO_DISABLE	0x24	/* IP8000 only: I/O per-block clock disable */
# define GEN_CLK_CORE_CFG2	0x28	/* IP8000 only */
# define GEN_CLK_IO_CFG2	0x2c	/* IP8000 only */
# define GEN_CLK_DDR_CFG2	0x38	/* IP8000 only */
#endif
#define GEN_SW_RESET		0x80
#define GEN_RESET_REASON	0x84
#define GEN_BOND_CFG		0x88
#if defined(IP5000) || defined(IP7000)
#define GEN_IO_PU_CFG		0x8c
#define GEN_MEM_RM_CFG		0x90
#define GEN_IO_CONFIG		0x94
#endif

#if defined(IP8000)
/*
 * GEN_CLK_IO_DISABLE bits:
 */
#define GEN_CLK_DISABLE_DDR_COMPENSATION	(1 << 21)
#define GEN_CLK_DISABLE_MPT			(1 << 20)
#define GEN_CLK_DISABLE_SD1			(1 << 19)
#define GEN_CLK_DISABLE_SD0			(1 << 18)
#define GEN_CLK_DISABLE_SERDES1			(1 << 17)
#define GEN_CLK_DISABLE_SERDES0			(1 << 16)
#define GEN_CLK_DISABLE_PLIO1			(1 << 15)
#define GEN_CLK_DISABLE_PLIO0			(1 << 14)
#define GEN_CLK_DISABLE_USB2_1			(1 << 13)
#define GEN_CLK_DISABLE_USB2_0			(1 << 12)
#define GEN_CLK_DISABLE_GMAC2_50		(1 << 11)
#define GEN_CLK_DISABLE_GMAC1_50		(1 << 10)
#define GEN_CLK_DISABLE_GMAC2_62_5		(1 << 9)
#define GEN_CLK_DISABLE_GMAC1_62_5		(1 << 8)
#define GEN_CLK_DISABLE_GMAC2_125		(1 << 7)
#define GEN_CLK_DISABLE_GMAC1_125		(1 << 6)
#define GEN_CLK_DISABLE_GMAC2_HCLK		(1 << 5)
#define GEN_CLK_DISABLE_GMAC1_HCLK		(1 << 4)
#define GEN_CLK_DISABLE_PCIE1_PHY		(1 << 3)
#define GEN_CLK_DISABLE_PCIE0_PHY		(1 << 2)
#define GEN_CLK_DISABLE_PCIE1_CTL		(1 << 1)
#define GEN_CLK_DISABLE_PCIE0_CTL		(1 << 0)
#endif

#define GEN_CLK_PLL_SECURITY_BIT_NO	31
#define GEN_CLK_PLL_SECURITY		(1 << GEN_CLK_PLL_SECURITY_BIT_NO)
#define GEN_CLK_PLL_ENSAT		(1 << 30)
#define GEN_CLK_PLL_FASTEN		(1 << 29)
#define GEN_CLK_PLL_NR(v)		(((v) - 1) << 23)
#define GEN_GET_CLK_PLL_NR(v)		((((v) >> 23) & 0x003f) + 1)

#if defined(IP5000) || defined(IP7000)
# define OSC1_FREQ		12000000
# define GEN_CLK_MPT_FREQ	OSC1_FREQ
# define GEN_CLK_PLL_NF(v)	(((v) - 1) << 11)
# define GEN_GET_CLK_PLL_NF(v)	((((v) >> 11) & 0x0fff) + 1)
# define GEN_CLK_PLL_OD(v)	(((v) - 1) << 8)
# define GEN_GET_CLK_PLL_OD(v)	((((v) >> 8) & 0x7) + 1)
# define GEN_CLK_PLL_SLOWDOWN	0
# define GEN_PLL_OD_VALUE	1
# define DDR_PLL_OD_VALUE	(2 * GEN_PLL_OD_VALUE)	/* a hidden div-by-2 */
#elif defined(IP8000)
# define OSC1_FREQ		48000000
# define GEN_CLK_MPT_FREQ	(OSC1_FREQ / 16)
# define GEN_CLK_PLL_NF(v)	((2 * (v) - 1) << 10)
# define GEN_GET_CLK_PLL_NF(v)	(((((v) >> 10) & 0x1fff) + 1) / 2)
# define GEN_CLK_PLL_OD(v)	((16 / (v)) & 0x7)
# define GEN_GET_CLK_PLL_OD(v)	(16 / ((((v) - 1) & 0x7) + 1))
# define GEN_CLK_PLL_AUX_REF()	(1 << 8)
# define GEN_CLK_PLL_SLOWDOWN	2
# define GEN_PLL_OD_VALUE	2	/* a hidden div-by-2 */
# define DDR_PLL_OD_VALUE	(1 * GEN_PLL_OD_VALUE)
#endif

#define GEN_CLK_PLL_RESET	(1 << 7)
#define GEN_CLK_PLL_BYPASS	(1 << 6)
#define GEN_CLK_PLL_POWERDOWN	(1 << 5)
#define GEN_CLK_PLL_SELECT	(1 << 4)

#define RESET_FLAG_FPU_INVALID		(1 << 25)
#define RESET_FLAG_FPU_DIV_ZERO		(1 << 24)
#define RESET_FLAG_FPU_OVERFLOW		(1 << 23)
#define RESET_FLAG_FPU_UNDERFLOW	(1 << 22)
#define RESET_FLAG_FPU_INEXACT		(1 << 21)
#define RESET_FLAG_PRIV_VIOLATION	(1 << 20)
#define RESET_FLAG_DST_MEM_ERROR	(1 << 19)
#define RESET_FLAG_SRC1_MEM_ERROR	(1 << 18)
#define RESET_FLAG_INST_MEM_ERROR	(1 << 17)
#define RESET_FLAG_WRITE_ADDR		(1 << 16)
#define RESET_FLAG_DST_SYNC_ERROR	(1 << 15)
#define RESET_FLAG_SRC1_SYNC_ERROR	(1 << 14)
#define RESET_FLAG_DST_ALGN_ERROR	(1 << 13)
#define RESET_FLAG_SRC1_ALGN_ERROR	(1 << 12)
#define RESET_FLAG_DST_ADDR_ERROR	(1 << 11)
#define RESET_FLAG_SRC1_ADDR_ERROR	(1 << 10)
#define RESET_FLAG_ILLEGAL_INST		(1 << 9)
#define RESET_FLAG_INST_SYNC_ERROR	(1 << 8)
#define RESET_FLAG_INST_ADDR_ERROR	(1 << 7)
#define RESET_FLAG_DATA_PORT_ERROR	(1 << 6)
#define RESET_FLAG_INST_PORT_ERROR	(1 << 5)
#define RESET_FLAG_SW_RESET		(1 << 4)
#define RESET_FLAG_DEBUG		(1 << 3)
#define RESET_FLAG_WATCHDOG		(1 << 2)
#define RESET_FLAG_POWER_ON		(1 << 1)
#define RESET_FLAG_EXTERNAL		(1 << 0)

/*
 * Timer block
 */
#define TIMER_BASE		(OCP_BASE + OCP_TIMERS)
#define TIMER_MPTVAL		0x00
#define TIMER_RTCOM		0x04
#define TIMER_TKEY		0x08
#define TIMER_WDCOM		0x0c
#define TIMER_WDCFG		0x10
#define TIMER_SYSVAL		0x14
#define TIMER_SYSCOM(tmr)	(0x18 + (tmr) * 4)
#define TIMER_TRN_CFG		0x100
#define TIMER_TRN		0x104

#if defined(IP5000) || defined(IP7000)
# define TIMER_COUNT 10
#elif defined(IP8000)
# define TIMER_COUNT 12
#endif

#define REALTIME_TIMER_INT		INT_CHIP(0, 30)
#define TIMER_INT(tmr)			INT_CHIP(1, (tmr))
#define TIMER_TKEYVAL			0xa1b2c3d4
#define TIMER_WATCHDOG_DISABLE		0x4d3c2b1a
#define TIMER_TRN_CFG_ENABLE_OSC	0x00000007

#ifndef __ASSEMBLY__
/*
 * ubicom32_io_timer
 */
struct ubicom32_io_timer {
	volatile u32_t mptval;
	volatile u32_t rtcom;
	volatile u32_t tkey;
	volatile u32_t wdcom;
	volatile u32_t wdcfg;
	volatile u32_t sysval;
	volatile u32_t syscom[TIMER_COUNT];
	volatile u32_t reserved[64 - 6 - TIMER_COUNT];	// skip all the way to OCP-TRNG section
	volatile u32_t rsgcfg;
	volatile u32_t trn;
};

#define UBICOM32_IO_TIMER ((struct ubicom32_io_timer *)TIMER_BASE)
#endif

#define UBICOM32_VECTOR_TO_TIMER_INDEX(vector) (vector - TIMER_INT(0))

/*
 * OCP-Debug Module (Mailbox)
 */
#define ISD_MAILBOX_BASE	(OCP_BASE + OCP_DEBUG)
#define ISD_MAILBOX_IN		0x00
#define ISD_MAILBOX_OUT		0x04
#define ISD_MAILBOX_STATUS	0x08

#define ISD_MAILBOX_INT		INT_CHIP(1, 30)

#define ISD_MAILBOX_STATUS_IN_FULL	(1 << 31)
#define ISD_MAILBOX_STATUS_IN_EMPTY	(1 << 30)
#define ISD_MAILBOX_STATUS_OUT_FULL	(1 << 29)
#define ISD_MAILBOX_STATUS_OUT_EMPTY	(1 << 28)

/*
 * OCP-Security
 */
#define SECURITY_BASE			(OCP_BASE + OCP_SECURITY)
#define SECURITY_CTRL			0x00
#define SECURITY_CTRL_BYTE_OFFSET(x)	((x) << 16)
#define SECURITY_CTRL_KEY_SIZE(x)	((x) << 8)
#define SECURITY_CTRL_HASH_ALG_NONE	(0 << 4)
#define SECURITY_CTRL_HASH_ALG_MD5	(1 << 4)
#define SECURITY_CTRL_HASH_ALG_SHA1	(2 << 4)
#define SECURITY_CTRL_CBC		(1 << 3)
#define SECURITY_CTRL_CIPHER_ALG_AES	(0 << 1)
#define SECURITY_CTRL_CIPHER_ALG_NONE	(1 << 1)
#define SECURITY_CTRL_CIPHER_ALG_DES	(2 << 1)
#define SECURITY_CTRL_CIPHER_ALG_3DES	(3 << 1)
#define SECURITY_CTRL_ENCIPHER		(1 << 0)
#define SECURITY_CTRL_DECIPHER		(0 << 0)
#define SECURITY_STAT			0x04
#define SECURITY_STAT_BUSY		(1 << 0)
#define SECURITY_KEY_VALUE(x)		(0x10 + (x) * 4)
#define SECURITY_KEY_IN(x)		(0x30 + (x) * 4)
#define SECURITY_KEY_OUT(x)		(0x50 + (x) * 4)
#define SECURITY_KEY_HASH(x)		(0x70 + (x) * 4)

#define UBI32_SECURITY_SHA1_STAT		0x4
#define UBI32_SECURITY_SHA1_STAT_BUSY_BIT	0

/*
 * SHA2 control register bit definitions.
 */
#define UBI32_SECURITY_SHA2_CTRL_SHA_224			(0 << 4)
#define UBI32_SECURITY_SHA2_CTRL_SHA_256			(1 << 4)
#define UBI32_SECURITY_SHA2_CTRL_SHA_384			(2 << 4)
#define UBI32_SECURITY_SHA2_CTRL_SHA_512			(3 << 4)
#define UBI32_SECURITY_SHA2_CTRL_DONE_INTERRUPT_ENABLE		(1 << 6)
#define UBI32_SECURITY_SHA2_CTRL_READY_INTERRUPT_ENABLE		(1 << 7)
#define UBI32_SECURITY_SHA2_CTRL_CLR_OVERFLOW_ERROR		(1 << 8)
#define UBI32_SECURITY_SHA2_CTRL_INPUT_DATA_OFFSET(offset)	((offset) << 16)
#define UBI32_SECURITY_SHA2_CTRL_RESET				(0xAB << 24)
#define UBI32_SECURITY_SHA2_CTRL				0x8

/*
 * SHA2 status register bit definitions.
 */
#define UBI32_SECURITY_SHA2_STAT_BUSY				(1 << 0)
#define UBI32_SECURITY_SHA2_STAT_INQUEUE_FULL			(1 << 1)
#define UBI32_SECURITY_SHA2_STAT_INQUEUE_OVERFLOW		(1 << 2)
#define UBI32_SECURITY_SHA2_STAT_BUSY_BIT			0
#define UBI32_SECURITY_SHA2_STAT_INQUEUE_FULL_BIT		1
#define UBI32_SECURITY_SHA2_STAT_INQUEUE_OVERFLOW_BIT		2
#define UBI32_SECURITY_SHA2_STAT				0xc

/*
 * Secret Key Input registers.
 */
#define UBI32_SECURITY_SEC_KEY_0	0x10
#define UBI32_SECURITY_SEC_KEY_1	0x14
#define UBI32_SECURITY_SEC_KEY_2	0x18
#define UBI32_SECURITY_SEC_KEY_3	0x1c
#define UBI32_SECURITY_SEC_KEY_4	0x20
#define UBI32_SECURITY_SEC_KEY_5	0x24
#define UBI32_SECURITY_SEC_KEY_6	0x28
#define UBI32_SECURITY_SEC_KEY_7	0x2C

/*
 * SHA1/AES/DES input registers.
 */
#define UBI32_SECURITY_SHA1_IN_0	0x30
#define UBI32_SECURITY_SHA1_IN_1	0x34
#define UBI32_SECURITY_SHA1_IN_2	0x38
#define UBI32_SECURITY_SHA1_IN_3	0x3C
#define UBI32_SECURITY_SHA1_IN_4	0x40

/*
 * Secret Ouput registers.
 */
#define UBI32_SECURITY_SEC_OUT_0	0x50
#define UBI32_SECURITY_SEC_OUT_1	0x54
#define UBI32_SECURITY_SEC_OUT_2	0x58
#define UBI32_SECURITY_SEC_OUT_3	0x5C
#define UBI32_SECURITY_SEC_OUT_4	0x60

/*
 * Hash Ouput registers.
 */
#define UBI32_SECURITY_SHA1_HASH_OUT_0	0x70
#define UBI32_SECURITY_SHA1_HASH_OUT_1	0x74
#define UBI32_SECURITY_SHA1_HASH_OUT_2	0x78
#define UBI32_SECURITY_SHA1_HASH_OUT_3	0x7C
#define UBI32_SECURITY_SHA1_HASH_OUT_4	0x80

/*
 * SHA2 Input registers
 */
#define UBI32_SECURITY_SHA2_IN_0	0x90
#define UBI32_SECURITY_SHA2_IN_1	0x94
#define UBI32_SECURITY_SHA2_IN_2	0x98
#define UBI32_SECURITY_SHA2_IN_3	0x9C
#define UBI32_SECURITY_SHA2_IN_4	0xA0

/*
 * SHA-512, SHA-384 hash_in/hash_out high registers:
 */
#define UBI32_SECURITY_SHA2_HASH_HI_0	0xA4
#define UBI32_SECURITY_SHA2_HASH_HI_1	0xA8
#define UBI32_SECURITY_SHA2_HASH_HI_2	0xAC
#define UBI32_SECURITY_SHA2_HASH_HI_3	0xB0
#define UBI32_SECURITY_SHA2_HASH_HI_4	0xB4
#define UBI32_SECURITY_SHA2_HASH_HI_5	0xB8
#define UBI32_SECURITY_SHA2_HASH_HI_6	0xBC
#define UBI32_SECURITY_SHA2_HASH_HI_7	0xC0

/*
 * SHA-256, SHA-224 hash_in/hash_out registers:
 */
#define UBI32_SECURITY_SHA2_HASH_LO_0	0xC4
#define UBI32_SECURITY_SHA2_HASH_LO_1	0xC8
#define UBI32_SECURITY_SHA2_HASH_LO_2	0xCC
#define UBI32_SECURITY_SHA2_HASH_LO_3	0xD0
#define UBI32_SECURITY_SHA2_HASH_LO_4	0xD4
#define UBI32_SECURITY_SHA2_HASH_LO_5	0xD8
#define UBI32_SECURITY_SHA2_HASH_LO_6	0xDC
#define UBI32_SECURITY_SHA2_HASH_LO_7	0xE0

/*
 * OCP-ICCR
 */
#define ICCR_BASE (OCP_BASE + OCP_ICCR)
#if defined(IP5000) || defined(IP7000)
# define ICACHE_TOTAL_SIZE 16384			/* in bytes */
#elif defined(IP8000)
# define ICACHE_TOTAL_SIZE 65536			/* in bytes */
#endif

/*
 * OCP-DCCR
 */
#define DCCR_BASE (OCP_BASE + OCP_DCCR)
#if defined(IP5000)
# define DCACHE_TOTAL_SIZE 8192				/* in bytes */
#elif defined(IP7000)
# define DCACHE_TOTAL_SIZE 16384			/* in bytes */
#elif defined(IP8000)
# define DCACHE_TOTAL_SIZE 65536			/* in bytes */
#endif

#define DCACHE_WRITE_QUEUE_LENGTH 6

#define CACHE_LINE_SHIFT	5
#define CACHE_LINE_SIZE		(1 << CACHE_LINE_SHIFT)	/* in bytes */

#define CCR_ADDR	0x00
#define CCR_RDD		0x04
#define CCR_WRD		0x08
#define CCR_STAT	0x0c
#define CCR_CTRL	0x10
#define CCR_RTIG	0x14				/* IP7000+ only */

#define CCR_STAT_MCBE	0
#define CCR_STAT_WIDEL	1				/* D-cache only */

#define CCR_CTRL_DONE 			0
#define CCR_CTRL_RESET 			2
#define CCR_CTRL_VALID 			3
#define CCR_CTRL_RD_DATA		(1 << 4)
#define CCR_CTRL_RD_TAG			(2 << 4)
#define CCR_CTRL_WR_DATA		(3 << 4)
#define CCR_CTRL_WR_TAG			(4 << 4)
#define CCR_CTRL_INV_INDEX		(5 << 4)
#define CCR_CTRL_INV_ADDR		(6 << 4)
#define CCR_CTRL_FLUSH_INDEX		(7 << 4)	/* D-cache only */
#define CCR_CTRL_FLUSH_INV_INDEX	(8 << 4)	/* D-cache only */
#define CCR_CTRL_FLUSH_ADDR		(9 << 4)	/* D-cache only */
#define CCR_CTRL_FLUSH_INV_ADDR		(10 << 4)	/* D-cache only */
#define CCR_CTRL_RTIG			8		/* IP7000+ only */

/*
 * OCP-OCMC
 */
#define OCMC_BASE		(OCP_BASE + OCP_OCMC)
#define OCMC_BANK_MASK		0x00
#define OCMC_BIST_CNTL		0x04			/* IP5000 only */
#define OCMC_BIST_STAT		0x08			/* IP5000 only */
#define OCMC_PAR_CNTL		0x04			/* IP8000 only - memory parity */

#define OCMC_BANK_PROG(n)	((1<<(n))-1)

#define OCMC_BIST_WRCK		(1 << 7)
#define OCMC_BIST_RESET		(1 << 5)
#define OCMC_BIST_SMART		(1 << 4)
#define OCMC_BIST_RUN		(1 << 3)
#define OCMC_BIST_REPAIR	(1 << 2)

#define OCMC_BIST_READY		(1 << 3)
#define OCMC_BIST_FAIL		(1 << 2)

/*
 * OCP-STATISTICS
 */
#define STATISTICS_BASE		(OCP_BASE + OCP_STATISTICS)
#define STAT_COUNTER_CTRL(n)	((n)*8)
#define STAT_COUNTER(n)		((n)*8 + 4)

#define STAT_EVENT_MP_INST		0
#define STAT_EVENT_OCM_ACCESS		4
#define STAT_EVENT_OCM_REQ		5
#define STAT_EVENT_IC_REQ_INVAL		13
#define STAT_EVENT_IC_MISS_INVAL	14
#define STAT_EVENT_IC_REQ_INVAL_NACK	15
#define STAT_EVENT_IC_REQ_VAL		16
#define STAT_EVENT_IC_MISS_VAL		17
#define STAT_EVENT_IC_REQ_VAL_NACK	18
#define STAT_EVENT_IC_MISS_Q 		19

#if defined(IP8000)
# define STAT_EVENT_DC_RD_REQ		24
# define STAT_EVENT_DC_RD_MISS		25
# define STAT_EVENT_DC_WR_REQ		26
# define STAT_EVENT_DC_WR_MISS		27
# define STAT_EVENT_DC_MISS_Q		28
# define STAT_EVENT_DC_WB_FULL		29
# define STAT_EVENT_DC_REQ_NACK		30
# define STAT_EVENT_DC_CORE_REQ		31
# define STAT_EVENT_DC_MISS		32
# define STAT_EVENT_DC_EVICT		33

# define STAT_EVENT_MMU_ITLB_REQ	40
# define STAT_EVENT_MMU_DTLB_REQ	41
# define STAT_EVENT_MMU_DTLB_RD_REQ	42
# define STAT_EVENT_MMU_DTLB_WR_REQ	43
# define STAT_EVENT_MMU_ITLB_REQ_DONE	44
# define STAT_EVENT_MMU_DTLB_REQ_DONE	45
# define STAT_EVENT_MMU_DTLB_RD_REQ_DONE	46
# define STAT_EVENT_MMU_DTLB_WR_REQ_DONE	47
# define STAT_EVENT_MMU_ITLB_MISS	48
# define STAT_EVENT_MMU_DTLB_MISS	49
# define STAT_EVENT_MMU_ITLB_RD_MISS	50
# define STAT_EVENT_MMU_ITLB_WR_MISS	51
# define STAT_EVENT_MMU_TLB_MISS_REQ	52
# define STAT_EVENT_MMU_TLB_PTE_MISS	53
# define STAT_EVENT_MMU_TLB_MISS_BUSY	54
#else
# define STAT_EVENT_DC_RD_REQ		20
# define STAT_EVENT_DC_RD_MISS		21
# define STAT_EVENT_DC_WR_REQ		22
# define STAT_EVENT_DC_WR_MISS		23
# define STAT_EVENT_DC_MISS_Q		24
# define STAT_EVENT_DC_WB_FULL		25
# define STAT_EVENT_DC_REQ_NACK		26
# define STAT_EVENT_DC_CORE_REQ		27
# define STAT_EVENT_DC_MISS		28
# define STAT_EVENT_DC_EVICT		29
#endif

#define STAT_EVENT_TRUE			30
#define STAT_EVENT_FALSE		31

/*
 * OCP_MTEST
 */
#define MTEST_BASE	(OCP_BASE + OCP_MTEST)
#define MTEST_ADDR	0x00
#define MTEST_WR	0x04
#define MTEST_RD	0x08
#define MTEST_CTRL	0x0c

/*
 * OCP_MCFG (IP7000 only)
 */
#define MCFG_BASE	(OCP_BASE + OCP_MCFG)
#define MCFG_CTRL	0x00
#define MCFG_WCFG	0x04
#define MCFG_RCFG	0x08

/*
 * Port registers
 */
#if defined(IP5000) || defined(IP7000)
# define IO_BASE		0x02000000
# define IO_PORT_RA		(IO_BASE + 0x00000000)
# define IO_PORT_RB		(IO_BASE + 0x00001000)
# define IO_PORT_RC		(IO_BASE + 0x00002000)
# define IO_PORT_RD		(IO_BASE + 0x00003000)
# define IO_PORT_RE		(IO_BASE + 0x00004000)
# define IO_PORT_RF		(IO_BASE + 0x00005000)
# define IO_PORT_RG		(IO_BASE + 0x00006000)
# define IO_PORT_RH		(IO_BASE + 0x00007000)
# define IO_PORT_RI		(IO_BASE + 0x00008000)
# define IO_PORT_RJ		(IO_BASE + 0x00009000)
# define IO_PORT_RLATCH		(IO_BASE + 0x00ff0000)	// For latched output only
# define IO_PORT_BR_OFFSET	0x00000800
#elif defined(IP8000)
# define IO_BASE		0xba000000
# define IO_PORT_RA		(IO_BASE + 0x00000000)
# define IO_PORT_RB		(IO_BASE + 0x00004000)
# define IO_PORT_RC		(IO_BASE + 0x00008000)
# define IO_PORT_RD		(IO_BASE + 0x0000c000)
# define IO_PORT_RE		(IO_BASE + 0x00010000)
# define IO_PORT_RF		(IO_BASE + 0x00014000)
# define IO_PORT_RG		(IO_BASE + 0x00018000)
# define IO_PORT_RH		(IO_BASE + 0x0001c000)
# define IO_PORT_RI		(IO_BASE + 0x00020000)
# define IO_PORT_RJ		(IO_BASE + 0x00024000)
# define IO_PORT_RK		(IO_BASE + 0x00028000)
# define IO_PORT_RL		(IO_BASE + 0x0002c000)
# define IO_PORT_RM		(IO_BASE + 0x00030000)
# define IO_PORT_RN		(IO_BASE + 0x00034000)
# define IO_PORT_RO		(IO_BASE + 0x00038000)
# define IO_PORT_RP		(IO_BASE + 0x0003c000)
# define IO_PORT_RQ		(IO_BASE + 0x00040000)
# define IO_PORT_RR		(IO_BASE + 0x00044000)
# define IO_PORT_RS		(IO_BASE + 0x00048000)
# define IO_PORT_RT		(IO_BASE + 0x0004c000)
# define IO_PORT_RU		(IO_BASE + 0x00050000)
# define IO_PORT_RV		(IO_BASE + 0x00054000)	// production chip only
# define IO_PORT_RW		(IO_BASE + 0x00058000)	// production chip only
# define IO_PORT_RX		(IO_BASE + 0x0005c000)	// production chip only
# define IO_PORT_RLATCH		(IO_BASE + 0x00ff0000)	// For latched output only
# define IO_PORT_BR_OFFSET	0x00002000
#endif

/*
 * General I/O Register Map (per port)
 */
#define IO_FUNC 0x00

#if defined(IP5000) || defined(IP7000)
# define IO_GPIO_CTL	0x04
# define IO_GPIO_OUT	0x08
# define IO_GPIO_IN	0x0C
# define IO_INT_STATUS	0x10
# define IO_INT_MASK	0x14
# define IO_INT_SET	0x18
# define IO_INT_CLR	0x1C
# define IO_TX_FIFO	0x20
# define IO_TX_FIFO_HI	0x24
# define IO_RX_FIFO	0x28
# define IO_RX_FIFO_HI	0x2c
# define IO_CTL0	0x30
# define IO_CTL1	0x34
# define IO_CTL2	0x38
# define IO_STATUS0	0x3c
# define IO_STATUS1	0x40
# define IO_STATUS2	0x44
# define IO_FIFO_WATER	0x48
# define IO_FIFO_LEVEL	0x4c
# define IO_GPIO_MASK	0x50

# define NUM_GPIO_PORT 9	/* port J and higher are special and not counted */
#elif defined(IP8000)
# define IO_ASYNCERR_CAUSE	0x0c
# define IO_ASYNCERR_ADDR	0x10
# define IO_INT_STATUS		0x14
# define IO_INT_MASK		0x18
# define IO_INT_SET_UNUSED	0x1c
# define IO_INT_CLR		0x20
# define IO_TX_FIFO		0x24
# define IO_TX_FIFO_HI		0x28
# define IO_RX_FIFO		0x2c
# define IO_RX_FIFO_HI		0x30
# define IO_INT_SET		0x34		/* IO_FUNC_SET */
# define IO_CTL0		0x38
# define IO_CTL1		0x3c
# define IO_CTL2		0x40
# define IO_CTL3		0x44
# define IO_CTL4		0x48
# define IO_CTL5		0x4c
# define IO_CTL6		0x50
# define IO_FIFO_WATER		0x54
# define IO_STATUS0		0x58
# define IO_STATUS1		0x5c
# define IO_STATUS2		0x60
# define IO_STATUS3		0x64
# define IO_STATUS4		0x68
# define IO_STATUS5		0x6c
# define IO_STATUS6		0x70
# define IO_FIFO_LEVEL		0x74

# define IO_IOM_BASE		IO_PORT_RU
# define IO_GPIO_GP_BASE(n)	(IO_IOM_BASE + ((n) << 5))
# define IO_GPIO_CTL		0x00
# define IO_GPIO_IN		0x04
# define IO_GPIO_OUT		0x08
# define IO_GPIO_FN1		0x0c
# define IO_GPIO_FN2		0x10
# define IO_GPIO_FN3		0x14
# define IO_GPIO_FN4		0x18
# define IO_GPIO_AUX		0x1c
# define IO_EXTINT_CFG		(IO_IOM_BASE + 0x100)

# define IO_PORT_PG0 (IO_IOM_BASE + 0x00)
# define IO_PORT_PG1 (IO_IOM_BASE + 0x20)
# define IO_PORT_PG2 (IO_IOM_BASE + 0x40)
# define IO_PORT_PG3 (IO_IOM_BASE + 0x60)
# define IO_PORT_PG4 (IO_IOM_BASE + 0x80)
# define IO_PORT_PG5 (IO_IOM_BASE + 0xA0)
# define IO_PORT_PG6 (IO_IOM_BASE + 0xC0)

# define NUM_GPIO_PORT 6	/* PG6 is special and not counted */
#endif

#define IO_FUNC_FUNCTION_CLK(func)	((1 << ((func) - 1)) << 24)	/* Function 0 doesn't need clock */
#define IO_FUNC_FUNCTION_RESET(func)	((1 << ((func) - 1)) << 4)	/* Function 0 doesn't need reset */
#define IO_FUNC_RX_FIFO			(1 << 3)
#define IO_FUNC_SELECT(func)		((func) << 0)

/*
 * External interrupt pins.
 */
#if defined(IP5000) || defined(IP7000)
# define EXT_INT_IO_BIT(pin)		((pin) + 5)			// Interrupt pin number -> I/O INT bit
# define EXT_INT_RISING_EDGE(pin)	(0x2 << (2*(pin) + 7))
# define EXT_INT_FALLING_EDGE(pin)	(0x1 << (2*(pin) + 7))
#elif defined(IP8000)
# define EXT_INT_IO_BIT(pin)		INT_CHIP(1, ((pin) + 23))	// Interrupt pin number -> I/O INT bit
#endif

/*
 * Flash (Port A)
 */
#define IO_XFL_BASE	IO_PORT_RA

#define IO_XFL_INT_START	(1 << 16)
#define IO_XFL_INT_ERR		(1 << 8)
#define IO_XFL_INT_DONE		(1 << 0)

#define IO_XFL_CTL0_MASK			(0xffe07fff)
#define IO_XFL_CTL0_RD_CMD(cmd)			(((cmd) & 0xff) << 24)
#define IO_XFL_CTL0_RD_DUMMY(n)			(((n) & 0x7) << 21)
#define IO_XFL_CTL0_CLK_WIDTH(core_cycles)	((((core_cycles) + 1) & 0x7e) << 8)	/* must be even number */
#define IO_XFL_CTL0_CE_WAIT(spi_cycles)		(((spi_cycles) & 0x3f) << 2)
#define IO_XFL_CTL0_MCB_LOCK			(1 << 1)
#define IO_XFL_CTL0_ENABLE			(1 << 0)
#define IO_XFL_CTL0_FAST_VALUE(div, wait)	(IO_XFL_CTL0_RD_CMD(0xb) | IO_XFL_CTL0_RD_DUMMY(1) | IO_XFL_CTL0_CLK_WIDTH(div) | IO_XFL_CTL0_CE_WAIT(wait) | IO_XFL_CTL0_ENABLE)
#define IO_XFL_CTL0_VALUE(div, wait)		(IO_XFL_CTL0_RD_CMD(3) | IO_XFL_CTL0_CLK_WIDTH(div) | IO_XFL_CTL0_CE_WAIT(wait) | IO_XFL_CTL0_ENABLE)

#define IO_XFL_CTL1_MASK			(0xc0003fff)
#define IO_XFL_CTL1_FC_INST(inst)		(((inst) & 0x3) << 30)
#define IO_XFL_CTL1_FC_DATA(n)			(((n) & 0x3ff) << 4)
#define IO_XFL_CTL1_FC_DUMMY(n)			(((n) & 0x7) << 1)
#define IO_XFL_CTL1_FC_ADDR			(1 << 0)

#define IO_XFL_CTL2_FC_CMD(cmd)			(((cmd) & 0xff) << 24)
#define IO_XFL_CTL2_FC_ADDR(addr)		((addr) & 0x00ffffff)	/* Only up to 24 bits */

#define IO_XFL_STATUS0_MCB_ACTIVE		(1 << 0)
#define IO_XFL_STATUS0_IOPCS_ACTIVE		(1 << 1)

/*
 * SDRAM
 */
#define IO_SDRAM_DATA_BASE	IO_PORT_RG
#define IO_SDRAM_CNTL_BASE	IO_PORT_RH

#define IO_SDRAM_CTRL0_EN_REF	(1 << 0)

/*
 * Port function code (common fucntion codes for all I/O ports)
 */
#define IO_PORTX_FUNC_GPIO		0x00
#define IO_PORTX_FUNC_XFL		0x01
#define IO_PORTX_FUNC_PCI		0x01		/* IP5000 & IP7000 */
#define IO_PORTX_FUNC_SERDES		0x01
#define IO_PORTX_FUNC_GMII		0x01
#define IO_PORTX_FUNC_DDR		0x01
#define IO_PORTX_FUNC_PCIE		0x01		/* IP8000 only */
#define IO_PORTX_FUNC_USB2_0		0x01
#define IO_PORTX_FUNC_PCIUSB_PHY	0x01		/* IP8000 only (for USB3.0) */
#define IO_PORTX_FUNC_I2S		0x01
#define IO_PORTX_FUNC_GPIO_INT_CLK	0x02		/* IP5000/IP7000 only */
#define IO_PORTX_FUNC_PLIO		0x02
#define IO_PORTX_FUNC_USB3_0		0x02		/* IP8000 only (for USB3.0) */
#define IO_PORTX_FUNC_GPIO_INT		0x03		/* IP5000/IP7000 only */
#define IO_PORTX_FUNC_MII		0x03		/* IP5000 only */

/*
 * FIFO
 */
#define IO_PORTX_INT_FIFO_TX_RESET	(1 << 31)
#define IO_PORTX_INT_FIFO_RX_RESET	(1 << 30)
#define IO_PORTX_INT_FIFO_TX_UF		(1 << 15)
#define IO_PORTX_INT_FIFO_TX_WM		(1 << 14)
#define IO_PORTX_INT_FIFO_RX_OF		(1 << 13)
#define IO_PORTX_INT_FIFO_RX_WM		(1 << 12)

#define IO_PORTX_FUNC_FIFO_TX_WM(n)	((n) << 16)
#define IO_PORTX_FUNC_FIFO_RX_WM(n)	((n) << 0)

/*
 * SerDes
 */
#define IO_PORTX_INT_SERDES_TXBUF_VALID (1 << 16)
#define IO_PORTX_INT_SERDES_RXERR (1 << 7)
#define IO_PORTX_INT_SERDES_RXEOP (1 << 6)
#define IO_PORTX_INT_SERDES_SYND (1 << 5)
#define IO_PORTX_INT_SERDES_TXBE (1 << 4)
#define IO_PORTX_INT_SERDES_TXEOP (1 << 3)
#define IO_PORTX_INT_SERDES_SXLP (1 << 2)
#define IO_PORTX_INT_SERDES_RXBF (1 << 1)
#define IO_PORTX_INT_SERDES_RXCRS (1 << 0)

#ifndef __ASSEMBLY__
#if defined(IP5000) || defined(IP7000)
struct ubicom32_io_port {
	volatile u32_t function;
	volatile u32_t gpio_ctl;
	volatile u32_t gpio_out;
	volatile u32_t gpio_in;
	volatile u32_t int_status;
	volatile u32_t int_mask;
	volatile u32_t int_set;
	volatile u32_t int_clr;
	volatile u32_t tx_fifo;
	volatile u32_t tx_fifo_hi;
	volatile u32_t rx_fifo;
	volatile u32_t rx_fifo_hi;
	volatile u32_t ctl0;
	volatile u32_t ctl1;
	volatile u32_t ctl2;
	volatile u32_t status0;
	volatile u32_t status1;
	volatile u32_t status2;
	volatile u32_t fifo_watermark;
	volatile u32_t fifo_level;
	volatile u32_t gpio_mask;
};

#define ubicom32_gpio_port ubicom32_io_port

/*
 * Convert a port number i.e. B to 0x2001000
 */
#define UBICOM32_PORT_NUM_TO_GPIO(port) ((struct ubicom32_gpio_port *)(IO_BASE + (port << 12)))

/*
 * Convert a gpio port i.e. 0x2001000 to B
 */
#define UBICOM32_GPIO_TO_PORT_NUM(port) (((port) - IO_BASE) >> 12)

/*
 * Convert a pointer, i.e. IO_PORT_RB to a struct ubicom32_gpio_port
 */
#define UBICOM32_GPIO_PORT(port) ((struct ubicom32_gpio_port *)(port))

#elif defined(IP8000)
struct ubicom32_io_port {
	volatile u32_t function;
	volatile u32_t reserved_0x04;
	volatile u32_t reserved_0x08;
	volatile u32_t asyncerr_cause;
	volatile u32_t asyncerr_addr;
	volatile u32_t int_status;
	volatile u32_t int_mask;
	volatile u32_t int_set_unused;
	volatile u32_t int_clr;
	volatile u32_t tx_fifo;
	volatile u32_t tx_fifo_hi;
	volatile u32_t rx_fifo;
	volatile u32_t rx_fifo_hi;
	volatile u32_t int_set;	// func_set
	volatile u32_t ctl0;
	volatile u32_t ctl1;
	volatile u32_t ctl2;
	volatile u32_t ctl3;
	volatile u32_t ctl4;
	volatile u32_t ctl5;
	volatile u32_t ctl6;
	volatile u32_t fifo_watermark;
	volatile u32_t status0;
	volatile u32_t status1;
	volatile u32_t status2;
	volatile u32_t status3;
	volatile u32_t status4;
	volatile u32_t status5;
	volatile u32_t status6;
	volatile u32_t fifo_level;
};

struct ubicom32_gpio_port {
	volatile u32_t gpio_ctl;
	volatile u32_t gpio_in;
	volatile u32_t gpio_out;
	volatile u32_t fn_sel[4];
	volatile u32_t gpio_aux;
};

struct ubicom32_iom {
	struct ubicom32_gpio_port gpio[7];
	struct ubicom32_gpio_port gpioreserved;
	volatile u32_t extint_cfg;
};

/*
 * Convert a port number i.e. 4 to 0xba050080
 */
#define UBICOM32_PORT_NUM_TO_GPIO(port) (&(((struct ubicom32_iom *)IO_IOM_BASE)->gpio[port]))

/*
 * Convert a gpio port i.e. 0xba050080 to 4
 */
#define UBICOM32_GPIO_TO_PORT_NUM(port) (((port) - IO_IOM_BASE) >> 5)

/*
 * Convert a pointer, i.e. PG1 to a struct ubicom32_gpio_port
 */
#define UBICOM32_GPIO_PORT(port) ((struct ubicom32_gpio_port *)(port))

#define UBICOM32_IO_EXTINT_CFG (((struct ubicom32_iom *)IO_IOM_BASE)->extint_cfg)
#endif

#define UBICOM32_IO_PORT(port) ((struct ubicom32_io_port *)((port)))
#endif

#if (UBICOM32_ARCH_VERSION >= 5)
#define	UBICOM32_CSR_PRIV_BIT		21
#define	UBICOM32_CSR_PREV_PRIV_BIT	22
#define	UBICOM32_CSR_FPTRAP_INEXACT_BIT		23
#define	UBICOM32_CSR_FPTRAP_UNDERFLOW_BIT	24
#define	UBICOM32_CSR_FPTRAP_OVERFLOW_BIT	25
#define	UBICOM32_CSR_FPTRAP_DIV_BY_0_BIT	26
#define	UBICOM32_CSR_FPTRAP_INVALID_BIT		27
#define	UBICOM32_CSR_FPTRAP_MASK		(0x1F << 23)
#define	UBICOM32_CSR_FPTRAP_SHIFT	23
#endif

#if defined(IP5000) || defined(IP7000)
#define UBICOM32_GET_RANGE_REGISTER(REG, IDX, LO, HI, EN) asm volatile (	\
        "       move.4          %0, "REG"_RANGE"IDX"_EN \n\t" \
        "       move.4          %1, "REG"_RANGE"IDX"_LO \n\t" \
        "       move.4          %2, "REG"_RANGE"IDX"_HI \n\t" \
	: "=d"((unsigned long) (EN)), "=d"((unsigned long)(LO)), "=d"((unsigned long)(HI)) \
        )

#define UBICOM32_SET_RANGE_REGISTER(REG, IDX, LO, HI, EN) asm volatile (   \
	"	move.4		"REG"_RANGE"IDX"_LO 	 ,%1 	\n\t"	\
	"	move.4		"REG"_RANGE"IDX"_HI 	 ,%2 	\n\t"	\
	"	move.4		"REG"_RANGE"IDX"_EN 	 ,%0 	\n\t"	\
	"	pipe_flush 0					\n\t"	\
	:								\
	: "d"((unsigned long)(EN)), "d"((unsigned long)(LO)), "d"((unsigned long)(HI)) \
	)
#else
#define UBICOM32_GET_RANGE_REGISTER(REG, IDX, LO, HI, EN, UEN) asm volatile (	\
	"	move.4		%0, "REG"_RANGE"IDX"_EN \n\t" \
	"	move.4		%1, "REG"_RANGE"IDX"_USER_EN \n\t" \
	"	move.4		%2, "REG"_RANGE"IDX"_LO \n\t" \
	"	move.4		%3, "REG"_RANGE"IDX"_HI \n\t" \
	: "=d"((unsigned long)(EN)), "=d"((unsigned long) (UEN)), "=d"((unsigned long) (LO)), "=d"((unsigned long) (HI)) \
	)

#define UBICOM32_SET_RANGE_REGISTER(REG, IDX, LO, HI, EN, UEN) asm volatile (   \
	"	move.4		"REG"_RANGE"IDX"_LO 	 ,%2 	\n\t"	\
	"	move.4		"REG"_RANGE"IDX"_HI 	 ,%3 	\n\t"	\
	"	move.4		"REG"_RANGE"IDX"_EN 	 ,%0 	\n\t"	\
	"	move.4		"REG"_RANGE"IDX"_USER_EN ,%1 	\n\t"	\
	"	pipe_flush 0					\n\t"	\
	:								\
	: "d"((unsigned long)(EN)), "d"((unsigned long)(UEN)), "d"((unsigned long)(LO)), "d"((unsigned long)(HI)) \
	)
#endif

#ifndef __ASSEMBLY__
/*
 * THREAD_STALL macro.
 */
#define THREAD_STALL \
	asm volatile ( \
	"	move.4	mt_dbg_active_clr, #-1	\n\t" \
	"	pipe_flush 0			\n\t" \
		: \
		: \
	)

/*
 * ubicom32_get_chip_id()
 *	Get the CHIP ID
 */
static inline u32_t ubicom32_get_chip_id(void)
{
	u32_t result;

	asm volatile (
	"	move.4		%0, CHIP_ID		\n\t"
		: "=r" (result)
	);

	return result;
}
#define IN_QEMU ((ubicom32_get_chip_id() & 0xffff) == 0xffff)

/*
 * ubicom32_get_inst_cnt()
 *	Get the per-thread INST_CNT value.
 */
static inline u32_t ubicom32_get_inst_cnt(void)
{
	u32_t result;

	asm volatile (
	"	move.4		%0, INST_CNT		\n\t"
		: "=r" (result)
	);

	return result;
}

/*
 * ubicom32_is_interrupt_set()
 */
static inline unsigned int ubicom32_is_interrupt_set(u8_t interrupt)
{
	u32_t ret;
	u32_t ibit = INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
		"	and.4	%0, "D(INT_STAT(INT_CHIP(0, 0)))", %1\n\t"
			: "=r" (ret)
			: "d" (ibit)
			: "cc"
		);

		return ret;
	}

#if defined(IP8000)
	if (INT_REG(interrupt) == INT_REG(INT_CHIP(2, 0))) {
		asm volatile (
		"	and.4	%0, "D(INT_STAT(INT_CHIP(2, 0)))", %1\n\t"
			: "=r" (ret)
			: "d" (ibit)
			: "cc"
		);

		return ret;
	}
#endif

	asm volatile (
	"	and.4	%0, "D(INT_STAT(INT_CHIP(1, 0)))", %1\n\t"
		: "=r" (ret)
		: "d" (ibit)
		: "cc"
	);

	return ret;
}

/*
 * ubicom32_is_interrupt_enabled()
 */
static inline unsigned int ubicom32_is_interrupt_enabled(u8_t interrupt)
{
	u32_t ret;
	u32_t ibit = INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
		"	and.4	%0, "D(INT_MASK(INT_CHIP(0, 0)))", %1\n\t"
			: "=r" (ret)
			: "d" (ibit)
			: "cc"
		);

		return ret;
	}

#if defined(IP8000)
	if (INT_REG(interrupt) == INT_REG(INT_CHIP(2, 0))) {
		asm volatile (
		"	and.4	%0, "D(INT_MASK(INT_CHIP(2, 0)))", %1\n\t"
			: "=r" (ret)
			: "d" (ibit)
			: "cc"
		);

		return ret;
	}
#endif

	asm volatile (
	"	and.4	%0, "D(INT_MASK(INT_CHIP(1, 0)))", %1\n\t"
		: "=r" (ret)
		: "d" (ibit)
		: "cc"
	);

	return ret;
}

/*
 * ubicom32_set_interrupt()
 */
static inline void ubicom32_set_interrupt(u8_t interrupt)
{
	u32_t ibit = INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
		"	move.4		"D(INT_SET(INT_CHIP(0, 0)))", %0\n\t"
			:
			: "r" (ibit)
		);

		return;
	}

#if defined(IP8000)
	if (INT_REG(interrupt) == INT_REG(INT_CHIP(2, 0))) {
		asm volatile (
		"	move.4		"D(INT_SET(INT_CHIP(2, 0)))", %0\n\t"
			:
			: "r" (ibit)
		);

		return;
	}
#endif

	asm volatile (
	"	move.4		"D(INT_SET(INT_CHIP(1, 0)))", %0\n\t"
		:
		: "r" (ibit)
	);
}

/*
 * ubicom32_set_interrupt_int_set0_with_mask()
 *	Set an interrupt in the INT_SET0 register.
 */
static inline void ubicom32_set_interrupt_int_set0_with_mask(u32_t ibit)
{
	asm volatile (
	"	move.4		"D(INT_SET(INT_CHIP(0, 0)))", %0\n\t"
		:
		: "r" (ibit)
	);
}

/*
 * ubicom32_clear_interrupt()
 */
static inline void ubicom32_clear_interrupt(u8_t interrupt)
{
	u32_t ibit = INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
		"	move.4		"D(INT_CLR(INT_CHIP(0, 0)))", %0\n\t"
			:
			: "r" (ibit)
		);

		return;
	}

#if defined(IP8000)
	if (INT_REG(interrupt) == INT_REG(INT_CHIP(2, 0))) {
		asm volatile (
		"	move.4		"D(INT_CLR(INT_CHIP(2, 0)))", %0\n\t"
			:
			: "r" (ibit)
		);

		return;
	}
#endif

	asm volatile (
	"	move.4		"D(INT_CLR(INT_CHIP(1, 0)))", %0\n\t"
		:
		: "r" (ibit)
	);
}

/*
 * ubicom32_clear_interrupt_int_clr0_with_mask()
 *	Clear an interrupt in the INT_CLR0 register.
 */
static inline void ubicom32_clear_interrupt_int_clr0_with_mask(u32_t ibit)
{
	asm volatile (
	"	move.4		"D(INT_CLR(INT_CHIP(0, 0)))", %0\n\t"
		:
		: "r" (ibit)
	);
}

/*
 * ubicom32_enable_interrupt()
 */
static inline void ubicom32_enable_interrupt(u8_t interrupt)
{
	u32_t ibit = INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
		"	or.4		"D(INT_MASK(INT_CHIP(0, 0)))", "D(INT_MASK(INT_CHIP(0, 0)))", %0\n\t"
			:
			: "d" (ibit)
			: "cc"
		);

		return;
	}

#if defined(IP8000)
	if (INT_REG(interrupt) == INT_REG(INT_CHIP(2, 0))) {
		asm volatile (
		"	or.4		"D(INT_MASK(INT_CHIP(2, 0)))", "D(INT_MASK(INT_CHIP(2, 0)))", %0\n\t"
			:
			: "d" (ibit)
			: "cc"
		);

		return;
	}
#endif

	asm volatile (
	"	or.4		"D(INT_MASK(INT_CHIP(1, 0)))", "D(INT_MASK(INT_CHIP(1, 0)))", %0\n\t"
		:
		: "d" (ibit)
		: "cc"
	);
}

/*
 * ubicom32_disable_interrupt()
 */
static inline void ubicom32_disable_interrupt(u8_t interrupt)
{
	u32_t ibit = ~INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
		"	and.4		"D(INT_MASK(INT_CHIP(0, 0)))", "D(INT_MASK(INT_CHIP(0, 0)))", %0\n\t"
			:
			: "d" (ibit)
			: "cc"
		);

		return;
	}

#if defined(IP8000)
	if (INT_REG(interrupt) == INT_REG(INT_CHIP(2, 0))) {
		asm volatile (
		"	and.4		"D(INT_MASK(INT_CHIP(2, 0)))", "D(INT_MASK(INT_CHIP(2, 0)))", %0\n\t"
			:
			: "d" (ibit)
			: "cc"
		);

		return;
	}
#endif

	asm volatile (
	"	and.4		"D(INT_MASK(INT_CHIP(1, 0)))", "D(INT_MASK(INT_CHIP(1, 0)))", %0\n\t"
		:
		: "d" (ibit)
		: "cc"
	);
}

/*
 * ubicom32_enable_global_interrupts()
 */
static inline void ubicom32_enable_global_interrupts(void)
{
	asm volatile (
	"	bset		GLOBAL_CTRL, GLOBAL_CTRL, #%%bit("D(GLOBAL_CTRL_INT_EN)")"
		:
		:
		: "cc"
	);
}

/*
 * ubicom32_disable_global_interrupts()
 */
static inline void ubicom32_disable_global_interrupts(void)
{
	asm volatile (
	"	bclr		GLOBAL_CTRL, GLOBAL_CTRL, #%%bit("D(GLOBAL_CTRL_INT_EN)")"
		:
		:
		: "cc"
	);
}

/*
 * ubicom32_get_reset_reason()
 */
static inline u32_t ubicom32_get_reset_reason(void)
{
	return *(u32_t *)(GENERAL_CFG_BASE + GEN_RESET_REASON);
}

/*
 * ubicom32_read_reg()
 */
static inline u32_t ubicom32_read_reg(volatile void *reg)
{
	u32_t v;
	asm volatile (
	"	move.4		%[dest], %[src]	\n\t"
		: [dest] "=r" (v)
		: [src] "m" (*(u32_t *)reg)
	);
	return v;
}

/*
 * ubicom32_write_reg()
 */
static inline void ubicom32_write_reg(volatile void *reg, u32_t v)
{
	asm volatile (
	"	move.4		%[dest], %[src]	\n\t"
		:
		: [src] "r" (v), [dest] "m" (*(u32_t *)reg)
	);
}

#endif /* __ASSEMBLY__ */
#endif /* _ASM_UBICOM32_UBICOM32_H */
