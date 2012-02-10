/*
 * arch/ubicom32/kernel/pic.c
 *   Ubicom32 architecture Linux Device Services Driver Interface
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
 *
 * NOTES:
 *
 * The PIC is a programmable interrupt controller that is written in software.
 * It emulates the behavior of an pic by fielding the interrupts, choosing a
 * victim thread to take the interrupt and forcing that thread to take a context
 * switch to the appropriate interrupt handler.
 *
 * Because traps are treated as just a special class of interrupts, the PIC
 * also handles the processing of traps.
 *
 * Because we compile Linux both UP and SMP, we need the PIC to use
 * architectural locking that is not "compiled out" when compiling UP.  For now,
 * we use the single atomic bit lock.
 */
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/sched.h>

#if defined(CONFIG_PROC_FS)
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#endif

#include <generated/asm-offsets.h>
#include <asm/machdep.h>
#include <asm/traps.h>
#include <asm/thread.h>
#include <asm/range-protect.h>
#include <asm/profile.h>

#if defined(CONFIG_MMU)
#include <asm/tlb.h>
#include <asm/fault.h>
#endif

/*
 * One can not print from the PIC so the best we can do is
 * check a condition and stall all of the threads.
 */
// #define DEBUG_PIC 1
#if defined(DEBUG_PIC)
#define DEBUG_ASSERT(cond) \
	if (!(cond)) { \
		THREAD_STALL; \
	}
#define STATIC
#define STALL_ON_TRAPS_IN_PIC 1
#else
#define DEBUG_ASSERT(cond)
#define STATIC static
#endif

/*
 * Make global so that we can use it in the RFI code in assembly.
 */
unsigned int pic_soft_irq_mask __attribute__((section(".ocm_data")));
EXPORT_SYMBOL(pic_soft_irq_mask);

STATIC unsigned int pic_suspend_mask __attribute__((section(".ocm_data")));
STATIC unsigned int pic_soft_irq __attribute__((section(".ocm_data")));
STATIC unsigned int pic_stack_space[256] __attribute__((section(".ocm_data")));
STATIC unsigned char pic_bits[8] __attribute__((section(".ocm_data"))) = {
	3, 0, 1, 0, 2, 0, 1, 0};

#if defined(CONFIG_MMU)
STATIC unsigned int pic_mmu_irq __attribute__((section(".ocm_data")));
#endif

/*
 * INTERRUPT_COUNT will always be a multiple of (1 << PIC_IRQ_SHIFT).
 */
#define PIC_IRQ_SHIFT 5
#define PIC_IRQ_WORDS (INTERRUPT_COUNT >> PIC_IRQ_SHIFT)

#define PIC_IRQ_MASK ((1 << PIC_IRQ_SHIFT) - 1)
#define PIC_IRQ_WORD(x) ((x) >> PIC_IRQ_SHIFT)
#define PIC_IRQ_BIT(x) ((x) & PIC_IRQ_MASK)

/*
 * Use a power of 2.
 */
#define PIC_NESTED_COUNT 4
#define PIC_NESTED_MASK ((1 << PIC_NESTED_COUNT) - 1)

/*
 * The counters below roll over in 32 bits so thay are not true totals
 * but they are useful for understanding the interrupt behavior of
 * things.
 */
STATIC struct pic_register_bank {
	volatile unsigned int enabled[PIC_IRQ_WORDS];
	volatile unsigned int mask[PIC_IRQ_WORDS];
	unsigned int attempts;
	unsigned int retry_nocpus;
	unsigned int retry_global;
	unsigned int retry_local;
	unsigned int retry_mmuio;
	unsigned int interrupts;
	unsigned int traps;
	unsigned int faults;
	unsigned int backout;
	unsigned int nested[PIC_NESTED_COUNT];
} pic_bank __attribute__((section(".ocm_data")));

enum pic_ctx_cause {
	PIC_INTERRUPT,
	PIC_TRAP,
#if defined(CONFIG_MMU)
	PIC_PAGE_FAULT,
#endif
};

/*
 * Which thread/cpu are we?
 */
STATIC int pic_tid __attribute__((section(".ocm_data"))) = -1;

#if defined(CONFIG_IRQSTACKS)
/*
 * per-CPU IRQ stacks (thread information and stack)
 *
 * NOTE: Do not use DEFINE_PER_CPU() as it makes it harder
 * to find the location of ctx from assembly language.
 */
union irq_ctx {
	struct thread_info      tinfo;
	u32                     stack[THREAD_SIZE/sizeof(u32)];
};
STATIC union irq_ctx *percpu_irq_ctxs[NR_CPUS];

/*
 *  Storage for the interrupt stack.
 */
#if !defined(CONFIG_IRQSTACKS_USEOCM)
STATIC char percpu_irq_stacks[(NR_CPUS * THREAD_SIZE) + (THREAD_SIZE - 1)];
#else
/*
 *  For OCM, the linker will ensure that space is allocated for the stack
 *  see (vmlinux.lds.S)
 */
STATIC char percpu_irq_stacks[];
#endif

#endif

/*
 * Save trap IRQ because we need to un-suspend if it gets set.
 */
STATIC unsigned int pic_trap_irq __attribute__((section(".ocm_data")));

/*
 * ret_from_interrupt_to_kernel
 *	Just restore the context and do nothing else.
 */
asmlinkage void ret_from_interrupt_to_kernel(void)__attribute__((naked));

/*
 * ret_from_interrupt_to_user
 *	Call scheduler if needed. Just restore the context.
 */
asmlinkage void ret_from_interrupt_to_user(void)__attribute__((naked));

#ifdef DEBUG_PIC
u32_t old_sp, old_pc, old_a0, old_a5, old_a3;
struct pt_regs copy_regs, *copy_save_area;
#endif

int __user_mode(unsigned long sp)
{
	u32_t saved_stack_base = sp & ~(ASM_THREAD_SIZE - 1);

#if defined(CONFIG_IRQSTACKS_USEOCM)
	if ((union irq_ctx *)saved_stack_base == percpu_irq_ctxs[smp_processor_id()]) {
		/*
		 *  On the interrupt stack.
		 */
		return 0;
	}
#endif
	if (!(u32_t)current) {
		return 0;
	}
	return saved_stack_base != ((u32_t)current->stack);
}

/*
 * pic_thread_irq_disable()
 *	Disable interrupts for the specified thread.
 */
STATIC void pic_thread_irq_disable(unsigned int tid)
{
	unsigned int mask = (1 << tid);

	asm volatile (
	"	or.4	scratchpad1, scratchpad1, %0	\n\t"
		:
		: "d" (mask)
		: "cc"
	);
}

/*
 * pic_thread_get_interrupts()
 *	Get the interrupt state for all threads.
 */
STATIC unsigned long pic_thread_get_interrupts(void)
{
	unsigned long ret = 0;
	asm volatile (
	"	move.4	%0, scratchpad1	\n\t"
		: "=r" (ret)
		:
	);
	return ret;
}


/*
 * pic_globals_locked
 *	Return true if any global locks are held.
 */
