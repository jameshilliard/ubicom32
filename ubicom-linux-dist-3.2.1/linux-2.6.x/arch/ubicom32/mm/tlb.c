/*
 * arch/ubicom32/mm/tlb.c
 *   Ubicom32 architecture page fault implementation.
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

#include <linux/module.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/pagemap.h>

#include <asm/ubicom32.h>
#include <asm/tlb.h>
#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/mmu_hardware.h>
#include <asm/segment.h>

/*
 * One can not print from the tlb code so the best we can do is
 * check a condition and stall all of the threads.
 */
// #define DEBUG_TLB
#if defined(DEBUG_TLB)
#define DEBUG_ASSERT(cond) \
	if (!(cond)) { \
		THREAD_STALL; \
	}
#define STATIC
#else
#define DEBUG_ASSERT(cond)
#define STATIC static
#endif

#define TLB_INTERRUPT_NUM 29

/*
 * Per hardware thread data structure and mask to communicate
 * with PIC.
 */
volatile unsigned int tlb_faulted_thread_mask __attribute__((section(".ocm_data"))) = 0;
volatile struct tlb_miss_per_thread tlb_missq[THREAD_COUNT] __attribute__((section(".ocm_data")));

/*
 * The Software TLB supports the concept of grouping the 
 * ASID into banks.  The idea being that one can switch
 * banks invalidating a range of ASIDS.
 */
volatile int tlb_bank_mode = 0;
volatile int tlb_bank_bit = 0;
volatile int tlb_bank = 0;

/*
 * PTEC (Page Table Cache Data Structure)
 */
STATIC struct ptec_entries {
	unsigned int way0[TLB_PTEC_ENTRIES_PER_WAY];
	unsigned int way1[TLB_PTEC_ENTRIES_PER_WAY];
} mmu_ptec __attribute__((__section__(".ptec_entries")));

STATIC struct ptec_tag mmu_ptec_tags[TLB_PTEC_ENTRIES_PER_WAY]
	 __attribute__((__section__(".ptec_tags")));

STATIC unsigned int rotate = 0;

/*
 * Which thread/cpu are we?
 */
STATIC int tlb_tid __attribute__((section(".ocm_data"))) = -1;
STATIC unsigned int tlb_stack_space[128] __attribute__((section(".ocm_data")));
STATIC int tlb_to_pic_int __attribute__((section(".ocm_data")));
STATIC unsigned int tlb_fill_count __attribute__((section(".ocm_data"))) = 0;

#if defined(DEBUG_TLB)
#define TLB_LAST_WRITE_SIZE (1 << 12)
#define TLB_LAST_WRITE_MASK (TLB_LAST_WRITE_SIZE - 1)
static struct tlb_write_info {
	unsigned long sysval;
	unsigned int ti;
	unsigned int index;
	unsigned short *tagp;
	unsigned int *entryp;
	unsigned int insertw1;
	unsigned short atag;
	unsigned int rtag;
	pte_t pte;
} tlb_last_write[TLB_LAST_WRITE_SIZE];
static int tlb_last_write_idx = 0;
#endif

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
unsigned int tlb_stalled_restarted = 0;
#endif

/*
 * tlb_unlock
 *	Release the TLB lock.
 */
STATIC void tlb_unlock(void)
{
	UBICOM32_UNLOCK(TLB_LOCK_BIT);
}

/*
 * tlb_lock()
 *	Acquire the TLB lock, spin if not available.
 */
STATIC void tlb_lock(void)
{
	UBICOM32_LOCK(TLB_LOCK_BIT);
}

/*
 * tlb_is_locked()
 *	Return true if the TLB lock is held.
 */
STATIC int tlb_is_locked(void) __attribute__ ((unused));
STATIC int tlb_is_locked(void)
{
	int ret;
	UBICOM32_IS_LOCKED(TLB_LOCK_BIT);
	return ret;
}

/*
 * tlb_pipe_flush()
 *	Flush the pipe.
 */
STATIC void tlb_pipe_flush(void)
{
	asm volatile (
	"	pipe_flush	0			\n\t"
	);
}

