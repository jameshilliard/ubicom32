/*
 * arch/ubicom32/kernel/traps.c
 *   Ubicom32 architecture trap handling support.
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

/*
 * Sets up all exception vectors
 */
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/a.out.h>
#include <linux/user.h>
#include <linux/string.h>
#include <linux/linkage.h>
#include <linux/init.h>
#include <linux/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/compiler.h>
#include <linux/stacktrace.h>
#include <linux/personality.h>

#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/stacktrace.h>
#include <asm/devtree.h>
#include <asm/setup.h>
#include <asm/fpu.h>
#include <asm/system.h>
#include <asm/traps.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/machdep.h>
#include <asm/siginfo.h>
#include <asm/thread.h>
#include <asm/thread_info.h>

#define TRAP_MAX_STACK_DEPTH 20

#if (UBICOM32_ARCH_VERSION < 5)
#define MIN_USER (KERNELSTART)
#define MAX_USER (memory_end)
#else
#define MIN_USER (UBICOM32_FDPIC_TEXT_START)
#define MAX_USER (UBICOM32_FDPIC_STACK_END)
#endif

/*
 * These symbols are filled in by the linker.
 */
extern unsigned long _stext;
extern unsigned long _etext;

extern unsigned long __ocm_text_run_begin;
extern unsigned long __data_begin;

extern void show_vmas(struct task_struct *task);

const char *trap_cause_strings[] = {
	/*0*/	"inst address decode error",
	/*1*/	"inst sync error",
	/*2*/	"inst illegal",
	/*3*/	"src1 address decode error",
	/*4*/	"dst address decode error",
	/*5*/	"src1 alignment error",
	/*6*/	"dst alignment error",
	/*7*/	"src1 sync error",
	/*8*/	"dst sync error",
	/*9*/	"DCAPT error",
	/*10*/	"inst range error",
	/*11*/	"src1 range error",
	/*12*/	"dst range error",
	/*13*/	"privilege violation",
};

/*
 * The device tree trap node definition.
 */
struct trapnode {
	struct devtree_node dn;
	unsigned int intthread;
};

static struct trapnode *tn;;

#if (UBICOM32_ARCH_VERSION < 5)
/*
 * trap_cause_to_str()
 *	Convert a trap_cause into a series of printk
 */
static void trap_cause_to_str(long status)
{
	int bit;

	if ((status & ((1 << TRAP_CAUSE_TOTAL) - 1)) == 0) {
		printk(KERN_NOTICE "decode: UNKNOWN CAUSES\n");
		return;
	}

	for (bit = 0; bit < TRAP_CAUSE_TOTAL; bit++) {
		if (status & (1 << bit)) {
			printk(KERN_NOTICE "\tdecode: %08x %s\n",
			       1 << bit, trap_cause_strings[bit]);
		}
	}
}
#else
/*
 * trap_cause_to_str()
 *	Convert a trap_cause into a series of printk
 */
static void trap_cause_to_str(long status, long csr)
{
	int bit;

	if (((status & ((1 << TRAP_CAUSE_TOTAL) - 1)) == 0) && (csr == 0)) {
		printk(KERN_NOTICE "decode: UNKNOWN CAUSES\n");
		return;
	}

	if (status & ((1 << TRAP_CAUSE_TOTAL) - 1)) {
		/*
		 * Interpret bits in the trap_cause register.
		 */
		for (bit = 0; bit < TRAP_CAUSE_TOTAL; bit++) {
			if (status & (1 << bit)) {
				printk(KERN_NOTICE "\tdecode: %08x %s\n",
				       1 << bit, trap_cause_strings[bit]);
			}
		}
	}

	if (csr) {
		/*
		 * Floating point traps are set. Print them.
		 */
		if (csr & (1 << UBICOM32_CSR_FPTRAP_INEXACT_BIT)) {
			printk(KERN_NOTICE "\tFPU INEXACT trap\n");
		}

		if (csr & (1 << UBICOM32_CSR_FPTRAP_UNDERFLOW_BIT)) {
			printk(KERN_NOTICE "\tFPU UNDERFLOW trap\n");
		}

		if (csr & (1 << UBICOM32_CSR_FPTRAP_OVERFLOW_BIT)) {
			printk(KERN_NOTICE "\tFPU OVERFLOW trap\n");
		}

		if (csr & (1 << UBICOM32_CSR_FPTRAP_DIV_BY_0_BIT)) {
			printk(KERN_NOTICE "\tFPU DIVIDE by 0 trap\n");
		}

		if (csr & (1 << UBICOM32_CSR_FPTRAP_INVALID_BIT)) {
			printk(KERN_NOTICE "\tFPU INVALID trap\n");
		}
	}
}
#endif