STATIC int pic_globals_locked(void)
{
	unsigned long global_locks = 0;
#if (UBICOM32_ARCH_VERSION <= 4)
	asm volatile (
	"	lsr.4	%0, scratchpad1, #16	\n\t"
		: "=d" (global_locks)
		:
		: "cc"
	);
#else
	/*
	 * For Version 5, we add in the sysret registers (see
	 * ret_from_interrupt for example usage).
	 */
	asm volatile (
	"	lsr.4	%0, scratchpad1, #16	\n\t"
	"	or.4	%0, scratchpad5, %0	\n\t"
		: "=d" (global_locks)
		:
		: "cc"
	);
#endif
	return (int)global_locks;

}

/*
 * pic_emulate_and_run()
 *	Emulate the instruction and then set the thread to run.
 */
/*
 * pic_preemptive_context_save()
 *	save thread context from another hardware thread.  The other thread must
 *	be stalled.
 *
 * TODO: Since the regs data structure is on the stack, we either need to use
 * an unmapped pointer to the data or we need to move the save/restore to the
 * logical CPU and only save a minimal state here.
 */
STATIC inline void pic_preemptive_context_save(unsigned int tid,
						struct pt_regs *regs)
{
	/*
	 * Save the current state of the specified thread
	 */
	asm volatile (
	"       move.4  a3, %0					\n\t"

		/* set src1 from the target thread */
	"       move.4  csr, %1					\n\t"
	"	setcsr_flush 0					\n\t"
	"	setcsr_flush 0					\n\t"

		/* copy state from the other thread */
	"       move.4  "D(PT_D0)"(a3), d0			\n\t"
	"       move.4  "D(PT_D1)"(a3), d1			\n\t"
	"       move.4  "D(PT_D2)"(a3), d2			\n\t"
	"       move.4  "D(PT_D3)"(a3), d3			\n\t"
	"       move.4  "D(PT_D4)"(a3), d4			\n\t"
	"       move.4  "D(PT_D5)"(a3), d5			\n\t"
	"       move.4  "D(PT_D6)"(a3), d6			\n\t"
	"       move.4  "D(PT_D7)"(a3), d7			\n\t"
	"       move.4  "D(PT_D8)"(a3), d8			\n\t"
	"       move.4  "D(PT_D9)"(a3), d9			\n\t"
	"       move.4  "D(PT_D10)"(a3), d10			\n\t"
	"       move.4  "D(PT_D11)"(a3), d11			\n\t"
	"       move.4  "D(PT_D12)"(a3), d12			\n\t"
	"       move.4  "D(PT_D13)"(a3), d13			\n\t"
	"       move.4  "D(PT_D14)"(a3), d14			\n\t"
	"       move.4  "D(PT_D15)"(a3), d15			\n\t"
	"       move.4  "D(PT_A0)"(a3), a0			\n\t"
	"       move.4  "D(PT_A1)"(a3), a1			\n\t"
	"       move.4  "D(PT_A2)"(a3), a2			\n\t"
	"       move.4  "D(PT_A3)"(a3), a3			\n\t"
	"       move.4  "D(PT_A4)"(a3), a4			\n\t"
	"       move.4  "D(PT_A5)"(a3), a5			\n\t"
	"       move.4  "D(PT_A6)"(a3), a6			\n\t"
	"       move.4  "D(PT_SP)"(a3), a7			\n\t"
	"       move.4  "D(PT_ACC0HI)"(a3), acc0_hi		\n\t"
	"       move.4  "D(PT_ACC0LO)"(a3), acc0_lo		\n\t"
	"       move.4  "D(PT_MAC_RC16)"(a3), mac_rc16		\n\t"
	"       move.4  "D(PT_ACC1HI)"(a3), acc1_hi		\n\t"
	"       move.4  "D(PT_ACC1LO)"(a3), acc1_lo		\n\t"
	"       move.4  "D(PT_SOURCE3)"(a3), source3		\n\t"
	"       move.4  "D(PT_INST_CNT)"(a3), inst_cnt		\n\t"
	"       move.4  "D(PT_CSR)"(a3), csr			\n\t"
	"       move.4  "D(PT_DUMMY_UNUSED)"(a3), #0		\n\t"
	"       move.4  "D(PT_INT_MASK0)"(a3), int_mask0	\n\t"
	"       move.4  "D(PT_INT_MASK1)"(a3), int_mask1	\n\t"
	"       move.4  "D(PT_TRAP_CAUSE)"(a3), trap_cause	\n\t"
	"       move.4  "D(PT_PC)"(a3), pc			\n\t"
	"	move.4	"D(PT_PREVIOUS_PC)"(a3), previous_pc	\n\t"
#if defined(IP8000)
	"       move.4  "D(PT_INT_MASK2)"(a3), int_mask2	\n\t"
#endif
#if (UBICOM32_ARCH_VERSION >= 5)
	"	move.4	"D(PT_UCSR)"(a3), ucsr			\n\t"
#endif

		/* disable csr thread select */
	"       movei   csr, #0					\n\t"
	"       setcsr_flush 0					\n\t"
		:
		: "r" (regs->dn), "d" ((tid << 9) | (1 << 8))
		: "a3"
	);

#if defined(CONFIG_MMU)
	regs->pgd = ubicom32_read_reg((volatile unsigned int *)MMU_TNUM_PGD(tid));
	regs->asids = ubicom32_read_reg((volatile unsigned int *)MMU_TNUM_ASIDS(tid));
	regs->asid_mask = ubicom32_read_reg((volatile unsigned int *)MMU_TNUM_ASID_MASK(tid));
	regs->asid_cmp = ubicom32_read_reg((volatile unsigned int *)MMU_TNUM_ASID_CMP(tid));
#endif
}

/*
 * pic_rotate_threads()
 *	Simple round robin algorithm for choosing the next cpu
 */
STATIC int pic_rotate_threads(unsigned long cpus)
{
	static int pic_nextbit __attribute__((section(".ocm_data")));
	int thisbit;

	/*
	 * Move the interrupts down so that we consider interrupts from where
	 * we left off, then take the interrupts we would lose and move them
	 * to the top half of the interrupts value.
	 */
	cpus = (cpus >> pic_nextbit) | (cpus << ((sizeof(cpus) * 8) - pic_nextbit));

	/*
	 * 50% of the time we won't take this at all and then of the cases where
	 * we do about 50% of those we only execute once.
	 */
	if (!(cpus & 0xffff)) {
		pic_nextbit += 16;
		cpus >>= 16;
	}

	if (!(cpus & 0xff)) {
		pic_nextbit += 8;
		cpus >>= 8;
	}

	if (!(cpus & 0xf)) {
		pic_nextbit += 4;
		cpus >>= 4;
	}

	pic_nextbit += pic_bits[cpus & 0x7];
	thisbit = (pic_nextbit & ((sizeof(cpus) * 8) - 1));
	pic_nextbit = (thisbit + 1) & ((sizeof(cpus) * 8) - 1);
	DEBUG_ASSERT(thisbit < THREAD_COUNT);
	return thisbit;
}