/*
 * tlb_draim_missq()
 *	Drain the missq to make sure nothing is stale.
 *
 * Note: This is used during tlb init to ensure that the missq
 * is empty.
 */
STATIC void tlb_drain_missq(void)
{
	unsigned int missqw0;
	unsigned int missqw1;

	/*
	 * Read the hardware miss queue while we have an INT
	 * outstanding.
	 */
	while (ubicom32_is_interrupt_set(TLB_INTERRUPT_NUM)) {
		missqw0 = ubicom32_read_reg((volatile int *)MMU_MISSQW0);
		missqw1 = ubicom32_read_reg((volatile int *)MMU_MISSQW1);
		tlb_pipe_flush();
	}
}

/*
 * tlb_purge_all_internal()
 *	Purge the PTEC of all entries.
 *
 * This is done by zeroing out the tags array.
 */
STATIC void tlb_purge_all_internal(void)
{
	unsigned int purge_cmd;

	/*
	 * Wipe out all of the tags. We can use memset, as long as, memset uses either
	 * 32 or 16 bit writes.  Hardware is reading this data in 16 bit chunks.
	 */
#if defined(DEBUG_TLB)
	tlb_last_write[tlb_last_write_idx].sysval = UBICOM32_IO_TIMER->sysval;
	tlb_last_write[tlb_last_write_idx].ti = (unsigned int)sw_ksp[thread_get_self()];
	tlb_last_write[tlb_last_write_idx].pte.pte = 0x12345678;
	tlb_last_write_idx = ((tlb_last_write_idx + 1) & TLB_LAST_WRITE_MASK);
#endif

	memset((void *)&mmu_ptec_tags, 0, sizeof(struct ptec_tag) * TLB_PTEC_ENTRIES_PER_WAY);

	/*
	 * Purge the entire TLB Hardware.
	 */
	purge_cmd = MMU_IPURGE_CMD_PUT(MMU_IPURGE_CMD_PURGE_ALL);
	ubicom32_write_reg((volatile unsigned int *)MMU_IPURGE, purge_cmd);

	purge_cmd = MMU_DPURGE_CMD_PUT(MMU_DPURGE_CMD_PURGE_ALL);
	ubicom32_write_reg((volatile unsigned int *)MMU_DPURGE, purge_cmd);
}

/*
 * tlb_ptec_hash()
 *	Unified function for finding the index, atag, and rtag values.
 *
 * The index is where in the ptec the entry belongs.
 *
 * The abrievated tag (atag) is the portion of the tag that is written
 * into the hash table.
 *
 * The residual tag (rtag) is the portion of the tag that is written
 * into the entry.
 */
STATIC void tlb_ptec_hash(unsigned int asid, unsigned long vpn,
		     unsigned int *index, unsigned short *atag, unsigned int *rtag)
{
	unsigned long extra_asid_bits, asid_mask;

	/*
	 * Spilt out the address and asid into parts
	 */
	extra_asid_bits = ((TLB_PTEC_ENTRIES_PER_WAY_BITS > MMU_ASID_BITS) ? (TLB_PTEC_ENTRIES_PER_WAY_BITS - MMU_ASID_BITS) : 0);
	asid_mask = asid << extra_asid_bits;

	/*
	 * Compute the XOR hash and fill in the output parameters
	 */
	*index = (vpn  ^ asid_mask) & TLB_PTEC_ENTRIES_PER_WAY_MASK;
	*atag = (((vpn >> TLB_PTEC_ENTRIES_PER_WAY_BITS) & MMU_ATAG_MASK) << MMU_ASID_BITS) | asid;
	*rtag = ((vpn >> (TLB_PTEC_ENTRIES_PER_WAY_BITS + MMU_ATAG_BITS)) & MMU_RTAG_MASK) << PTEC_ENTRY_RTAG_SHIFT;
}

/*
 * tlb_remove_entry()
 *	Remove an entry in the PTEC and the hardware.
 *
 * Clear this page's tag from the PTEC if it matches.  We must use a
 * move.2 because hardware is reading the bits as well.
 */
