/*
 * arch/ubicom32/mm/fault.c
 *   Ubicom32 architecture page fault implementation.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *  Copyright (C) 1998  D. Jeff Dionne <jeff@lineo.ca>,
 *  Copyright (C) 2000  Lineo, Inc.  (www.lineo.com)
 *
 *  Based on:
 *
 *  linux/arch/m68k/mm/fault.c
 *
 *  Copyright (C) 1995  Hamish Macdonald
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
#include <linux/signal.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/hardirq.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/uaccess.h>

#include <asm/system.h>
#include <asm/processor.h>
#include <asm/traps.h>
#include <asm/pgtable.h>
#include <asm/tlb.h>
#include <asm/mmu_hardware.h>
#include <asm/mmu_context.h>

/*
 * Uncomment the define of VM_TRACE_DEBUG to display debugging information.
 */
// #define VM_TRACE_DEBUG 1

#if defined(CONFIG_DEBUG_VM)
#define STATIC
#else
#define STATIC static
#endif

/*
 * Create a macro for trace level debugging.  Critical messages
 * directly use printk since we don't turn those off.
 */
#if !defined(VM_TRACE_DEBUG)
#define fault_printk(...)
#else

#define FAULT_DBG_ERROR 10
#define FAULT_DBG_TRACE 20
STATIC unsigned int fault_dbg_lvl = 10;
#define fault_printk(lvl, printkargs...)	\
	if ((lvl) <= fault_dbg_lvl) {		\
		printk(KERN_INFO printkargs);	\
	}
#endif

/*
 * fault_terminate()
 *	Terminate the current process using do_exit(SIGKILL).
 *
 * This code prints a trap marker and then terminates the current process.  Even if the 
 * code is executing in the kernel, we assume that this is a bad pointer from the user
 * and we only take out the one processes that got us here.
 */
STATIC void fault_terminate(struct pt_regs *regs, unsigned long missqw0, unsigned long missqw1) 
{
	/*
	 * This is an unrecoverable error, break spinlocks, printk, and die.
	 */
	bust_spinlocks(1);
	if (user_mode(regs)) {
		printk(KERN_ALERT "\nUser Mode Page Fault\n");
	} else {
		printk(KERN_ALERT "\nKernel Mode Page Fault\n");
	}

	printk(KERN_ALERT "\tmissqw0: %08lx, missqw1: %08lx\n", missqw0, missqw1);

	printk(KERN_ALERT "\ttid: %08lx, pgd: %08lx\n",
			MMU_MISSQW0_TNUM_GET(missqw0),
        		MMU_MISSQW0_PGD_GET(missqw0) << MMU_MISSQW0_PGD_SHIFT);
	printk(KERN_ALERT "\tasid: %08lx, addr: %08lx\n",
			MMU_MISSQW1_ASID_GET(missqw1),
			MMU_MISSQW1_VPN_GET(missqw1) << MMU_VPN_SHIFT);
	printk(KERN_ALERT "\ttype: %s, src: %s, op: %s\n",
			MMU_MISSQW0_TYPE_GET(missqw0) ? "priv" : "miss",
			MMU_MISSQW0_SRC_GET(missqw0) ? "data" : "inst",
			MMU_MISSQW0_OP_GET(missqw0) ? "write" : "read-exe");

	die("Page Fault Terminate", regs, 0);
	do_exit(SIGKILL);
}

/*
 * fault_pte()
 *	Obtain the pte corresponding to the fault.
 */
STATIC pte_t *fault_pte(pgd_t *pgd, unsigned long address)
{
	pmd_t *pmd = (pmd_t *)__pgd_offset(pgd, address);

	if (unlikely(pmd_none(*pmd)) ||
	    (unlikely(pmd_bad(*pmd))) ||
	    (unlikely(pmd_present(*pmd) == 0))) {
		return 0;
	}
	return pte_offset_map_phys(pmd, address);
}

#if defined(CONFIG_DEBUG_VM)
/*
 * fault_is_resolved()
 *	Return true if the fault appears to be resolved.
 */