/*
 * pic_backout_of_irq()
 *	Back out of an interrupt that was not delivered to the specified threads.
 *
 * One way or the other this interrupt is not being processed, make sure
 * that it is reset.  We are not going to call irq_end_vector() so
 * unmask the interrupt.
 *
 * Note: We do not increment the back out count inside because this
 * function is also called in the normal course of processing traps
 * and faults.  We increment the back out count at the call point
 * when we know the caller is actually backing out because they
 * cannot deliver.
 */
STATIC void pic_backout_of_irq(int vector, unsigned long tid_mask)
{
#if defined(CONFIG_SMP)
	/*
	 * Reset the IPI request for these threads if this is the IPI
	 * vector.
	 */
	if (unlikely(vector == smp_ipi_irq)) {
		smp_reset_ipi(tid_mask);
	}
#endif
	pic_unmask_vector(vector);
}

#if defined(CONFIG_IRQSTACKS)
/*
 * pic_choose_savearea_and_returnvec()
 *	Test our current state (user, kernel, interrupt) and set things up.
 *
 * This version of the function uses 3 stacks and nests interrupts
 * on the interrupt stack.
 */
STATIC struct pt_regs *pic_choose_savearea_and_returnvec(thread_t tid, u32_t linux_sp, u32_t *pvec)
{
	struct pt_regs *save_area;
	u32_t masked_linux_sp = linux_sp & ~(THREAD_SIZE - 1);
	struct thread_info * ti= (struct thread_info *)sw_ksp[tid];

#if defined(CONFIG_SMP)
	union irq_ctx *icp = percpu_irq_ctxs[ti->cpu];
#else
	union irq_ctx *icp = percpu_irq_ctxs[0];
#endif

	if (masked_linux_sp == (u32_t)icp) {
		/*
		 * Fault/Interrupt occurred while on the interrupt stack.
		 */
		save_area = (struct pt_regs *)((char *)linux_sp - sizeof(struct pt_regs) - 8);
		*pvec = (u32_t)(&ret_from_interrupt_to_kernel);
	} else {
		/*
		 *  Fault/Interrupt occurred while on user/kernel stack.  This is a new
		 *  first use of the interrupt stack.
		 */
		save_area = (struct pt_regs *) ((char *)icp + sizeof(icp->stack) - sizeof(struct pt_regs) - 8);
		if (masked_linux_sp == (u32_t)ti) {
			*pvec  = (u32_t)(&ret_from_interrupt_to_kernel);
		} else {
			*pvec  = (u32_t)(&ret_from_interrupt_to_user);
		}

		/*
		 * Because the softirq code will execute on the "interrupt" stack, we
		 * need to maintain the knowledge of what "task" was executing on the
		 * cpu.  This is done by copying the thread_info->task from the cpu
		 * we are about to context switch into the interrupt contexts thread_info
		 * structure.
		 */
		icp->tinfo.task = ti->task;
		icp->tinfo.preempt_count =
				(icp->tinfo.preempt_count & ~SOFTIRQ_MASK) |
				(ti->preempt_count & SOFTIRQ_MASK);
		icp->tinfo.interrupt_nesting = 0;
	}
	save_area->nesting_level = icp->tinfo.interrupt_nesting;
	pic_bank.nested[save_area->nesting_level & PIC_NESTED_MASK]++;
	return save_area;
}

#else

/*
 * pic_choose_savearea_and_returnvec()
 *	Test our current state (user, kernel, interrupt) and set things up.
 *
 * The version of the function uses just the user & kernel stack and
 * nests interrupts on the existing kernel stack.
 */
STATIC struct pt_regs *pic_choose_savearea_and_returnvec(thread_t tid, u32_t linux_sp, u32_t *pvec)
{
	struct pt_regs *save_area;
	u32_t masked_linux_sp;
	struct thread_info *ti = (struct thread_info *)sw_ksp[tid];

	/*
	 * The PIC uses physical addresses for the SP and TI.
	 */
	linux_sp = virt_to_phys(linux_sp);
	ti = (struct thread_info *)virt_to_phys(ti);
	masked_linux_sp = linux_sp & ~(THREAD_SIZE - 1);
	if (masked_linux_sp == (u32_t)ti) {
		/*
		 * Fault/Interrupt occurred while on the kernel stack.
		 */
		save_area = (struct pt_regs *)((char *)linux_sp - sizeof(struct pt_regs) - 8);
		*pvec = (u32_t) (&ret_from_interrupt_to_kernel);
	} else {
		/*
		 *  Fault/Interrupt occurred while on user stack.
		 */
		ti->interrupt_nesting = 0;
		save_area = (struct pt_regs *)((u32_t)ti + THREAD_SIZE - sizeof(struct pt_regs) - 8);
		*pvec  = (u32_t) (&ret_from_interrupt_to_user);
	}

	save_area->nesting_level = ti->interrupt_nesting;
	pic_bank.nested[save_area->nesting_level & PIC_NESTED_MASK]++;
	return save_area;
}
#endif

#if (UBICOM32_ARCH_VERSION <= 4)
/*
 * pic_execute_preempt()
 *	Actually switch the registers for tid to the new values and make it run.
 */
STATIC inline void pic_execute_preempt(thread_t tid, 
				  struct pt_regs * save_area,
				  unsigned int arg1, 
				  unsigned int arg2, 
				  unsigned int arg3, 
				  unsigned int execute_vector,
				  unsigned int return_vector)
{
	u32_t thread_mask = (1 << tid);
	u32_t write_csr = (tid << 15) | (1 << 14);

	/*
	 *	Set pc to execute_vector
	 *	Set d0 to save_area
	 *	Set d1 to arg1
	 *	Set d2 to arg2
	 *	Set d3 to arg3
	 *	Set a5 to the proper return vector (user or kernel code).
	 */
	asm volatile (
	"	setcsr	%0			\n\t"
	"	setcsr_flush 0			\n\t"
	"	move.4	d0, %1			\n\t" /* d0 = save_area */
	"	move.4	d1, %5			\n\t" /* d1 = arg1 */
	"	move.4	d2, %6			\n\t" /* d2 = arg2 */
	"	move.4	d3, %7			\n\t" /* d3 = arg3 */
	"	move.4	sp, %1			\n\t" /* sp = save_area */
	"	move.4	a5, %2			\n\t" /* a5 = return_vector */
	"	move.4	pc, %3			\n\t" /* pc = execute_vector */
	"	setcsr	#0			\n\t"
	"	setcsr_flush 0			\n\t"
#ifdef CONFIG_PROTECT_KERNEL
	"	enable_kernel_ranges %4		\n\t"
#endif
	"	move.4	mt_dbg_active_set, %4	\n\t" /* Activate thread even if
						       * in dbg/fault state */
	"	move.4	mt_active_set, %4	\n\t" /* Restart target
						       * thread. */
		:
		: "r" (write_csr), "r" (save_area),
		  "r" (return_vector), "r" (execute_vector),
		  "d" (thread_mask), "r" (arg1), "r" (arg2), "r" (arg3)
		: "cc"
	);
}
#else
/*
 * pic_execute_preempt()
 *	Actually switch the registers for tid to the new values and make it run.
 */