STATIC void tlb_remove_entry(unsigned int asid, unsigned long vpn,
			     struct ptec_tag *p, unsigned int index, 
			     unsigned short atag, unsigned int rtag,
			     int execute)
{
	unsigned int purge_cmd;

	DEBUG_ASSERT(tlb_is_locked());

	if (p->tag0 == atag) {
		unsigned int *entryp = &mmu_ptec.way0[index];
		if ((*entryp & PTEC_ENTRY_RTAG_MASK) == rtag) {
			asm volatile (
			"	move.2 (%[tagp]), #0	\n\t"
				:
				: [tagp]"a" (&p->tag0)
				: "memory"
			);
		}
	} 

	if (p->tag1 == atag) {
		unsigned int *entryp = &mmu_ptec.way1[index];
		if ((*entryp & PTEC_ENTRY_RTAG_MASK) == rtag) {
			asm volatile (
			"	move.2 (%[tagp]), #0	\n\t"
				:
				: [tagp]"a" (&p->tag1)
				: "memory"
			);
		}
	}

	/*
	 * Now purge the TLB hardware by VPN and ASID.
	 */
	if (execute) {
		purge_cmd = MMU_IPURGE_ASID_PUT(asid) |
			MMU_DPURGE_VPN_PUT(vpn) |
			MMU_IPURGE_CMD_PUT(MMU_IPURGE_CMD_PURGE_BY_VPN_AND_ASID);
		ubicom32_write_reg((volatile unsigned int *)MMU_IPURGE, purge_cmd);
	}

	purge_cmd = MMU_DPURGE_ASID_PUT(asid) |
		MMU_DPURGE_VPN_PUT(vpn) |
		MMU_DPURGE_CMD_PUT(MMU_DPURGE_CMD_PURGE_BY_VPN_AND_ASID);
	ubicom32_write_reg((volatile unsigned int *)MMU_DPURGE, purge_cmd);
}

/*
 * tlb_write_entry()
 *	Write a TLB entry to the PTEC and to the hardware.
 *
 * Write the PTEC entry and tag. We must clear the tag to keep
 * the hardware walker away from the entry until we have updated
 * the actual entry.
 *
 * Since purge and insert are protected by a lock, we can insert
 * the new entry into the TLB hardware.
 */
