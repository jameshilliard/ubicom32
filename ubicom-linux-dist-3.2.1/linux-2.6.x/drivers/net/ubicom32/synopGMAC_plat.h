/**\file
 *  This file serves as the wrapper for the platform/OS dependent functions
 *  It is needed to modify these functions accordingly based on the platform and the
 *  OS. Whenever the synopsys GMAC driver ported on to different platform, this file
 *  should be handled at most care.
 *  The corresponding function definitions for non-inline functions are available in
 *  synopGMAC_plat.c file.
 * \internal
 * -------------------------------------REVISION HISTORY---------------------------
 * Ubicom		01/Mar/2010			Modified for Ubicom32
 * Synopsys		01/Aug/2007			Created
 */

#ifndef SYNOP_GMAC_PLAT_H
#define SYNOP_GMAC_PLAT_H 1

#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/kernel.h>
#include <linux/mii.h>
#include <linux/errno.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <asm/devtree.h>

#define TR0(fmt, args...) printk(KERN_CRIT "SynopGMAC: " fmt, ##args)

#ifdef DEBUG
#undef TR
#  define TR(fmt, args...) printk(KERN_CRIT "SynopGMAC: " fmt, ##args)
#else
# define TR(fmt, args...) /* not debugging: nothing */
#endif

#define DEFAULT_DELAY_VARIABLE  10
#define DEFAULT_LOOP_VARIABLE   10/*000*/

/* There are platform related endian conversions
 *
 */

//#define LE32_TO_CPU	__le32_to_cpu
//#define BE32_TO_CPU	__be32_to_cpu
//#define CPU_TO_LE32	__cpu_to_le32

/* Error Codes */
#define ESYNOPGMACNOERR   0
#define ESYNOPGMACNOMEM   ENOMEM
#define ESYNOPGMACPHYERR  EIO
//#define ESYNOPGMACBUSY    3

struct ethtionode {
	const struct devtree_node dn;	/* read-only */
	u16 command;			/* read/write */
	volatile u16 status;		/* read-only */
};

#define UBI32_ETH_VP_STATUS_LINK	(1<<0)
#define UBI32_ETH_VP_STATUS_SPEED100	(0x1<<1)
#define UBI32_ETH_VP_STATUS_SPEED1000	(0x1<<2)
#define UBI32_ETH_VP_STATUS_DUPLEX	(0x1<<3)
#define UBI32_ETH_VP_STATUS_FLOW_CTRL	(0x1<<4)

#define UBI32_ETH_VP_STATUS_RX_STATE	(0x1<<5)
#define UBI32_ETH_VP_STATUS_TX_STATE	(0x1<<6)

#define UBI32_ETH_VP_CMD_RX_ENABLE	(1<<0)
#define UBI32_ETH_VP_CMD_TX_ENABLE	(1<<1)

/**
  * These are the wrapper function prototypes for OS/platform related routines
  */

/**
  * This is a wrapper function for Memory allocation routine. In linux Kernel
  * it is kmalloc function
  * @param[in] size in bytes to allocate
  */

static void __inline__ *plat_alloc_memory(size_t size)
{
	return kmalloc(size, GFP_KERNEL);
}

/**
  * This is a wrapper function for consistent dma-able Memory allocation routine.
  * In linux Kernel, it depends on DMA support structure
  * @param[in] size in bytes to allocate
  */

static void __inline__ *plat_alloc_consistent_dmaable_memory(struct device *dev, size_t size, dma_addr_t *dma_addr)
{
	//return (dma_alloc_coherent (dev,size,dma_addr,GFP_KERNEL)); - we don't have it!
	return (dma_alloc_noncoherent (dev, size, dma_addr, GFP_KERNEL));
}

/**
  * This is a wrapper function for freeing consistent dma-able Memory.
  * In linux Kernel, it depends on DMA support structure
  * @param[in] cpu_addr pointer to be freed
  */

static void __inline__ plat_free_consistent_dmaable_memory(struct device *dev, size_t size, void *cpu_addr, dma_addr_t dma_addr)
{
	//dma_free_coherent (dev,size,cpu_addr,dma_addr); - we don't have it!
	dma_free_noncoherent (dev,size, cpu_addr, dma_addr);
}

/**
  * This is a wrapper function for Memory free routine. In linux Kernel
  * it it kfree function
  * @param[in] buffer pointer to be freed
  */
static void __inline__ plat_free_memory(void *buffer)
{
	kfree(buffer);
}

