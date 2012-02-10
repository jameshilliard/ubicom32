/*
 * arch/ubicom32/mm/context.c
 *   Task context initialization
 *
 * (C) Copyright 2009, Ubicom, Inc.
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
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/pagemap.h>

#include <asm/mmu_hardware.h>
#include <asm/mmu_context.h>
#include <asm/tlb.h>
#include <asm/ubicom32.h>

#define CONTEXT_ASID_BITS 9
#define CONTEXT_ASID_BANK_BIT 9
#define CONTEXT_ASID_MASK ((1 << CONTEXT_ASID_BITS) - 1)
#define CONTEXT_ROLLOVER_VALUE_ILLEGAL 0

static DEFINE_SPINLOCK(asid_lock);
static unsigned int asid_rotator = UBICOM32_ASID_START;
static long rollover_rotator = 1;
static int asid_bank = 0;

/*
 * context_asid_increment()
 *	Increment the ASID counter based on size of the ASID.
 */
static unsigned int context_asid_increment(unsigned int asid)
{
	return ((asid + 1) & CONTEXT_ASID_MASK);
}

/*
 * context_disable_user_space()
 *     Disable all user space access from any thread.
 *
 * All threads will wind up going into do_page_fault() to have their
 * accesses fixed.  In do_page_fault(), they obtain a new ASID for each
 * context.
 */
static void context_disable_user_space(void)
{
	int i;
	unsigned int val;

	thread_t tid = thread_get_self();
	val = MMU_TNUM_ASID0_PUT(UBICOM32_ASID_KERNEL) |
		MMU_TNUM_ASID1_PUT(MMU_ASID_ILLEGAL) |
		MMU_TNUM_ASID2_PUT(MMU_ASID_ILLEGAL);

	for (i = 0; i < THREAD_COUNT; i++) {
		if (i == tid) {
			continue;
		}
		ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASIDS(i), val);
	}
}

/*
 * context_assign_kernel()
 *	Enable use of the Kernel ASID before a full process context is established.
 */
void context_assign_kernel(void)
{
	unsigned int val;
	thread_t tid;
	unsigned long flags;
#if 0
	printk("[%d]: switching to kernel only mode\n", tid);
#endif
	spin_lock_irqsave(&asid_lock, flags);
	tid = thread_get_self();
	ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_PGD(tid), (u32_t)virt_to_phys(init_mm.pgd));
	ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASIDS(tid), MMU_TNUM_ASID0_PUT(UBICOM32_ASID_KERNEL));
#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
	val = MMU_TNUM_CMP_EN0_PUT(1) | MMU_TNUM_ASID_CMP0_PUT(0x8);
#else
	val = MMU_TNUM_CMP_EN0_PUT(1) | MMU_TNUM_ASID_CMP0_PUT(0xa);
#endif
	ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASID_CMP(tid), val);
	spin_unlock_irqrestore(&asid_lock, flags);
}

/*
 * context_assign()
 *	Assign an ASID for this mm context on each use.
 *
 * The MMU for Ubicom32 has both a hardware TLB and a larger
 * On-Chip-Memory (OCM) based page table cache (PTEC).  
 *
 * In a classic TLB implementation, the TLB must be purged on context
 * switch.  This can result is significant performance overhead.
 * 
 * To avoid the expense of TLB purging (which for Ubicom32 requires both
 * a hardware purge and a PTEC purge), the TLB extends the address space
 * with an Address Space ID (ASID).  The ASID means that 2^n address
 * spaces can be uniquely identified.  For Ubicom32 the ASID is 10 bits,
 * giving us 1024 unique address spaces.
 *
 * Once the system uses/needs more than 2^10 unique address spaces, the
 * MMU will need to be purged (TLB/PTEC).  After the purge, some
 * mechanism must be used to ensure that previous ASID values are not
 * still "valid" in the system.  Since the PTEC can be large, chances
 * are that TLB mappings can remain in the PTEC across several context
 * switches.
 *
 * Several approaches can be used to prevent ASID re-use.  One could
 * search the Linux data structures (mm), one could maintain a bitmap of
 * in use ASIDs, or one could treat each new "set" of ASIDs as a unique
 * set using a rollover id to ensure that old ASIDs are reassigned
 * before an older tasks goes to use the ASID.  This later approach is
 * the simplest to implement and highest performance.
 *
 * The rollover counter is a 64 bit counter that is incremented each
 * time the ASID counter "rolls over".  If the "roll over" values in
 * the mm and the system do not match, the mm will be assigned a new
 * ASID on next use.
 *
 * If the ASID counter rolls over, the rollover counter is incremented
 * and the TLB and PTEC are purged.  This is necessary to avoid stale
 * re-use of ASID values.
 *
 * TODO: Do we want to enhance the algorithm to provide a unique asid
 * per shared vma.
 */