/*
 * trap_print_information()
 *	Print the cause of the trap and additional info.
 */
static void trap_print_information(const char *str, struct pt_regs *regs)
{
	printk(KERN_WARNING "\n");

	if (current) {
		printk(KERN_WARNING "Process %s (pid: %d)\n",
			current->comm, current->pid);
	}

	if (current && current->mm) {
		printk(KERN_NOTICE
			"text = 0x%p-0x%p  data = 0x%p-0x%p\n"
			"bss = 0x%p-0x%p   user-stack = 0x%p\n"
			"\n",
			(void *)current->mm->start_code,
			(void *)current->mm->end_code,
			(void *)current->mm->start_data,
			(void *)current->mm->end_data,
			(void *)current->mm->end_data,
			(void *)current->mm->brk,
			(void *)current->mm->start_stack);
	}

#if (UBICOM32_ARCH_VERSION < 5)
	printk(KERN_WARNING "%s: Causes: 0x%08x\n", str,
			(unsigned int)regs->trap_cause);
	trap_cause_to_str(regs->trap_cause);
#else
	printk(KERN_WARNING "%s: Causes: 0x%08x Float traps 0x%08x\n", str,
	       (unsigned int)regs->trap_cause, (unsigned int) (regs->csr & UBICOM32_CSR_FPTRAP_MASK));
	trap_cause_to_str(regs->trap_cause, (regs->csr & UBICOM32_CSR_FPTRAP_MASK));
#endif
	show_regs(regs);
	show_vmas(current);
	show_stack(NULL, (unsigned long *)regs->an[7]);
	printk(KERN_NOTICE "--- End Trap --- \n");

#if defined(CONFIG_UBICOM32_STALL_ON_TRAP_MARKER)
	/*
	 * If requested, we stall after printing the trap
	 * marker to allow for more detailed debugging.
	 */
	printk(KERN_NOTICE "Stalling all threads...\n");
	THREAD_STALL;
#endif
}

/*
 * dump_stack()
 *	Dump the stack of the current task.
 */
void dump_stack(void)
{
	show_stack(NULL, NULL);
}
EXPORT_SYMBOL(dump_stack);

/*
 * show_stack_calls_dump()
 *	Dump the call/calli list.
 */
static void show_stack_calls_dump(int idx, unsigned long *calls)
{
	int i;
	for (i = 0; i < idx; i++) {
		printk("%s0x%p, ", (i & 0x3) == 0 ?  "\n    " : "", (void *)calls[i]);
	}
	printk("%s%s\n\n", (i & 0x3) == 0 ?  "\n    " : "",
	       idx == TRAP_MAX_STACK_DEPTH ? "..." : "");
}

/*
 * show_stack()
 *	Print out information from the current stack.
 *
 * Note: The input parameters can vary a great deal.
 * The task can specified or current is implied.
 * The sp can be NULL, user sp, or kernel sp.
 */