STATIC void tlb_write_entry(pte_t pte,
			    unsigned int insertw1,
			    struct ptec_tag *p,
			    unsigned int index,
			    unsigned short atag,
			    unsigned int rtag)
{
	unsigned short *tagp;
	unsigned int *entryp;

	DEBUG_ASSERT(tlb_is_locked());

	/*
	 * First see if we match in either way.  The tag is the full
	 * 16 bits (vpn bits + asid bits).  
	 *
	 * NOTE: One might think we should check both the atag and
	 * rtag; however, the hardware does not support having the
	 * same atag in both ways.  Thus, the rtag value is not
	 * relevant, the code must ensure that the two atags are
	 * unique.
	 */
	if (p->tag0 == atag) {
		tagp = &p->tag0;
		entryp = &mmu_ptec.way0[index];
		goto write_entry;
	}

	if (p->tag1 == atag) {
		tagp = &p->tag1;
		entryp = &mmu_ptec.way1[index];
		goto write_entry;
	}

	/*
	 * Second, see if either way is free.
	 */
	if (p->tag0 == 0) {
		tagp = &p->tag0;
		entryp = &mmu_ptec.way0[index];
		goto write_entry;
	}

	if (p->tag1 == 0) {
		tagp = &p->tag1;
		entryp = &mmu_ptec.way1[index];
		goto write_entry;
	}

	/*
	 * Third, choose a way?
	 */
	if (rotate++ & 0x1) {
		tagp = &p->tag0;
		entryp = &mmu_ptec.way0[index];
		goto write_entry;
	}

	tagp = &p->tag1;
	entryp = &mmu_ptec.way1[index];

write_entry:

#if defined(DEBUG_TLB)
	tlb_last_write[tlb_last_write_idx].sysval = UBICOM32_IO_TIMER->sysval;
	tlb_last_write[tlb_last_write_idx].ti = (unsigned int)sw_ksp[thread_get_self()];
	tlb_last_write[tlb_last_write_idx].tagp = tagp;
	tlb_last_write[tlb_last_write_idx].entryp = entryp;
	tlb_last_write[tlb_last_write_idx].index = index;
	tlb_last_write[tlb_last_write_idx].insertw1 = insertw1;
	tlb_last_write[tlb_last_write_idx].atag = atag;
	tlb_last_write[tlb_last_write_idx].rtag = rtag;
	tlb_last_write[tlb_last_write_idx].pte = pte;

	tlb_last_write_idx = ((tlb_last_write_idx + 1) & TLB_LAST_WRITE_MASK);
#endif

	asm volatile (
	"	movei	d1, #"D(~PTEC_ENTRY_RTAG_MASK)"			\n\t"	// Obtain the mask
	"	and.4	d1, %[pte], d1					\n\t"	// Remove these bits
	"	move.2	(%[tagp]), #0					\n\t"	// Clear the atag
	"	or.4	(%[entryp]), %[rtag], d1			\n\t"	// Write the PTE + rtag
	"	move.2	(%[tagp]), %[atag]				\n\t"	// Write the atag
	"	moveai	a1, #%%hi("D(MMU_INSERTW0)")			\n\t"	// Move the PTE into INSERTW0
	"	or.4	%%lo("D(MMU_INSERTW0)")(a1), %[rtag], d1	\n\t"	// Write the PTE + rtag
	"	move.4	%%lo("D(MMU_INSERTW1)")(a1), %[insertw1]	\n\t"	// Write the PTE + rtag
		:
		: [pte]"r"(pte), [rtag]"r"(rtag), [tagp]"a"(tagp),
		  [entryp]"a"(entryp), [atag]"d"(atag), [insertw1]"d"(insertw1)
		: "cc", "memory", "d1", "a1"
	);

	DEBUG_ASSERT(atag != 0);
	DEBUG_ASSERT(*tagp == atag);
}

/*
 * tlb_walk_and_fill()
 *	Walk the 2 level page directory and fill the PTEC.
 */
STATIC int tlb_walk_and_fill(unsigned int missqw0, unsigned int missqw1)
{
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *ptep;

	unsigned char type;
	unsigned int asid;
	unsigned long address;

	unsigned short atag;
	unsigned int index, rtag;
	struct ptec_tag *p;

	/*
	 * If this is not a miss, we need to generate a fault.
	 */
	type = (unsigned char)MMU_MISSQW0_TYPE_GET(missqw0);
	if (unlikely(type)) {
		return 0;
	}

	/*
	 * If this is the reserved ASID value, force a fault.
	 */
	asid = MMU_MISSQW1_ASID_GET(missqw1);
	if (unlikely(asid == MMU_ASID_ILLEGAL)) {
		return 0;
	}

	/*
	 * If we are in bank mode, only allow asids that match the current
	 * bank value.
	 */
	if ((asid != UBICOM32_ASID_KERNEL) && tlb_bank_mode && ((asid >> tlb_bank_bit) != tlb_bank)) {
		return 0;
	}

	/*
	 * Find the PTE, if we can.
	 */
	pgd = (pgd_t *)(MMU_MISSQW0_PGD_GET(missqw0) << MMU_MISSQW0_PGD_SHIFT);
	address =  (unsigned long)(MMU_MISSQW1_VPN_GET(missqw1) << MMU_VPN_SHIFT);
	pmd = (pmd_t *)__pgd_offset(pgd, address);
	DEBUG_ASSERT((unsigned long)pmd >= UBICOM32_FDPIC_UNMAPPED_START);
	if (unlikely(pmd_none(*pmd)) || (unlikely(pmd_bad(*pmd)))) {
		return 0;
	}

	ptep = pte_offset_map_phys(pmd, address);
	DEBUG_ASSERT((unsigned long)ptep >= UBICOM32_FDPIC_UNMAPPED_START);
	if (unlikely(pte_none(*ptep)) || (unlikely(pte_bad(*ptep)))) {
		return 0;
	}

	/*
	 * Track fills from PTE to PTEC so that the host OS can determine
	 * recently misses/accessed entries.
	 */
	pte_val(*ptep) |= PTE_PTEC_FILL;

	/*
	 * Find our location based on asid and address.
	 */
	tlb_ptec_hash(asid, (address >> PAGE_SHIFT), &index, &atag, &rtag);
	p = &mmu_ptec_tags[index];
	tlb_lock();
	tlb_write_entry(*ptep, missqw1, p, index, atag, rtag);
	tlb_unlock();
	return 1;
}

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
/*
 * tlb_restart_stalled_threads()
 *	See if any threads are stalled due to IP8K TLB defect.
 *
 * The IP8K pre-production chips have a bug whereby if 1 thread
 * takes a TLB miss while another thread takes a TLB fault, the
 * TLB fault will not be written to the missq.  This function
 * is designed to detect this situation and get the stalled threads
 * going again.  It does so by detecting MMU stalled threads that
 * have not been posted to the miss queue.
 *
 * TODO: When this workaround is removed, the PIC will need to be fixed
 * so that faulted_thread_mask is cleared before the preemption and the
 * code flow is returned to its previous flow.
 */