STATIC inline void pic_execute_preempt(thread_t tid, 
				  struct pt_regs *save_area,
				  unsigned int arg1, 
				  unsigned int arg2, 
				  unsigned int arg3, 
				  unsigned int execute_vector,
				  unsigned int return_vector,
				  unsigned int saved_csr)
{
	u32_t thread_mask = (1 << tid);
	u32_t write_csr = (tid << 15) | (1 << 14);
	unsigned long newsp = (unsigned long)phys_to_virt(save_area);

	/*
	 *	Set pc to execute_vector
	 *	Set d0 to save_area
	 *	Set d1 to arg1
	 *	Set d2 to arg2
	 *	Set d3 to arg3
	 *	Set a5 to the proper return vector (user or kernel code).
	 *	Set csr to saved_csr
	 */
	asm volatile (
	"	setcsr	%0			\n\t"
	"	setcsr_flush 0			\n\t"
	"	move.4	d0, %1			\n\t" /* d0 = save_area */
	"	move.4	d1, %5			\n\t" /* d1 = arg1 */
	"	move.4	d2, %6			\n\t" /* d2 = arg2 */
	"	move.4	d3, %7			\n\t" /* d3 = arg3 */
	"	move.4	sp, %1			\n\t" /* sp = save_area */
	"	move.4	a5, %2			\n\t" /* a5 = return_vector */
	"	move.4	pc, %3			\n\t" /* pc = execute_vector */
	"	move.4	csr, %8			\n\t" /* Set thread 0 csr from saved_csr */
	"	setcsr	#0			\n\t"
	"	setcsr_flush 0			\n\t"
#if defined(CONFIG_FPU)
	"	move.4  mt_fp_flush, %4		\n\t" /* Flush FPU */
#endif
	"       move.4  mt_blocked_clr, %4      \n\t" /* Clear any left over thread block */
	"	move.4	mt_dbg_active_set, %4	\n\t" /* Activate thread even if
						       * in dbg/fault state */
	"	move.4	mt_active_set, %4	\n\t" /* Restart target
						       * thread. */
		:
		: "r" (write_csr), "r" (newsp),
		  "r" (return_vector), "r" (execute_vector),
		  "d" (thread_mask), "r" (arg1), "r" (arg2), "r" (arg3), "r" (saved_csr)
		: "cc"
	);
}
#endif

/*
 * pic_preempt_thread()
 *	Force context switch a logical CPU to execute the vector associated with cause
 */
STATIC void pic_preempt_thread(enum pic_ctx_cause cause, thread_t tid, 
			      unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
	u32_t thread_mask = (1 << tid);
	u32_t read_csr = ((tid << 9) | (1 << 8));
	u32_t execute_vector = 0;
	unsigned int frame_type = 0;
	u32_t linux_sp;
	u32_t return_vector;
	struct pt_regs *save_area, *regs;

	/*
	 * The thread we are context-switching to must not be enabled.
	 */
	DEBUG_ASSERT(!thread_is_enabled(tid));

	/*
	 * Changing the CSR is expensive so we make the change
	 * only once.  We sometimes collect information that is
	 * only needed for once cause and not others.
	 */
	asm volatile (
	"	setcsr	%1		\n\t"
	"	setcsr_flush 0		\n\t"
	"	setcsr_flush 0		\n\t"
	"	move.4	%0, SP		\n\t"
	"	setcsr	#0		\n\t"
	"	setcsr_flush 0		\n\t"
		: "=r" (linux_sp)
		: "m" (read_csr)
	);

	/*
	 * Set the function to execute and the frame type.
	 */
	if (likely(cause == PIC_INTERRUPT)) {
		execute_vector = (u32_t)(&do_IRQ);
		frame_type = UBICOM32_FRAME_TYPE_INTERRUPT;
		pic_bank.interrupts++;
	} else if (cause == PIC_TRAP) {
		asm volatile (
		"	move.4	MT_BREAK_CLR, %0\n\t"
		"	move.4	MT_TRAP_CLR, %0\n\t"
			: 
			: "r" (thread_mask)
		);
		execute_vector = (u32_t)(&do_trap);
		frame_type = UBICOM32_FRAME_TYPE_TRAP;
		pic_bank.traps++;
	}
#if defined(CONFIG_MMU)
	else if (cause == PIC_PAGE_FAULT) {
		ubicom32_write_reg((volatile int *)MMU_RESTART, arg1);
		execute_vector = (u32_t)(&do_page_fault);
		frame_type = UBICOM32_FRAME_TYPE_PAGE_FAULT;
		pic_bank.faults++;
	}
#endif

	/*
	 * At this point, we will definitely force this thread to
	 * a new context, show its interrupts as disabled.
	 */
	pic_thread_irq_disable(tid);

	/*
	 * Test our current state (user, kernel, interrupt).  Save the
	 * appropriate data and setup for the return.
	 */
	save_area = pic_choose_savearea_and_returnvec(tid, linux_sp, &return_vector);

	/*
	 *  The pt_regs (save_area) contains the type of thread that we are dealing
	 *  with (KERNEL/NORMAL) and is copied into each pt_regs area.  We get this
	 *  from the current tasks kernel pt_regs area that always exists at the
	 *  top of the kernel stack.
	 */
	regs = (struct pt_regs *)((u32_t)sw_ksp[tid] + THREAD_SIZE - sizeof(struct pt_regs) - 8);
	if (virt_addr_valid(regs)) {
		regs = (struct pt_regs *)virt_to_phys(regs);
	}
	save_area->thread_type = regs->thread_type;

	/*
	 * Preserve the context of the Linux thread.
	 */
	pic_preemptive_context_save(tid, save_area);

#if (UBICOM32_ARCH_VERSION >= 5)
	/*
	 * We use the sysret as the last instruction in the return_from_interrupt_xxxx sequences while
	 * we are restoring process state. Because of this we have to shift the state of CSR_PRIV
	 * bit into CSR_PREV_PRIV bit location. Aftet the sysret instruction executes this state will
	 * moved back into the CSR_PRIV bit location.
	 */
	if (save_area->csr & ( 1<< UBICOM32_CSR_PRIV_BIT)) {
		/*
		 * PREV_PRIV bit in saved csr.
		 */
		save_area->csr |= (1<< UBICOM32_CSR_PREV_PRIV_BIT);
	} else {
		/*
		 * Clear PREV_PRIV bit in saved csr.
		 */
		save_area->csr &= ~(1 << UBICOM32_CSR_PREV_PRIV_BIT);
	}

	/*
	 * Clear the state of CSR_PRIV bit in the saved csr. During the process restoration
	 * the saved csr value will be copied over into an active csr register. We do not want
	 * to go into user mode at that point in the kernel as there more work to do.
	 * This is a safe thing to do because the syscall_exit will force setting of the CSR_PREV_BIT bit 
	 * to guarantee that the system will be executing in user mode after execution of syscall_exit.
	 */
	save_area->csr &= ~(1 << UBICOM32_CSR_PRIV_BIT);
#endif
	/*
	 * Load the frame_type into the save_area.
	 */
	save_area->frame_type = frame_type;

#if defined(STALL_ON_TRAPS_IN_PIC)
	/*
	 * Before we get backtrace and showing stacks working well, it sometimes
	 * helps to enter the debugger when a trap occurs before we change the
	 * thread to handle the fault.  This optional code causes all threads to
	 * stop on every trap frame.  One assumes that GDB connected via the
	 * mailbox interface will be used to recover from this state.
	 */
	if (frame_type == UBICOM32_FRAME_TYPE_TRAP) {
		THREAD_STALL;
	}
#endif

#ifdef DEBUG_PIC
	copy_regs = *save_area;
	copy_save_area = save_area;

	old_a0 = save_area->an[0];
	old_a3 = save_area->an[3];
	old_sp = save_area->an[7];
	old_a5 = save_area->an[5];
	old_pc = save_area->pc;
#endif

	/*
	 * Now we have to switch the kernel thread to run the selected function in execute_vector.
	 */
#if (UBICOM32_ARCH_VERSION < 5)
	pic_execute_preempt(tid, save_area, arg1, arg2, arg3, execute_vector, return_vector);
#else
	pic_execute_preempt(tid, save_area, arg1, arg2, arg3, execute_vector, return_vector, save_area->csr);
#endif

#if !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	thread_enable_mask(thread_mask);
#else
	/*
	 * We are restarting a faulted thread. Clear this thread from
	 * the list of threads waiting for do_page_fault servcie via
	 * pic. Restart the thread and let go of the lock.
	 *
	 * TODO: Move the following back to the loop that starts the
	 * preemption process when the TLB support for the IP8K shuttle
	 * part is no longer necessary.
	 */
	UBICOM32_LOCK(MT_EN_LOCK_BIT);
	if (cause == PIC_PAGE_FAULT) {
		tlb_faulted_clear(tid);
	}
	thread_enable_mask_nolock(thread_mask);
	UBICOM32_UNLOCK(MT_EN_LOCK_BIT);
#endif
}