STATIC int fault_is_resolved(struct pt_regs *regs,
			 unsigned long missqw0,
			 unsigned long missqw1)
{
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *ptep;
	unsigned long address;
	unsigned long src = MMU_MISSQW1_SRC_GET(missqw1);
	unsigned long op = MMU_MISSQW0_OP_GET(missqw0);

	/*
	 * Potential hardware bug, check if this is an ifetch with a write op.
	 * If so, we will be in an infinite loop.  check here because this
	 * is under debug.
	 */
	if ((src == 0) && (op == 1)) {
		printk(KERN_CRIT "iftech/write: missqw0=%lx, missqw1=%lx\n",
		       missqw0, missqw1);
		return 0;
	}

	/*
	 * See if we now have a valid pte?
	 */
	pgd = (pgd_t *)(MMU_MISSQW0_PGD_GET(missqw0) << MMU_MISSQW0_PGD_SHIFT);
	address =  (unsigned long)(MMU_MISSQW1_VPN_GET(missqw1) << MMU_VPN_SHIFT);
	pmd = (pmd_t *)__pgd_offset(pgd, address);
	if (unlikely(pmd_none(*pmd)) || (unlikely(pmd_bad(*pmd)))) {
		printk(KERN_CRIT "address[0x%lx] pgd[%p] pmd[%p] is empty\n",
		       address, pgd, pmd);
		return 0;
	}

	ptep = pte_offset_map_phys(pmd, address);
	if (unlikely(pte_none(*ptep)) || (unlikely(pte_bad(*ptep)))) {
		printk(KERN_CRIT "address[0x%lx] pgd[%p] pmd[%p] pte[%p] is empty\n",
		       address, pgd, pmd, ptep);
		return 0;
	}

	if (unlikely(!pte_present(*ptep))) {
		printk(KERN_CRIT "address[0x%lx] pgd[%p] pmd[%p] pte[%p] is invalid: 0x%lx\n",
		       address, pgd, pmd, ptep, pte_val(*ptep));
		return 0;
	}

	if (MMU_MISSQW0_OP_GET(missqw0) && !pte_write(*ptep)) {
		printk(KERN_CRIT "address[0x%lx] pgd[%p] pmd[%p] pte[%p] write requested but not given: 0x%lx\n",
		       address, pgd, pmd, ptep, pte_val(*ptep));
		/* Fall through, not as critical */
	}

	fault_printk(FAULT_DBG_TRACE, "FAULT[%d]: ti[%p], missqw0=%08lx, missqw1=%08lx, resolved!\n", 
	       raw_smp_processor_id(), (void *)current_thread_info(), missqw0, missqw1);
	return 1;
}
#endif

/*
 * fault_is_priv()
 *	Return true if the fault is a privilege violation.
 */
STATIC int fault_is_priv(struct pt_regs *regs,
			 unsigned long missqw0, 
			 unsigned long missqw1,
			 pgd_t *pgd)
{
	pte_t *ptep;
	unsigned long address;

	/*
	 * If the PTE is a supervisory PTE and we are in user_mode()
	 * declare this as a privilege violation.
	 */
	address =  (unsigned long)(MMU_MISSQW1_VPN_GET(missqw1) << MMU_VPN_SHIFT);
	ptep = fault_pte(pgd, address);
	if (!ptep) {
		return 0;
	}

	if (!pte_present(*ptep)) {
		return 0;
	}

	if (user_mode(regs) && ((pte_val(*ptep) & L_PTE_USER) == 0)) {
		fault_printk(FAULT_DBG_ERROR, "FAULT[%d]: user_mode and address: %08lx, pte[%p]=%lx\n",
			     raw_smp_processor_id(),
			     (unsigned long)address,
			     ptep,
			     pte_val(*ptep));
		return 1;
	}
	return 0;
}

/*
 * fault_access_type()
 *	Return the VM flags related information for this access type.
 *
 */