STATIC void tlb_restart_stalled_threads(unsigned int missqw0)
{
	thread_t tid = MMU_MISSQW0_TNUM_GET(missqw0);
        int retries = 0;
	unsigned int possible;
	unsigned int bus_state0;
	unsigned int bus_state1;
	unsigned int bus_state2;
        unsigned int faulted_maybe;
	unsigned int mmu_status;

	/*
	 * No TLB interrupts. Chaeck if anybody is blocked
	 * because of a fault.
	 */
	bus_state0 = ubicom32_read_reg((volatile int *)MMU_BUS_ST0);
	bus_state1 = ubicom32_read_reg((volatile int *)MMU_BUS_ST1);
	bus_state2 = ubicom32_read_reg((volatile int *)MMU_BUS_ST2);
        faulted_maybe = bus_state0 & bus_state1 & bus_state2;
	UBICOM32_LOCK(MT_EN_LOCK_BIT);
	faulted_maybe &= (~(1 << tid)) & (~tlb_faulted_threads());
	UBICOM32_UNLOCK(MT_EN_LOCK_BIT);

restart:
	if (ubicom32_is_interrupt_set(TLB_INTERRUPT_NUM)) {
		return;
	}

	/*
	 * No TLB interrupts. Chaeck if anybody is blocked
	 * because of a fault.
	 */
	bus_state0 = ubicom32_read_reg((volatile int *)MMU_BUS_ST0);
	bus_state1 = ubicom32_read_reg((volatile int *)MMU_BUS_ST1);
	bus_state2 = ubicom32_read_reg((volatile int *)MMU_BUS_ST2);
	possible = bus_state0 & bus_state1 & bus_state2;

	/*
	 * If the state of the faulted threads change, drop them out of the list.
	 */
	faulted_maybe &= possible;
        if (!faulted_maybe) {
		return;
	}

        mmu_status = ubicom32_read_reg((volatile int *)MMU_STATUS);
        if (MMU_STATUS_STATE_GET(mmu_status)) {
		retries = 0;
		goto restart;
	}

	if (++retries < 5) {
		goto restart;
	}

	tlb_stalled_restarted++;

	/*
	 * Restart the stalled threads which will re-execute the
	 * fault and thus have their entry properly written to the
	 * miss queue.
	 */
	while (faulted_maybe) {
		unsigned tlb_restart = 1 << 5;
		tid = ffz(~faulted_maybe);
		faulted_maybe &= ~(1 << tid);
		tlb_restart |= tid;
		ubicom32_write_reg((volatile int *)MMU_RESTART, tlb_restart);
	}
}
#endif /* CONFIG_UBICOM32_IP8K_SHUTTLE */