/*
 * pic_deliver_interrupt()
 *	Attempt to deliver the interrupt to one or all of the threads.
 */
STATIC void pic_deliver_interrupt(int vector,
				   unsigned long deliver_to,
				   int all)
{
	unsigned long disabled_threads;
	unsigned long possible_threads;
	unsigned long trapped_threads;
#if defined(CONFIG_MMU)
	unsigned long faulted_threads;
	unsigned long blocked_threads;
#endif

	/*
	 * Track the number of times, we attempt to deliver
	 * any sort of interrupt.
	 */
	pic_bank.attempts++;
retry:
	/*
	 * Disable all of the threads that we might want to send
	 * this interrupt to.
	 */
	DEBUG_ASSERT(deliver_to);
	thread_disable_mask(deliver_to);

	/*
	 * If any threads are in the trap state, we have to service the
	 * trap for those threads first.
	 */
	asm volatile (
	"	move.4	%0, MT_TRAP		\n\t"
		: "=r" (trapped_threads)
		:
	);

	trapped_threads &= deliver_to;
	if (unlikely(trapped_threads)) {
		unsigned long enable_mask;

		/*
		 * all traps will be handled, so clear the trap bit before 
		 * restarting any threads
		 */
	        ubicom32_clear_interrupt(pic_trap_irq);

		/*
		 * Let the remaining untrapped threads, continue.  Do
		 * this before doing anything else.  If not, the PIC
		 * could deadlock with a disabled thread in back out.
		 */
		enable_mask = deliver_to & ~trapped_threads;
		if (enable_mask) {
			thread_enable_mask(enable_mask);
		}

		/*
		 * Back out of this IRQ since we are not delivering the
		 * interrupt.
		 */
		pic_backout_of_irq(vector, deliver_to);

		/*
		 * For the trapped threads force them to handle
		 * a trap.
		 */
		while (trapped_threads) {
			unsigned long tid = ffz(~trapped_threads);
			trapped_threads &= ~(1 << tid);
			pic_preempt_thread(PIC_TRAP, tid, 0, 0, 0);
		}
		return;
	}


#if defined(CONFIG_MMU)
	/*
	 * If any threads are in the fault state, we have to service
	 * the fault for those threads first?  This special faulted 
	 * state must be very short or interrupts would be held off.
	 * For this reason, the fault handler must immediately enable
	 * interrupts.
	 */
	faulted_threads = tlb_faulted_threads();
	faulted_threads &= deliver_to;
	if (unlikely(faulted_threads)) {
		unsigned long enable_mask;

		/*
		 * all faults will be handled, so clear the fault bit before 
		 * restarting any threads
		 */
	        ubicom32_clear_interrupt(pic_mmu_irq);

		/*
		 * Let the remaining untrapped threads, continue.  Do
		 * this before doing anything else.  If not, the PIC
		 * could deadlock with a disabled thread in back out.
		 */
		enable_mask = deliver_to & ~faulted_threads;
		if (enable_mask) {
			thread_enable_mask(enable_mask);
		}

		/*
		 * Back out of this IRQ since we are not delivering the
		 * interrupt.
		 */
		pic_backout_of_irq(vector, deliver_to);

		/*
		 * For the faulted threads force them to handle
		 * the fault.
		 */
		while (faulted_threads) {
			unsigned int missqw0;
			unsigned int missqw1;
			unsigned long tid = ffz(~faulted_threads);
			faulted_threads &= ~(1 << tid);
			tlb_faulted_missq(tid, &missqw0, &missqw1);
#if !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
			tlb_faulted_clear(tid);
#endif
			pic_preempt_thread(PIC_PAGE_FAULT, tid, missqw0, missqw1, 0);
		}
		return;
	}

	/*
	 * Stop the PIC from sending spurious interrupts for the MMU.
	 */
	if (vector == pic_mmu_irq) {
		thread_enable_mask(deliver_to);
		pic_backout_of_irq(vector, deliver_to);
		return;
	}
#endif

	/*
	 * If any of the global locks are held, we can not deliver any
	 * interrupts.  This case has a possible live lock issue where
	 * one or more main threads constantly grabs a global lock
	 * preventing the PIC from delivering any interrupts.  For now
	 * we live with the possibility.
	 */
	if (unlikely(pic_globals_locked())) {
		thread_enable_mask(deliver_to);
		pic_bank.retry_global++;
		while (unlikely(pic_globals_locked())) {
			/*
			 * Do nothing but waste CPU cycles
			 */
		}
		goto retry;
	}

	/*
	 * Check if the threads have per-CPU interrupts disabled?
	 */
	disabled_threads = pic_thread_get_interrupts();
	possible_threads = deliver_to & ~disabled_threads;
	if (unlikely(!possible_threads)) {
		unsigned int scratch __attribute__ ((unused)) = 0;

		thread_enable_mask(deliver_to);
		pic_bank.retry_local++;

#if defined(CONFIG_SMP)
		/*
		 * In the SMP case, we can not wait because 1 CPU might
		 * be sending an IPI to another cpu which is currently
		 * blocked.  The only way to ensure IPI delivery is to
		 * back out and keep trying.  For SMP, we don't sleep
		 * until the interrupts are delivered.
		 */
		pic_backout_of_irq(vector, deliver_to);
		pic_bank.backout++;
		return;
#else
		/*
		 * In the UP case, we have nothing to do so we should wait.
		 *
		 * We assume that all of the PIC irqs are in mask0.
		 *
		 * Since the interrupt masks are "re-loaded" before we
		 * suspend in the outer loop, we do not need to save
		 * them here.
		 *
		 * We test that we were awakened for our specific
		 * interrupts because the pic mask/unmask operations
		 * will force the pic awake even if the interrupt on the
		 * mainline thread is not completed.
		 */
		asm volatile (
		"	move.4	INT_MASK0, %1		\n\t"
		"	move.4	INT_MASK1, #0		\n\t"
#if defined(IP8000)
		"	move.4	INT_MASK2, #0		\n\t"
#endif

		"1:	suspend				\n\t"
		"	move.4	%0, INT_STAT0		\n\t"
		"	and.4	%0, %0, %1		\n\t"
		"	jmpeq.f	1b			\n\t"

		"	move.4	INT_CLR0, %2		\n\t"
			: "+r" (scratch)
			: "d" (pic_suspend_mask), "r" (pic_soft_irq_mask)
			: "cc"
		);
		goto retry;
#endif
	}

#if defined(CONFIG_MMU)
	/*
	 * Theads that are blocked because of the MMU (either the
	 * hardware walker is running or the software walker is running)
	 * can not be interrupted.  To do so would create a race where
	 * the software walker could execute while the thread it is
	 * executing for is still running.  This would mean that the PGD
	 * might be switched out from under the software TLB walker.
	 * This is unacceptable.
	 *
	 * On an additional note, if the stopped thread is currently 
	 * waiting for an I/O blocking region, we can not interrupt
	 * the thread because restarting the thread could cause a re-read
	 * of the blocking region.
	 *
	 * The bus state registers allows us to check the state of the
	 * thread but not to differentiate all of the causes.  We allow
	 * ourselves not to interrupt an OCM blocked thread (in addition
	 * to I/O and MMU) because the event is rare (2 OCM access in a
	 * single instruction).  This means that a thread that is blocked
	 * for OCM, MMU or I/O will not be interrupted by the PIC.
	 */
	blocked_threads = tlb_threads_blocked_not_interruptable();
	possible_threads &= ~blocked_threads;
	if (unlikely(!possible_threads)) {
		/*
		 * Let the blocked threads run and spin around
		 * and try again.
		 */
		thread_enable_mask(deliver_to);
		pic_bank.retry_mmuio++;
		goto retry;
	}
#endif

	/*
	 * Deliver to one CPU.  We pull this case out because ffz is an
	 * expensive operation.  Using a special case for !all ensures
	 * that most interrupts do not suffer this additional overhead.
	 */
	if (!all) {
		/*
		 * Find our victim and then enable everyone else.
		 */
		unsigned long tid = pic_rotate_threads(possible_threads);
		DEBUG_ASSERT((deliver_to & (1 << tid)));
		DEBUG_ASSERT((possible_threads & (1 << tid)));
#if defined(CONFIG_SMP)
		DEBUG_ASSERT(vector != smp_ipi_irq);
#endif

		deliver_to &= ~(1 << tid);
		if (deliver_to) {
			thread_enable_mask(deliver_to);
		}
		pic_preempt_thread(PIC_INTERRUPT, tid, vector, 0, 0);
		return;
	}

	/*
	 * If some of the threads can not take the interrupt, re-enable
	 * them.  If the vector is the smp_ipi_irq we will need to
	 * resend the IPI for those threads.  We do not use the back out
	 * here because we don't want to enable the mask until the
	 * threads that get the IPI are completed.
	 */
	deliver_to &= ~possible_threads;
	if (unlikely(deliver_to)) {
		thread_enable_mask(deliver_to);
#if defined(CONFIG_SMP)
		if (unlikely(vector == smp_ipi_irq)) {
			smp_reset_ipi(deliver_to);
		}
#endif
	}

	/*
	 * Deliver to all possible threads(s).
	 */
	while (possible_threads) {
		unsigned long tid = ffz(~possible_threads);
		possible_threads &= ~(1 << tid);
		pic_preempt_thread(PIC_INTERRUPT, tid, vector, 0, 0);
	}
}