void context_assign(struct mm_struct *mm)
{	
	unsigned int val;
	thread_t tid;
	unsigned long flags;

	/*
	 * Track that the VM is run on this logical CPU.
	 * TODO: Are we really going to us this?
	 */
	mm->cpu_vm_mask = cpumask_of_cpu(raw_smp_processor_id());

	/*
	 * We try to keep using the same ASID increasing the value of
	 * the PTEC.
	 */
	spin_lock_irqsave(&asid_lock, flags);
	if (mm->context.rollover == rollover_rotator) {
		goto mmu_update;
	}

	mm->context.rollover = rollover_rotator;
	mm->context.asid = (asid_bank << CONTEXT_ASID_BANK_BIT) | asid_rotator;
	if (asid_rotator == MMU_ASID_ILLEGAL) {
		/*
		 * Mark that we rolled over and reset the asid rotator
		 */
		if (++rollover_rotator == CONTEXT_ROLLOVER_VALUE_ILLEGAL) {
			++rollover_rotator;
		}

		/*
		 * Divide the ASID into 2 banks.  By doing this we are
		 * able to ensure that ASIDs in-flight in the miss queue
		 * are treated as invalid.  Otherwise, the in-flight
		 * ASIDs could wind up being inserted into the TLB after
		 * the purge below.  This would result in stale entries
		 * in the TLB.  This split range use of the AISDs is the
		 * best method found to ensure that in-flight entries
		 * are not used after the purge.
		 */
		asid_bank ^= 1;
		tlb_asid_set_bank(CONTEXT_ASID_BANK_BIT, asid_bank);
		context_disable_user_space();
		tlb_purge_all();

		/*
		 * Reset the asid_ rotator and assign a new value for this mm.
		 */
		asid_rotator = UBICOM32_ASID_START;
		mm->context.rollover = rollover_rotator;
		mm->context.asid = (asid_bank << CONTEXT_ASID_BANK_BIT) | asid_rotator;
	}
	asid_rotator = context_asid_increment(asid_rotator);

	/*
	 * Setup the hardware ASID registers.  Use 0 for kernel space,
	 * and 1 and 2 for user space.
	 *
	 * ASID0 goes from 0xa0000000 (VM_ALLOC_START) to 0xafffffff (VM_ALLOC_END).
	 * ASID1 goes from 0 to 0x7fffffff.
	 * ASID2 goes from 0x80000000 to 0x9fffffff
	 */
mmu_update:
	BUG_ON(mm->context.asid == MMU_ASID_ILLEGAL);
	BUG_ON(!virt_addr_valid(mm->pgd));
	tid = thread_get_self();
#if 0
	printk("[%d]: switching to mm[%p], asid[%lx]\n",
	       tid, mm, mm->context.asid);
#endif
	ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_PGD(tid), virt_to_phys(mm->pgd));

	val = MMU_TNUM_ASID0_PUT(UBICOM32_ASID_KERNEL) |
		MMU_TNUM_ASID1_PUT(mm->context.asid) |
		MMU_TNUM_ASID2_PUT(mm->context.asid);
	ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASIDS(tid), val);

#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
	val = MMU_TNUM_ASID_MASK0_PUT(0x8) | MMU_TNUM_ASID_MASK1_PUT(0x8);
#else
	val = MMU_TNUM_ASID_MASK0_PUT(0xf) | MMU_TNUM_ASID_MASK1_PUT(0x8) | MMU_TNUM_ASID_MASK2_PUT(0xe);
#endif
	ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASID_MASK(tid), val);

#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
	val = MMU_TNUM_CMP_EN0_PUT(1) | MMU_TNUM_ASID_CMP0_PUT(0x8) | MMU_TNUM_CMP_EN1_PUT(1) | MMU_TNUM_ASID_CMP1_PUT(0x0);
#else
	val = MMU_TNUM_CMP_EN0_PUT(1) | MMU_TNUM_ASID_CMP0_PUT(0xa) |
		MMU_TNUM_CMP_EN1_PUT(1) | MMU_TNUM_ASID_CMP1_PUT(0x0) |
		MMU_TNUM_CMP_EN2_PUT(1) | MMU_TNUM_ASID_CMP2_PUT(0x8);
#endif
	ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASID_CMP(tid), val);
	spin_unlock_irqrestore(&asid_lock, flags);

}

/*
 * init_new_context()
 *	Create a new context.
 * 
 * For Ubicom32, we have both the TLB hardware and the larger PTEC
 * that must be managed.  We start by assigning an illegal value
 * to the context.  The running value will be picked up on context
 * switch.
 *
 * Called from Linux fork() code.
 */
int init_new_context(struct task_struct *tsk, struct mm_struct *mm)
{
#if 0
	printk("initing new context: %p\n", mm);
#endif
	mm->context.asid = MMU_ASID_ILLEGAL;
	mm->context.rollover = CONTEXT_ROLLOVER_VALUE_ILLEGAL;
	return 0;
}