/*
 * tlb()
 *	Implements the Software TLB walker.
 *
 * The Software TLB thread's responsibility is to read the MISSQ
 * find a valid PTE entry and update the PTEC or signal a fault
 * to the PIC.
 */
STATIC void tlb(void *arg)
{
	/*
	 * Drain the missq
	 */
	tlb_drain_missq();
	ubicom32_enable_interrupt(TLB_INTERRUPT_NUM);
	while (1) {
		unsigned int missqw0;
		unsigned int missqw1;
		int more;
		int resolved;
		thread_t tid;

		thread_suspend();

		/*
		 * No need to clear the interrupt, reading the queues
		 * clears the TLB interrupt.
		 *
		 * Read the hardware miss queue.
		 */
		missqw0 = ubicom32_read_reg((volatile int *)MMU_MISSQW0);
		missqw1 = ubicom32_read_reg((volatile int *)MMU_MISSQW1);
		more = MMU_MISSQW1_MULTIPLE_GET(missqw1);

		if (likely(!more)) {
			/*
			 * Walk the page directory to see if we can resolve
			 * the fault.
			 */
			resolved = tlb_walk_and_fill(missqw0, missqw1);
		} else {
			/*
			 * The read of the queue here is very tight.
			 * The queue can not be re-read within 7 cycles.
			 * The miss predicated branch above is just long
			 * enough to ensure that the queue is readable
			 * once again.
			 */
			unsigned int missqw0_2nd;
			unsigned int missqw1_2nd;
			missqw0_2nd = ubicom32_read_reg((volatile int *)MMU_MISSQW0);
			missqw1_2nd = ubicom32_read_reg((volatile int *)MMU_MISSQW1);
			DEBUG_ASSERT(MMU_MISSQW1_MULTIPLE_GET(missqw1_2nd) == 0);
			resolved = tlb_walk_and_fill(missqw0_2nd, missqw1_2nd);
			if (resolved) {
				resolved = tlb_walk_and_fill(missqw0, missqw1);
			} else {
				missqw0 = missqw0_2nd;
				missqw1 = missqw1_2nd;
			}
		}

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
		/*
		 * The 8K pre-production chip has a defect that causes
		 * threads to permanently stall if a fault occurs
		 * at the same time as a miss.  Attempt to find and
		 * restart these "stalled" threads.
		 */
		tlb_restart_stalled_threads(missqw0);
#endif

		/*
		 * If we fixed the miss, start the thread running again
		 * and do not signal the PIC.
		 */
		if (resolved) {
			tlb_fill_count++;
			ubicom32_write_reg((volatile int *)MMU_RESTART, missqw0);
			continue;
		}

		/*
		 * If things are not resolved, a fault has occurred
		 * save the faulting information and notify the PIC.
		 *
		 * The thread is left in the MMU blocked state in
		 * addition to being in the software faulted state.
		 */
		tid = MMU_MISSQW0_TNUM_GET(missqw0);
		tlb_missq[tid].missqw0 = missqw0;
		tlb_missq[tid].missqw1 = missqw1;
		tlb_faulted_set(tid);
		ubicom32_set_interrupt(tlb_to_pic_int);
	}
}

/*
 * tlb_purge_all()
 *	Purge the PTEC of all entries.
 *
 * This is done by zeroing out the tags array.
 */
void tlb_purge_all(void)
{
	cpu_physical();
	tlb_lock();
	tlb_purge_all_internal();
	tlb_unlock();
	tlb_pipe_flush();
	cpu_virtual();
}

/*
 * tlb_purge_asid()
 *	Purge the PTEC of all entries containing the specified ASID.
 */