STATIC unsigned long fault_access_type(unsigned long missqw0, unsigned long missqw1)
{
	/*
	 * If the missqw1 src is a 0, this is an ifetch failure.
	 */
	unsigned long src = MMU_MISSQW1_SRC_GET(missqw1);
	if (src == 0) {
		return VM_EXEC;
	}
	
	return (MMU_MISSQW0_OP_GET(missqw0) ? VM_WRITE : VM_READ);
}

/*
 * fault_as_soft_tlb()
 *	If the fault is just a miss, re-write the entry using a new ASID.
 *
 * If we have faulted here because this is a miss, the Software TLB
 * needs us to assign a new ASID value and then write the entry.
 */
STATIC int fault_as_soft_tlb(struct mm_struct *mm, unsigned long address, unsigned long missqw0, unsigned long missqw1)
{
	pte_t *ptep;
	unsigned char type;

	/*
	 * If this is not a miss, we need to generate a fault.
	 */
	type = (unsigned char)MMU_MISSQW0_TYPE_GET(missqw0);
	if (unlikely(type)) {
		return 0;
	}

	ptep = fault_pte(mm->pgd, address);
	if (!ptep) {
		return 0;
	}

	if (unlikely(pte_none(*ptep)) ||
	    (unlikely(pte_bad(*ptep))) ||
	    (unlikely(pte_present(*ptep) == 0))) {
		return 0;
	}

	/*
	 * Assign a new ASID to the mm context before we write the entry.
	 */
	context_assign(mm);
	tlb_update_entry(MMU_MISSQW0_SRC_GET(missqw0), mm->context.asid, address, *ptep);
	return 1;
}

/*
 * do_page_fault()
 *	Handle all page faults in the system.
 */