/*
 * pic_snapshot_rotate()
 *	Get rotating next set bit value.
 */
STATIC int pic_snapshot_rotate(unsigned int snapshot[])
{
	static int pic_snapshot_nextword __attribute__((section(".ocm_data")));
	static int pic_snapshot_nextbit __attribute__((section(".ocm_data")));
	int thisword = 0;
	int thisbit = 0;
	int i;
	
	DEBUG_ASSERT(pic_snapshot_nextword < PIC_IRQ_WORDS);
	DEBUG_ASSERT(pic_snapshot_nextbit < (sizeof(unsigned int) * 8));

	for (i = 0; i < PIC_IRQ_WORDS; i++) {
		unsigned int interrupts;

		/*
		 * Choose which word of the snapshot to search.
		 */
		thisword = pic_snapshot_nextword;
		interrupts = snapshot[thisword];

		/*
		 * If no interrupts in this word, go to the next one.
		 */
		if (!interrupts) {
			pic_snapshot_nextword = ((++pic_snapshot_nextword) == PIC_IRQ_WORDS) ? 0 : pic_snapshot_nextword;
			continue;
		}
	
		/*
		 * Move the interrupts down so that we consider interrupts from where
		 * we left off, then take the interrupts we would lose and move them
		 * to the top half of the interrupts value.
		 */
		interrupts = (interrupts >> pic_snapshot_nextbit) |
			(interrupts << ((sizeof(interrupts) * 8) - pic_snapshot_nextbit));
		
		/*
		 * 50% of the time we won't take this at all and then of the cases where
		 * we do about 50% of those we only execute once.
		 */
		if (!(interrupts & 0xffff)) {
			pic_snapshot_nextbit += 16;
			interrupts >>= 16;
		}
		
		if (!(interrupts & 0xff)) {
			pic_snapshot_nextbit += 8;
			interrupts >>= 8;
		}
		
		if (!(interrupts & 0xf)) {
			pic_snapshot_nextbit += 4;
			interrupts >>= 4;
		}
		pic_snapshot_nextbit += pic_bits[interrupts & 0x7];
		thisbit = (pic_snapshot_nextbit & PIC_IRQ_MASK);
		
		/*
		 * Now update the choice for next time.
		 */
		pic_snapshot_nextbit = (thisbit + 1) & PIC_IRQ_MASK;

		/*
		 * Calculate the final bit position.
		 */
		thisbit = (thisword << PIC_IRQ_SHIFT) + thisbit;
		return thisbit;
	}
	
	/*
	 * Never Reached
	 */
	DEBUG_ASSERT(0);
	return 0;
}


/*
 * pic_snapshot_clear_int()
 *	Clear the interrupt from the snapshot.
 */