void tlb_purge_asid(unsigned int asid)
{
	unsigned int purge_cmd;
	struct ptec_tag *p;
	int i;

	cpu_physical();
	tlb_lock();
	p = mmu_ptec_tags;
	for (i = 0; i < TLB_PTEC_ENTRIES_PER_WAY; i++, p++) {
		if ((p->tag0 & MMU_ASID_MASK) == asid) {
			asm volatile (
			"	move.2 (%[tagp]), #0	\n\t"
			:
			: [tagp]"a" (&p->tag0)
			: "memory"
			);
		}

		if ((p->tag1 & MMU_ASID_MASK) == asid) {
			asm volatile (
			"	move.2 (%[tagp]), #0	\n\t"
			:
			: [tagp]"a" (&p->tag1)
			: "memory"
			);
		}
	}

	/*
	 * Now purge the TLB hardware by ASID.
	 */
	purge_cmd = MMU_IPURGE_ASID_PUT(asid) |
		MMU_IPURGE_CMD_PUT(MMU_IPURGE_CMD_PURGE_BY_ASID);
	ubicom32_write_reg((volatile unsigned int *)MMU_IPURGE, purge_cmd);

	purge_cmd = MMU_DPURGE_ASID_PUT(asid) |
		MMU_DPURGE_CMD_PUT(MMU_DPURGE_CMD_PURGE_BY_ASID);
	ubicom32_write_reg((volatile unsigned int *)MMU_DPURGE, purge_cmd);
	tlb_unlock();
	tlb_pipe_flush();
	cpu_virtual();
}

/*
 * tlb_update_entry()
 *	Called to update the PTEC and TLB with recent changes to a pte.
 *
 * The target is 0 for iTLB and 1 for dTLB.
 */
void tlb_update_entry(int target, unsigned int asid, unsigned long addr, pte_t pte)
{
	unsigned int insertw1;
	unsigned short atag;
	unsigned int index, rtag;
	struct ptec_tag *p;
	unsigned long vpn = (addr >> PAGE_SHIFT);

	insertw1 = MMU_INSERTW1_VPN_PUT(vpn) |
		MMU_INSERTW1_TARGET_PUT(target) |
		MMU_INSERTW1_ASID_PUT(asid);

	tlb_ptec_hash(asid, vpn, &index, &atag, &rtag);
	p = &mmu_ptec_tags[index];
	cpu_physical();
	tlb_lock();
	tlb_write_entry(pte, insertw1, p, index, atag, rtag);
	tlb_unlock();
	tlb_pipe_flush();
	cpu_virtual();
}

/*
 * tlb_purge_page()
 *	Purge the specified page from the PTEC.
 */
void tlb_purge_page(unsigned int asid, unsigned long address, int execute)
{
	unsigned short atag;
	unsigned int index, rtag;
	struct ptec_tag *p;
	unsigned long vpn = (address >> PAGE_SHIFT);

	tlb_ptec_hash(asid, vpn, &index, &atag, &rtag);
	p = &mmu_ptec_tags[index];

	cpu_physical();
	tlb_lock();
	tlb_remove_entry(asid, vpn, p, index, atag, rtag, execute);
	tlb_unlock();
	tlb_pipe_flush();
	cpu_virtual();
}

/*
 * tlb_purge_range()
 *	Purge the specified address from the PTEC.
 */
void tlb_purge_range(unsigned int asid, unsigned long start, unsigned long end, int execute)
{
	unsigned long s, e;

	BUG_ON(start >= end);
	s = start & PAGE_MASK;
	e = (end + (PAGE_SIZE - 1)) & PAGE_MASK;
	while (s < e) {
		tlb_purge_page(asid, s, execute);
		s += PAGE_SIZE;
	}
}

/*
 * tlb_asid_set_bank()
 *	Set the range of valid asids.
 */
void tlb_asid_set_bank(int bits, int bank)
{
	cpu_physical();
	tlb_lock();
	tlb_bank_mode = 1;
	tlb_bank_bit = bits;
	tlb_bank = bank;
	tlb_unlock();
	cpu_virtual();
}

/*
 * tlb_init()
 *	Initialize the TLB hardware and Software TLB Walker.
 *
 * One interesting aspect of the TLB on the 8K is that the TLB is a shared
 * resource by all logical CPUs.
 */