/**
  * This is a wrapper function for platform dependent delay
  * Take care while passing the argument to this function
  * @param[in] delay contains loops value of delay
  */
static void __inline__ plat_delay(u32 delay)
{
	while (delay--);
}

/**
 * The Low level function to read register contents from Hardware.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * \return  Returns the register contents
 */
static u32 __inline__ synopGMACReadReg(u32 *RegBase, u32 RegOffset)
{
	u32 addr = (u32)RegBase + RegOffset;
	u32 data;
	UBICOM32_LOCK(GMAC_LOCK_BIT);
	asm volatile (
	"	nops		5		\n\t"
	"	move.4		%0, (%1)	\n\t"
	: "=d" (data)
	: "a" (addr)
	);
	UBICOM32_UNLOCK(GMAC_LOCK_BIT);
	TR("%s RegBase = 0x%08x RegOffset = 0x%08x RegData = 0x%08x\n", __FUNCTION__, (u32)RegBase, RegOffset, data );
	return data;
}

/**
 * The Low level function to write to a register in Hardware.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * @param[in] Data to be written
 * \return  void
 */
static void  __inline__ synopGMACWriteReg(u32 *RegBase, u32 RegOffset, u32 RegData)
{
	u32 addr = (u32)RegBase + RegOffset;
        UBICOM32_LOCK(GMAC_LOCK_BIT);
	asm volatile (
	"	nops		5		\n\t"
	"	move.4		(%0), %1	\n\t"
	:
	: "a" (addr), "d" (RegData)
	);
	UBICOM32_UNLOCK(GMAC_LOCK_BIT);
	TR("%s RegBase = 0x%08x RegOffset = 0x%08x RegData = 0x%08x\n", __FUNCTION__,(u32) RegBase, RegOffset, RegData );
}

/**
 * The Low level function to set bits of a register in Hardware.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * @param[in] Bit mask to set bits to logical 1
 * \return  void
 */
static void __inline__ synopGMACSetBits(u32 *RegBase, u32 RegOffset, u32 BitPos)
{
	u32 addr = (u32)RegBase + RegOffset;
	u32 data;
	UBICOM32_LOCK(GMAC_LOCK_BIT);
	asm volatile (
	"	nops		5		\n\t"
	"	or.4		%0, (%1), %2	\n\t"
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	"	nops		5		\n\t"
#endif
	"	move.4		(%1), %0	\n\t"
	: "=&d" (data)
	: "a" (addr), "d" (BitPos)
	: "cc"
	);
	UBICOM32_UNLOCK(GMAC_LOCK_BIT);
	TR("%s !!!!!!!!!!!!! RegOffset = 0x%08x RegData = 0x%08x (|  0x%08x)\n", __FUNCTION__, RegOffset, data, BitPos);
}

/**
 * The Low level function to clear bits of a register in Hardware.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * @param[in] Bit mask to clear bits to logical 0
 * \return  void
 */
static void __inline__ synopGMACClearBits(u32 *RegBase, u32 RegOffset, u32 BitPos)
{
	u32 addr = (u32)RegBase + RegOffset;
	u32 data;
	UBICOM32_LOCK(GMAC_LOCK_BIT);
	asm volatile (
	"	nops		5		\n\t"
	"	and.4		%0, (%1), %2	\n\t"
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	"	nops		5		\n\t"
#endif
	"	move.4		(%1), %0	\n\t"
	: "=&d" (data)
	: "a" (addr), "d" (~BitPos)
	: "cc"
	);
	UBICOM32_UNLOCK(GMAC_LOCK_BIT);
	TR("%s !!!!!!!!!!! RegOffset = 0x%08x RegData = 0x%08x (& ~0x%08x)\n", __FUNCTION__, RegOffset, data, BitPos);
}

/**
 * The Low level function to Check the setting of the bits.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * @param[in] Bit mask to set bits to logical 1
 * \return  returns TRUE if set to '1' returns FALSE if set to '0'. Result undefined there are no bit set in the BitPos argument.
 *
 */
static bool __inline__ synopGMACCheckBits(u32 *RegBase, u32 RegOffset, u32 BitPos)
{
	u32 addr = (u32)RegBase + RegOffset;
	u32 data;
	UBICOM32_LOCK(GMAC_LOCK_BIT);
	asm volatile (
	"	nops		5		\n\t"
	"	and.4		%0, (%1), %2	\n\t"
	: "=d" (data)
	: "a" (addr), "d" (BitPos)
	: "cc"
	);
	UBICOM32_UNLOCK(GMAC_LOCK_BIT);
	return (data != 0);
}

#endif