void show_stack(struct task_struct *task, unsigned long *sp)
{
	unsigned long stack = (unsigned long)sp;
	int pass;

	/*
	 * Which task are we tracing.  If not current, we 
	 * assume the task is not running.
	 */
	if (!task) {
		task = current;
	}
	printk(KERN_NOTICE "Starting backtrace: PID %d '%s'\n",
			task->pid, task->comm);

	/*
	 * If sp is NULL, compute the starting stack value.  We assume that we
	 * want a kernel stack at this point.
	 */
	if (!stack) {
		if (task != current) {
			stack = task->thread.sp;
		} else {
			stack = current_stack_pointer;
		}
	}

	/*
	 * If the task is in kernel mode, we will try and do both kernel and user (2 passes).  
	 * If the task is in user mode, we will only do 1 pass and print only user data.
	 */
	pass = (__user_mode(stack)) ? 1 : 2;
	while (pass) {
		unsigned long calls[TRAP_MAX_STACK_DEPTH];
		struct pt_regs *pt = task_pt_regs(task);
		struct stacktrace_bounds bounds;
		struct stacktrace_frame frame;
		unsigned int i, idx = 0;

		/*
		 * Set up the bounds for kernel or user.
		 */
		if (!__user_mode(stack)) {
			unsigned long ram_start, ram_end;
			processor_dram(&ram_start, &ram_end);
			bounds.type = STACKTRACE_KERNEL;
			bounds.stext = ram_start;
			bounds.etext = ram_end;
			bounds.ocm_stext = (unsigned long)&__ocm_text_run_begin;
			bounds.ocm_etext = (unsigned long)&__data_begin;
			bounds.sstack = stack;
			bounds.estack = (unsigned long)task->stack + THREAD_SIZE - 8;

			printk(KERN_NOTICE "\tKernel Stack: 0x%lx\n", stack);
			printk(KERN_NOTICE "\t\tocm_code[0x%p-0x%p]\n",
					(void *)bounds.ocm_stext, (void *)bounds.ocm_etext);

		} else {
			/*
			 * Does this task have an mm (i.e. is there a user space?)
			 */
			if (!task->mm) {
				printk(KERN_NOTICE "No mm for userspace stack.\n");
				break;
			}
			bounds.type = STACKTRACE_USER;
			bounds.stext = task->mm->start_code;
			bounds.etext = task->mm->end_code;
			bounds.ocm_stext = 0;
			bounds.ocm_etext = 0;
			bounds.sstack = stack;
			bounds.estack = task->mm->start_stack;

			/*
			 * New out-dented header for user space.
			 */
			if (!(task->personality & FDPIC_FUNCPTRS)) {
				printk(KERN_NOTICE "\tUser Stack: 0x%lx\n", stack);
			} else {
				printk(KERN_NOTICE "\tUser Stack (fdpic): 0x%lx\n", stack);
			}
		}
		printk(KERN_NOTICE "\t\tcode[0x%p-0x%p]\n", (void *)bounds.stext, (void *)bounds.etext);
		printk(KERN_NOTICE "\t\tstack[0x%p-0x%p]\n\n", (void *)stack, (void *)bounds.estack);

		/*
		 * Get the frames.
		 */
		frame.sp = stack;
		frame.pc = STACKTRACE_DEFAULT_PC;
		while (idx < TRAP_MAX_STACK_DEPTH) {
			calls[idx] = stacktrace_iterate(current, &bounds, &frame);
			if (calls[idx] == 0) {
				break;
			}
			idx++;
		}

		/*
		 * Now print out the data.  Again, we have slightly different
		 * approaches for kernel/user.  In the kernel case, we print
		 * them one line at a time with the function names if possible.
		 */
		printk(KERN_NOTICE "\tCALL && CALLI on stack:");
		if (bounds.type == STACKTRACE_KERNEL) {
#if !defined(CONFIG_KALLSYMS)
			show_stack_calls_dump(idx, calls);
#else
			for (i = 0; i < idx; i++) {
				printk(KERN_CRIT "\t\t[<%p>] %pS\n", (void *)calls[i], (void *)calls[i]);
			}
#endif
		} else {
			show_stack_calls_dump(idx, calls);
		}

		/*
		 * If we are going around again it is for the user stack and we
		 * have already printed the kernel information.  In this case, the caller
		 * either specified a kernel stack or none at all.  Now we need
		 * to find the most valid user stack we can.  This would be the stack
		 * at entry to the kernel.
		 */
		stack = pt->an[7];

		/*
		 * Adjust exit condition.
		 */
		pass--;
	}
}

/*
 * die()
 *	Ubicom specific function to force death.
 *	
 * This function does not care if the regs are user or kernel mode.
 */