void __init tlb_init(void)
{
	void *stack_high;
        unsigned int config;
	int i;
	unsigned int pgmask;

	/*
	 * Tell the PIC about our IRQ so that it will unsuspend
	 * if one occurs while waiting for the per thread lock.
	 */
	if (irq_soft_alloc(&tlb_to_pic_int) < 0) {
		panic("no software IRQ is available\n");
		return;
	}
	pic_set_mmu_irq(tlb_to_pic_int);

	/*
	 * Determine the stack space and color it.
	 */
	stack_high = (void *)tlb_stack_space;
	memset(stack_high, 0xaa,  sizeof(tlb_stack_space));
	stack_high += sizeof(tlb_stack_space);
	stack_high -= 8;

	/*
	 * Now allocate and start the Software TLB.
	 */
	tlb_tid = thread_alloc();
	if (tlb_tid < 0) {
		panic("no thread available to run Software TLB");
		return;
	}
	thread_start(tlb_tid, tlb, NULL, stack_high, THREAD_TYPE_NORMAL);

	/*
	 * Zero out the PTEC.
	 */
	memset((void *)&mmu_ptec, 0, sizeof(struct ptec_entries));
	memset((void *)&mmu_ptec_tags, 0, sizeof(struct ptec_tag) * TLB_PTEC_ENTRIES_PER_WAY);

	/*
	 * Setup TLB configuration registers (CF0, CF1, CF2).
	 */
	config = MMU_PTEC_CFG0_ENTRY_BITS_PUT(TLB_PTEC_ENTRIES_PER_WAY_BITS) |
		MMU_PTEC_CFG0_RTAG_PUT(MMU_RTAG_MASK);
	ubicom32_write_reg((volatile unsigned int *)MMU_PTEC_CFG0, config);
	ubicom32_write_reg((volatile unsigned int *)MMU_PTEC_CFG1, (unsigned int)&mmu_ptec_tags);
	ubicom32_write_reg((volatile unsigned int *)MMU_PTEC_CFG2, (unsigned int)&mmu_ptec);

	/*
	 * Initialize all of the per thread configuration registers to 0.
	 */
	for (i = 0; i < THREAD_COUNT; i++) {
		ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_PGD(i), 0);
		ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASIDS(i), 0);
		ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASID_MASK(i), 0);
		ubicom32_write_reg((volatile unsigned int *)MMU_TNUM_ASID_CMP(i), 0);
	}

	/*
	 * Enable the TLB hardware. The TLB PAGE MASK is a function
	 * of the page size represented by the min page size.
	 * 4k = 0x0, 8k = 0x1, and 16 = 0x3.
	 *
	 * The production chip added a power saving feature in a
	 * previously unused bit.  We set that bit here to take advantage
	 * of this new feature.  The SLEEP_EN bit is tied to the MP clock
	 * gating feature of the CPU.  The MMU clock is gated if the CPU
	 * is idle for a programmed number of instruction cycles.
	 */
	pgmask = (1 << (PAGE_SHIFT - 12)) - 1;

	/*
	 * TODO: The production IP8k adds an MMU clock gating feature which
	 * is associated with the MP clock gating feature.  We don't want to
	 * turn this on until after the "day one" bring up work is completed.
	 */
#define USE_UBICOM32_CLOCK_GATING_FEATURE
#if defined(USE_UBICOM32_CLOCK_GATING_FEATURE)
	config = MMU_CONFIG_HPR_DELAY_PUT(0xf) |
		 MMU_CONFIG_PTEC_ENABLE_PUT(1) |
		 MMU_CONFIG_PAGE_MASK_PUT(pgmask) |
		 MMU_CONFIG_SLEEP_EN_PUT(1);
#else
	config = MMU_CONFIG_HPR_DELAY_PUT(0xf) |
		 MMU_CONFIG_PTEC_ENABLE_PUT(1) |
		MMU_CONFIG_PAGE_MASK_PUT(pgmask);
#endif
	ubicom32_write_reg((volatile unsigned int *)MMU_CONFIG, config);
}