STATIC void pic_snapshot_clear_int(unsigned int snapshot[], int vector)
{
	unsigned int word = PIC_IRQ_WORD(vector);
	unsigned int mask = (1 << PIC_IRQ_BIT(vector));

	DEBUG_ASSERT(snapshot[word] & mask);
	snapshot[word] &= ~mask;
}

/*
 * pic_snapshot_has_interrupts()
 *	Does the snapshot still contain any interrupts.
 */
STATIC unsigned int pic_snapshot_has_interrupts(unsigned int snapshot[])
{
	unsigned int ret = snapshot[0];

#if (INTERRUPT_COUNT > 32)
	ret |= snapshot[1];
#endif

#if (INTERRUPT_COUNT > 64)
	ret |= snapshot[2];
#endif
	return ret;
}

/*
 * pic_snapshot()
 *	Read the hardware status registers.
 *
 * We only care about interrupts that we have been told to care
 * about.  The interrupt must be enabled, unmasked, and have
 * occurred in the hardware.
 */
STATIC void pic_snapshot(unsigned int snapshot[])
{
	pic_lock_acquire();
	asm volatile (
	"	and.4	%[ss0], INT_STAT0, %[en0]	\n\t"
	"	and.4	%[ss0], %[ss0], %[mask0]	\n\t"
	"	and.4	%[ss1], INT_STAT1, %[en1]	\n\t"
	"	and.4	%[ss1], %[ss1], %[mask1]	\n\t"
		: [ss0]"=&r" (snapshot[0]), [ss1]"=&r" (snapshot[1])
		: [en0]"d" (pic_bank.enabled[0]), [mask0]"d" (pic_bank.mask[0]),
		  [en1]"d" (pic_bank.enabled[1]), [mask1]"d" (pic_bank.mask[1])
		: "memory", "cc"
	);

#if (UBICOM32_ARCH_VERSION >= 5)
	asm volatile (
	"	and.4	%[ss2], INT_STAT2, %[en2]	\n\t"
	"	and.4	%[ss2], %[ss2], %[mask2]	\n\t"
		: [ss2]"=&r" (snapshot[2])
		: [en2]"d" (pic_bank.enabled[2]), [mask2]"d" (pic_bank.mask[2])
		: "memory", "cc"
	);
#endif
	pic_lock_release();

}

/*
 * pic_set_masks()
 *	Set the pic's mask registers.
 */
STATIC void pic_set_masks(void)
{
	pic_lock_acquire();

	asm volatile (
	"	move.4 INT_MASK0, %0	\n\t"
	"	move.4 INT_MASK1, %1	\n\t"
		:
		: "U4" (pic_bank.mask[0]), "U4" (pic_bank.mask[1])
	);

#if defined(IP8000)
	asm volatile (
	"	move.4 INT_MASK2, %0	\n\t"
		:
		: "U4" (pic_bank.mask[2])
	);
#endif
	pic_lock_release();
}

/*
 * pic()
 *	This thread acts as the interrupt controller for Linux.
 */
STATIC void pic(void *arg)
{
	unsigned int snapshot[PIC_IRQ_WORDS];
	unsigned long cpus;

#if !defined(CONFIG_SMP)
	/*
	 * In a non-smp configuration, we can not use the cpu(s) arrays because
	 * there is not a 1-1 correspondence between cpus(s) and our threads.
	 * Thus we must get a local idea of the mainline threads and use the
	 * one and only 1 set as the victim.  We do this once before the pic
	 * loop.
	 *
	 * In the SMP case, we will use the cpu(s) map to determine which cpu(s)
	 * are valid to send interrupts to.
	 */
	int victim = 0;
	unsigned int mainline = thread_get_mainline();
	if (mainline == 0) {
		/*
		 * We can't do anything else since we are not a proper Linux
		 * CPU.  You can't call panic().
		 */
		THREAD_STALL;
		return;
	}
	victim = ffz(~mainline);
	cpus = (1 << victim);
#endif

	while (1) {
		/*
		 * If one changes this code not to reload the INT_MASK(s), you
		 * need to know that code in the lock waiting above does not
		 * reset the MASK registers back; so that code will need to be
		 * changed.
		 */
		pic_set_masks();
		thread_suspend();

		/*
		 * Read the interrupt status registers masking for only those
		 * we are about.
		 */
		pic_snapshot(snapshot);

		/*
		 * For each interrupt in the "snapshot" we will mask the
		 * interrupt handle the interrupt (typically calling do_IRQ()).
		 */
		while (pic_snapshot_has_interrupts(snapshot)) {
			int all = 0;
			int vector = pic_snapshot_rotate(snapshot);
			pic_snapshot_clear_int(snapshot, vector);

			/*
			 * Now mask off this vector so that the PIC
			 * ignores it until it is acknowledged.  The
			 * interrupt is unmasked by desc->chip->end()
			 * function in the per chip generic interrupt
			 * handling code (arch/ubicom32/kernel/irq.c).
			 */
			pic_mask_vector(vector);
#if !defined(CONFIG_SMP)
			pic_deliver_interrupt(vector, cpus, all);
#else
			cpus = smp_get_affinity(vector, &all);
			if (!cpus) {
				/*
				 * No CPU to deliver to so just leave
				 * the interrupt unmasked and increase
				 * the retry count.  We will eventually
				 * return and deliver it again.
				 */
				pic_unmask_vector(vector);
				pic_bank.retry_nocpus++;
				continue;
			}
			pic_deliver_interrupt(vector, cpus, all);
#endif
		}
	}

	/* NOTREACHED */
}

#if defined(CONFIG_PROC_FS)
/*
 * picfs_show()
 *	Display PIC interrupt information.
 *
 * We do not try to be consistent here because that would require
 * the pic to grab locks which we don't want to have that overhead
 * thus these values are approximations.
 */
static int picfs_show(struct seq_file *m, void *v)
{
	int n = (int)v - 1;

	if (n == 0) {
		int i;
		seq_printf(m, "Attempts\t\t: %u\n", pic_bank.attempts);
		seq_printf(m, "Failures\t\t: %u\n", pic_bank.backout);

		seq_printf(m, "\n\tDeliveries\n");
		seq_printf(m, "Interrupts\t\t: %u\n", pic_bank.interrupts);
		seq_printf(m, "Traps\t\t\t: %u\n", pic_bank.traps);
		seq_printf(m, "Page faults\t\t: %u\n", pic_bank.faults);

		seq_printf(m, "\n\tNested Deliveries\n");
		for (i = 0; i < PIC_NESTED_COUNT; i++) {
			seq_printf(m, "Nested[%d]\t\t: %u\n", i, pic_bank.nested[i]);
		}

		seq_printf(m, "\n\tRetry Causes\n");
		seq_printf(m, "Affinity failures\t: %u\n",
			   pic_bank.retry_nocpus);
		seq_printf(m, "Global locks\t\t: %u\n",
			   pic_bank.retry_global);
		seq_printf(m, "Local interrupts\t: %u\n",
			   pic_bank.retry_local);
		seq_printf(m, "MMU or I/O Blocked\t: %u\n",
			   pic_bank.retry_mmuio);

		seq_printf(m, "\n");
	}

	/*
	 * Print out each word of the PIC enabled/mask set.
	 */
	seq_printf(m, "[%d]: enabled[%08x] - mask[%08x]\n",
		   n, pic_bank.enabled[n], pic_bank.mask[n]);
	return 0;
}