void die(char *str, struct pt_regs *regs, long trap_cause)
{
	unsigned int s3value;

	oops_enter();
	bust_spinlocks(1);

	/*
	 * Try to make sure we can print to the console.
	 */
	console_verbose();

	/*
	 * Detect recursive die().  We don't trust anything
	 * so just THREAD_STALL;
	 */
	if (test_and_set_thread_flag(TIF_RECURSIVE_DIE)) {
		THREAD_STALL;
	}

	/*
	 * First time through, let's try and print trap information.
	 */
	trap_print_information(str, regs);

	/*
	 * If the debugger is attached via the hardware mailbox protocol,
	 * go into an infinite loop and the debugger will figure things out.
	 */
	asm volatile (
	"	move.4 %0, scratchpad3	\n\t"
		: "=r" (s3value)
	);
	if (s3value) {
		asm volatile(
		"	1:	jmpt.t 1b	\n\t"
		);
	}

	/*
	 * Set the debug taint value.
	 */
	add_taint(TAINT_DIE);
	bust_spinlocks(0);
	oops_exit();
}

/*
 * die_if_kernel()
 *	Determine if we are in kernel mode and if so print stuff out and die.
 */
void die_if_kernel(char *str, struct pt_regs *regs, long trap_cause)
{
	if (user_mode(regs)) {
		return;
	}
	die(str, regs, trap_cause);
	do_exit(SIGSEGV);
}

/*
 * do_trap()
 *	Handle traps.
 *
 * Traps are treated as interrupts and registered with the PIC.  When
 * the PIC takes the interrupt, it will determine if a trap has occurred
 * and service the trap prior to servicing the interrupt.
 *
 * This function is force executed when the PIC switches a thread to it.
 */
void do_trap(struct pt_regs *regs)
{
	int sig = SIGSEGV;
	siginfo_t info;
	unsigned int trap_cause = regs->trap_cause;
#if defined(CONFIG_FPU)
	unsigned int fpu_traps = regs->csr & UBICOM32_CSR_FPTRAP_MASK;
#endif

	BUG_ON(!irqs_disabled());
	raw_local_irq_enable();

#if !defined(CONFIG_UNALIGNED_ACCESS_DISABLED)
	if (unaligned_only(regs)) {
		/*
		 * This is an unaligned trap only. Call unaligned_emulate to get it serviced
		 * if possible.
		 */
		int ret = unaligned_emulate(regs);
		if (ret == 0) {
			raw_local_irq_disable();
			return;
		}
	}
#endif


	/*
	 * In kernel, check for exception
	 */
	if (unlikely(!user_mode(regs))) {
		if ((trap_cause & (1 << TRAP_CAUSE_DST_RANGE_ERR)) ||
		    (trap_cause & (1 << TRAP_CAUSE_SRC1_RANGE_ERR)) ||
		    (trap_cause & (1 << TRAP_CAUSE_I_RANGE_ERR))) {

			const struct exception_table_entry *fix = search_exception_tables(regs->pc);
			if (fix) {
				regs->pc = fix->fixup;
				raw_local_irq_disable();
				return;
			}
		}

		die("Kernel Trap", regs, trap_cause);
	}

	/*
	 * User process problem, setup a signal for this process
	 */
	if ((trap_cause & (1 << TRAP_CAUSE_DST_RANGE_ERR)) ||
	    (trap_cause & (1 << TRAP_CAUSE_SRC1_RANGE_ERR)) ||
	    (trap_cause & (1 << TRAP_CAUSE_I_RANGE_ERR))) {
		sig = SIGSEGV;
		info.si_code = SEGV_MAPERR;
	} else if ((trap_cause & (1 << TRAP_CAUSE_DST_MISALIGNED)) ||
		   (trap_cause & (1 << TRAP_CAUSE_SRC1_MISALIGNED))) {
		sig = SIGBUS;
		info.si_code = BUS_ADRALN;
	} else if ((trap_cause & (1 << TRAP_CAUSE_DST_DECODE_ERR)) ||
		   (trap_cause & (1 << TRAP_CAUSE_SRC1_DECODE_ERR))) {
		sig = SIGILL;
		info.si_code = ILL_ILLOPN;
	} else if ((trap_cause & (1 << TRAP_CAUSE_ILLEGAL_INST))) {
		/*
		 * Check for software break point and if found signal
		 * breakpoint, not illegal instruction.  Since GDB needs
		 * the kernel to become active at the point a break
		 * point the IP5K/IP7K cannot use a breakpoint (which
		 * would stop all hardware threads).  Instead, GDB uses
		 * an illegal instruction.
		 *
		 * For the IP5K/IP7K, we take care not to cause another
		 * trap by ensuring that the address range makes sense.
		 *
		 * Historically, the code supports multiple "software"
		 * break point values:
		 *
		 * 0xaabbccdd - The original illegal instruction.
		 * 0xfabbccdd - Because the previous value is legal in v4.
		 */
		unsigned long instruction;
		if (between(regs->pc, MIN_USER, MAX_USER) &&
			(regs->pc & 3) == 0 &&
			get_user(instruction, (unsigned long *)regs->pc) == 0) {

			if ((instruction == 0xfabbccdd) ||
			    (instruction == 0xaabbccdd)) {
				sig = SIGTRAP;
				info.si_code = TRAP_BRKPT;
				goto send_signal;
			}
		}
		sig = SIGILL;
		info.si_code = ILL_ILLOPC;
	} else if ((trap_cause & (1 << TRAP_CAUSE_I_DECODE_ERR))) {
		sig = SIGILL;
		info.si_code = ILL_ILLOPC;
	} else if ((trap_cause & (1 << TRAP_CAUSE_DCAPT))) {
		sig = SIGTRAP;
		info.si_code = TRAP_TRACE;
	}

#if defined(CONFIG_FPU)
	else if (fpu_traps) {
		/*
		 * Floating point trap.
		 */
		sig = SIGFPE;

		/*
		 * Refine the error to exactly what is going on.
		 */
		if (fpu_traps & (1 << UBICOM32_CSR_FPTRAP_INEXACT_BIT)) {
			info.si_code = FPE_FLTRES;
		} else if (fpu_traps & (1 << UBICOM32_CSR_FPTRAP_UNDERFLOW_BIT)) {
			info.si_code = FPE_FLTUND;
		} else if (fpu_traps & (1 << UBICOM32_CSR_FPTRAP_OVERFLOW_BIT)) {
			info.si_code = FPE_FLTOVF;
		} else if (fpu_traps & (1 << UBICOM32_CSR_FPTRAP_DIV_BY_0_BIT)) {
			info.si_code = FPE_FLTDIV;
		} else {
			info.si_code = FPE_FLTINV;
		}
	}
#endif

	/*
	 * Print a trap information block to the console, do not
	 * print this above the case because we don't want it
	 * printed for software break points.
	 */
	trap_print_information("User Trap", regs);

#if defined(CONFIG_FPU)
	/*
	 * Clear the FPU traps from the csr register.
	 */
	if (sig == SIGFPE) {
		regs->csr &= ~UBICOM32_CSR_FPTRAP_MASK;
	}
#endif

send_signal:

	force_sig_info(sig, &info, current);
	raw_local_irq_disable();
}