asmlinkage void do_page_fault(struct pt_regs *regs, 
			      unsigned long missqw0, 
			      unsigned long missqw1)
{
	struct task_struct *tsk;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	unsigned long acctyp;
	const struct exception_table_entry *fix;
	pgd_t *pgd;
	unsigned long address;
	int user = 0;
	int fault = 0;

	/*
	 * We should be able to handle an interrupt while resolving
	 * a fault; so enable interrupts now.
	 */
	raw_local_irq_enable();
	address = MMU_MISSQW1_VPN_GET(missqw1) << MMU_VPN_SHIFT;

	fault_printk(FAULT_DBG_TRACE, "FAULT[%d]: ti[%p], missqw0=%08lx, missqw1=%08lx, addr=%08lx\n",
	       raw_smp_processor_id(), (void *)current_thread_info(), missqw0, missqw1, address);

	/*
	 * See if we have a valid mm and what mode the PC was in.
	 */
	tsk = current;
	mm = tsk->mm;
	user = user_mode(regs);

	/*
	 * Check that we have an mm structure.
	 */
	if (!mm) {
		printk(KERN_CRIT "FAULT[%d]: page fault without task mm structure\n", 
		       raw_smp_processor_id());
		goto no_context;
	}

	/*
	 * Make sure that our root page directory handed to use by the
	 * hardware is the same as for this task. If not, we failed to
	 * properly update the PGD in switch_mm() or we have a spurious
	 * hardware fault for a different PGD.  Because of the PIC
	 * preempting cache access it is possible to get 2 writes in the
	 * tlb miss queue for the same miss.  This means that by the
	 * time we arrive here on the 2nd request, it is possible that
	 * this thread is not running the same task.
	 */
	pgd = (pgd_t *)(MMU_MISSQW0_PGD_GET(missqw0) << MMU_MISSQW0_PGD_SHIFT);
	pgd = phys_to_virt(pgd);
	if (mm->pgd != pgd) {
		raw_local_irq_disable();
		return;
	}

	/*
	 * During rollover, the fault code serves as the soft tlb since
	 * the soft tlb does not know any of the Linux data structures.
	 * During the rollover, we need to use the ASID from the mm
	 * structure and not the one given by hardware as that ASID
	 * could be out of sync.
	 */
	if (fault_as_soft_tlb(mm, address, missqw0, missqw1)) {
		raw_local_irq_disable();
		return;
	}

	/*
	 * If we're in an atomic, we must not take a fault.
	 */
	if (in_atomic()) {
		printk(KERN_CRIT "FAULT[%d]: page fault in an atomic: preempt_count=%x\n", 
		       raw_smp_processor_id(), preempt_count());
		goto no_context;
	}


	/*
	 * See if the faulting address is within the task address space.
	 */
	if (address > TASK_SIZE) {
		printk(KERN_CRIT "FAULT[%d]: page fault beyond user space\n", raw_smp_processor_id());
		goto no_context;
	}

	/* 
	 * The privilege violations are reported as part of the MMU miss information 
	 * and not as a trap (as is done with other architectures).  Thus we must
	 * check for priv violations and report either a user mode failure
	 * or kill the kernel.
	 */
	down_read(&mm->mmap_sem);
	if (fault_is_priv(regs, missqw0, missqw1, pgd)) {
		fault_printk(FAULT_DBG_ERROR, "FAULT[%d]: privilege violation\n", raw_smp_processor_id());
		goto bad_area;
	}

	/*
	 * Read the VMA list and find if we have a VMA covering this
	 * fault address.  The find returns the vma where vma->vm_end
	 * > address.  Make sure to check the start!
	 *
	 * Make sure that all paths release the semaphore.
	 */
	vma = find_vma(mm, address);
	if (!vma) {
		fault_printk(FAULT_DBG_ERROR, "FAULT[%d]: unable to find address: %lx\n",
		       raw_smp_processor_id(), address);
		goto bad_area;
	}

	/*
	 * Is this in a hole in the address space?  If so, see if the
	 * stack region can be expanded to cover the hole.
	 */
	if (vma->vm_start > address) {
		if (!(vma->vm_flags & VM_GROWSDOWN)) {
			fault_printk(FAULT_DBG_ERROR, "FAULT[%d]: region is not expandable\n",
			       raw_smp_processor_id());
			goto bad_area;
		}
		if (expand_stack(vma, address)) {
			fault_printk(FAULT_DBG_ERROR, "FAULT[%d]: expand_stack failed\n",
			       raw_smp_processor_id());
			goto bad_area;
		}
	}

	/*
	 * We have a valid VMA for this address, now determine the type
	 * of access?
	 */
	acctyp = fault_access_type(missqw0, missqw1);
	if ((vma->vm_flags & acctyp) != acctyp) {
		/*
		 * The access type was not allowed for this region,
		 * thus we now have a bad_area.
		 */
		fault_printk(FAULT_DBG_ERROR, "FAULT[%d]: illegal access type: %lx, %lx\n", 
		       raw_smp_processor_id(), vma->vm_flags & acctyp, acctyp);
		goto bad_area;
	}

	/*
	 * Call generic fault hanlder to resolve the fault.  If Linux
	 * can not resolve the fault, we must terminate instead of
	 * endlessly retrying the fault.
	 */
	fault = handle_mm_fault(mm, vma, address, 
				(acctyp == VM_WRITE) ? FAULT_FLAG_WRITE : 0);
	if (unlikely(fault & VM_FAULT_ERROR)) {
		/*
		 * We hit a shared mapping outside of the file, or some
		 * other thing happened to us that made us unable to
		 * handle the page fault gracefully.
		 */
		if (fault & VM_FAULT_OOM) {
			up_read(&mm->mmap_sem);
			if (user) {
				printk(KERN_CRIT "FAULT[%d]: killing process group: %s\n", 
				       raw_smp_processor_id(), current->comm);
				do_group_exit(SIGKILL);
				raw_local_irq_disable();
				return;
			}
			printk(KERN_CRIT "FAULT[%d]: OOM in kernel space: %s\n", 
			       raw_smp_processor_id(), current->comm);
			goto no_context;
		}

		if (fault & VM_FAULT_SIGBUS) {
			fault_printk(FAULT_DBG_ERROR, "FAULT[%d]: handle_mm_fault returned SIGBUS\n",
			       raw_smp_processor_id());
			goto bad_area;
		}
		printk(KERN_CRIT "Linux changed the definition of VM_FAULT_ERROR: %x\n", fault);
		up_read(&mm->mmap_sem);
		goto no_context;
	}

#if defined(CONFIG_DEBUG_VM)
	/*
	 * Make sure the fault was resolved, if not stop.  This should
	 * only happen during bring-up as it would cause an infinite
	 * fault in real-life.  The primary cause would be different
	 * views of where an address lies in the 2 level page table.
	 */
	if (!fault_is_resolved(regs, missqw0, missqw1)) {
		printk(KERN_CRIT "do_page_fault: fault not resolved\n");
		goto no_context;
	}
#endif

	/*
	 * Count the fault types.
	 */
	if (fault & VM_FAULT_MAJOR) {
		current->maj_flt++;
	} else {
		current->min_flt++;
	}
	up_read(&mm->mmap_sem);
	raw_local_irq_disable();
	return;

bad_area:
	/*
	 * Something tried to access memory that is not mappable.  If
	 * this is a user space request, we will signal the user.  If
	 * this is the kernel, we die (unless we can fix it with the
	 * exception table).
	 */
	up_read(&mm->mmap_sem);
	if (user) {
		struct siginfo si;
		si.si_signo = SIGSEGV;
		si.si_errno = 0;
		si.si_code = SEGV_MAPERR;
		si.si_addr = (void __user *)address;
		force_sig_info(SIGSEGV, &si, tsk);
		raw_local_irq_disable();
		return;
	}

	/* Fall through */

no_context:
	/*
	 * Since we can enter here for both user and non-user mode
	 * without the above check, we need to re-check that we are not
	 * in user mode before trying the exception table.  This
	 * prevents a bogus user program from loading a PC with an
	 * exception table address and trying to get the kernel to do
	 * "funny" things.
	 *
	 * Try and see if the fault is in the exception table.  If found
	 * move the PC to the fixup location; otherwise terminate.
	 */
	if (!user) {
		fault_printk(FAULT_DBG_TRACE, "FAULT[%d]: Looking up: %p\n", 
				       raw_smp_processor_id(), (void *)regs->pc);
		fix = search_exception_tables(regs->pc);
		if (fix) {
			fault_printk(FAULT_DBG_TRACE, "FAULT[%d]: Fixing fault at %p to %p fixup\n", 
				       raw_smp_processor_id(), (void *)regs->pc, (void *)fix->fixup);
			regs->pc = fix->fixup;
			raw_local_irq_disable();
			return;
		}
	}
	fault_terminate(regs, missqw0, missqw1);
	raw_local_irq_disable();
}

/*
 * translate_to_physical()
 *	Translate a given address to physical address by scanning
 *      through the page tables starting at the provided pgd.
 *
 * Inputs: pgd to start the translation from
 *	   address to translate
 *
 * Output: Translated address with offset. 
 * Return will be 0 if a proper pte is not found.
 */
unsigned long translate_to_physical(pgd_t *pgd, unsigned long addr)
{
	pte_t *ptep;

	if (addr >= UBICOM32_FDPIC_UNMAPPED_START) {
		/*
		 * Unmapped address. Just return it.
		 */
		return addr;
	}

	/*
	 * Find the pte for this address.
	 */
	ptep = fault_pte(pgd, addr);
	if (!ptep) {
		return 0;
	}

	if (unlikely(pte_none(*ptep)) ||
	    (unlikely(pte_bad(*ptep))) ||
	    (unlikely(pte_present(*ptep) == 0))) {
		return 0;
	}
	
	/*
	 * We have a valid pte. Extract and return the pfn.
	 */
	return((pte_pfn(*ptep) << PAGE_SHIFT) | (addr & ~PAGE_MASK));
}