/*
 * pic_start()
 *	Start the picfs display.
 */
static void *picfs_start(struct seq_file *m, loff_t *pos)
{
	unsigned long i = *pos;
	return i < PIC_IRQ_WORDS ? (void *)(i + 1) : NULL;
}

/*
 * pic_next()
 *	Move to the next position.
 */
static void *picfs_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return picfs_start(m, pos);
}

/*
 * pic_stop()
 */
static void picfs_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations picfs_seq_ops = {
	.start = picfs_start,
	.next  = picfs_next,
	.stop  = picfs_stop,
	.show  = picfs_show,
};

/*
 * picfs_open()
 *	Create a sequence based /proc filesystem.
 */
static int picfs_open(struct inode *inode, struct file *filp)
{
	return seq_open(filp, &picfs_seq_ops);
}

static const struct file_operations picfs_fops = {
	.open		= picfs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

/*
 * picfs_init()
 *	Create the /proc/pic file system.
 */
static int __init picfs_init(void)
{
	if (proc_create("pic", 0, NULL, &picfs_fops) != 0) {
		return -ENOMEM;
	}
	return 0;
}
module_init(picfs_init);
#endif // CONFIG_PROC_FS

/*
 * pic_mask_vector()
 *	Temporarily mask the interrupt vector, turn off the bit in the mask
 *	register.
 */
void pic_mask_vector(unsigned int vector)
{
	unsigned int word = PIC_IRQ_WORD(vector);
	unsigned int mask = ~(1 << PIC_IRQ_BIT(vector));

	pic_lock_acquire();
	pic_bank.mask[word] &= mask;
	pic_lock_release();
	thread_resume(pic_tid);
}

/*
 * pic_unmask_vector()
 *	Unmask the interrupt vector so that it can be used, turn on the bit in
 *	the mask register.
 *
 * Because it is legal for the interrupt path to disable an interrupt,
 * the unmasking code must ensure that disabled interrupts are not
 * unmasked.
 */
void pic_unmask_vector(unsigned int vector)
{
	unsigned int word = PIC_IRQ_WORD(vector);
	unsigned int mask = (1 << PIC_IRQ_BIT(vector));

	pic_lock_acquire();
	pic_bank.mask[word] |= (mask & pic_bank.enabled[word]);
	pic_lock_release();
	thread_resume(pic_tid);
}

/*
 * pic_enable_vector()
 *	The PIC implements an interrupt controller and has a local (to the
 *	PIC) copy of its interrupt mask.
 */
void pic_enable_vector(unsigned int vector)
{
	unsigned int word = PIC_IRQ_WORD(vector);
	unsigned int mask = (1 << PIC_IRQ_BIT(vector));

	pic_lock_acquire();
	pic_bank.enabled[word] |= mask;
	pic_bank.mask[word] |= mask;
	pic_lock_release();
	thread_resume(pic_tid);
}

/*
 * pic_disable_vector()
 *	The PIC implements an interrupt controller and has a local (to the
 *	PIC) copy of its interrupt mask.
 */
void pic_disable_vector(unsigned int vector)
{
	unsigned int word = PIC_IRQ_WORD(vector);
	unsigned int mask = ~(1 << PIC_IRQ_BIT(vector));

	pic_lock_acquire();
	pic_bank.enabled[word] &= mask;
	pic_bank.mask[word] &= mask;
	pic_lock_release();
	thread_resume(pic_tid);
}

/*
 * pic_get_threadid()
 *	Return the threadid of the PIC thread.
 */
thread_t pic_get_threadid(void)
{
	return pic_tid;
}

/*
 * pic_set_trap_irq()
 *	Save away the trap Soft IRQ
 *
 * NOTE: This interrupt will also wake the PIC when it is waiting to
 * deliver interrupts.
 */
void pic_set_trap_irq(unsigned int irq)
{
	BUG_ON(irq > 31);

	pic_trap_irq = irq;
	pic_suspend_mask |= (1 << irq);
	pic_enable_vector(irq);
	pic_unmask_vector(irq);
}

/*
 * pic_set_mmu_irq()
 *	Save away the mmu Soft IRQ
 *
 * NOTE: This interrupt will also wake the PIC when it is waiting to
 * deliver interrupts.
 */
#if defined(CONFIG_MMU)
void pic_set_mmu_irq(unsigned int irq)
{
	BUG_ON(irq > 31);

	pic_mmu_irq = irq;
	pic_suspend_mask |= (1 << irq);
	pic_enable_vector(irq);
	pic_unmask_vector(irq);
}
#endif

/*
 * pic_init()
 *	Initialize the PIC (Interrupt Controller)
 */
void __init pic_init(void)
{
#if defined(CONFIG_IRQSTACKS)
	int i;
	union irq_ctx *icp;
#endif

	void *stack_high = (void *)pic_stack_space;
	memset(stack_high, 0xaa, sizeof(pic_stack_space));
	stack_high += sizeof(pic_stack_space);
	stack_high -= 8;

	/*
	 * Obtain a soft IRQ to use
	 */
	if (irq_soft_alloc(&pic_soft_irq) < 0) {
		panic("no software IRQ is available\n");
		return;
	}

	BUG_ON(pic_soft_irq > 31);
	pic_soft_irq_mask |= (1 << pic_soft_irq);
	pic_suspend_mask |= pic_soft_irq_mask;

	/*
	 * Now allocate and start the PIC thread.
	 */
	pic_tid = thread_alloc();
	if (pic_tid < 0) {
		panic("no thread available to run PIC");
		return;
	}

#if defined(CONFIG_IRQSTACKS)
	/*
	 * Initialize the per-cpu irq thread_info structure that
	 * is at the top of each per-cpu irq stack.
	 */
	icp = (union irq_ctx *)
		(((unsigned long)percpu_irq_stacks + (THREAD_SIZE - 1)) & ~(THREAD_SIZE - 1));
	for (i = 0; i < NR_CPUS; i++) {
		struct thread_info *ti = &(icp->tinfo);
		ti->task = NULL;
		ti->exec_domain = NULL;
		ti->cpu = i;
		ti->preempt_count = 0;
		ti->interrupt_nesting = 0;
		percpu_irq_ctxs[i] = icp++;
	}
#endif
	/*
	 * Register the pic performance counters with the Ubicom profiler.
	 */
	profile_register_performance_counter((volatile unsigned int *)&pic_bank.interrupts, "interrupts");
	profile_register_performance_counter((volatile unsigned int *)&pic_bank.traps, "traps");
	profile_register_performance_counter((volatile unsigned int *)&pic_bank.faults, "faults");

	/*
	 * Finally, let's start the pic running.
	 */
	thread_start(pic_tid, pic, NULL, stack_high, THREAD_TYPE_NORMAL);
}