/*
 * trap_init()
 *	init trap handling
 *
 * Trap handling is done through the PIC.  Every time an interrupt
 * occurs, the PIC looks for threads that are listed in the TRAP
 * register and forces a call to the trap handler.
 */
void __init trap_init(void)
{
	int err;
	unsigned char tirq;
	struct devtree_node *dn;

	/*
	 * Now setup the Software IRQ so that if a trap occurs the PIC
	 * is started.  The irq is there just to "force" the PIC to run.
	 */
	tn = (struct trapnode *)devtree_find_node("traps");
	if (!tn) {
		printk(KERN_WARNING "trap_init_interrupt skipped.\n");
		return;
	}

	dn = (struct devtree_node *)tn;
	err = devtree_irq(dn, NULL, &tirq);
	if (err) {
		printk(KERN_WARNING "error obtaining trap irq value: %d\n",
			err);
		return;
	}

	if (tirq == DEVTREE_IRQ_NONE) {
		printk(KERN_WARNING "trap irq not available: %d\n", tirq);
		return;
	}

	/*
	 * Let ultra know which thread is handling the traps and
	 * what the interrupt to use is.
	 */
	tn->intthread = pic_get_threadid();

	/*
	 * Tell the PIC about our IRQ so that it will unsuspend
	 * if one occurs while waiting for the per thread lock.
	 */
	pic_set_trap_irq(tirq);

	/*
	 * If we do not have a trap node in the device tree, we leave the fault
	 * handling to the underlying hardware.
	 */
	tn = (struct trapnode *)devtree_find_node("traps");
	if (!tn) {
		printk(KERN_WARNING "traps are not handled by linux\n");
		return;
	}
}
