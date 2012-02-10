/*
 * arch/ubicom32/kernel/stacktrace.c
 *   Ubicom32 architecture stack back trace implementation.
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
#include <linux/sched.h>
#include <linux/stacktrace.h>
#include <linux/module.h>
#include <linux/kallsyms.h>

#include <asm/thread_info.h>
#include <asm/stacktrace.h>
#include <asm/thread.h>
#include <asm/ftrace.h>

#ifdef CONFIG_MMU
#  include <asm/fault.h>
#endif

// #define STACKTRACE_DEBUG 1
// #define STACKTRACE_SELFTEST
#define STACKTRACE_DEPTH_COUNT 256

#if defined(CONFIG_STACKTRACE)
#  if (!defined(CONFIG_FRAME_POINTER) || !defined(CONFIG_KALLSYMS))
#    warning "You will get best results turning on CONFIG_FRAME_POINTER and CONFIG_KALLSYMS"
#  endif
#endif

#if !defined(STACKTRACE_DEBUG)
#define STATIC static
#define debug_printk(args...)
#else
#define STATIC
#define debug_printk(args...) printk(KERN_EMERG args)
#endif

/*
 * stacktrace_decode_size_from_target()
 *	Given a call target, compute the frame size
 *
 * NOTE: Assume that the first instruction of the call
 * target is the one that adjusts the SP.
 */
STATIC int stacktrace_decode_size_from_target(unsigned long target)
{
	unsigned long instruction = *(unsigned int *)target;
	debug_printk("instruction[%08lx] = %08lx\n", target, instruction);

	/*
	 * Decode for movei d<reg>, #<imm>
	 */
	if ((instruction & 0xfff00000) == (uint32_t)0xc9000000) {
		uint16_t reg = (instruction & 0x000f0000) >> 16;
		int16_t imm = instruction & 0x0000ffff;
		debug_printk("movei d%d,#%d\n", reg, imm);

		target += 4;
		instruction = *(unsigned int *)target;

		/*
		 * Decode for add.4 sp, sp, d<reg>
		 */
		if (instruction == (0x79270127 | (reg << 11))) {
			debug_printk("add.4 sp,sp,d%d\n", reg);
			return -imm;
		}

		/*
		 * Decode for sub.4 sp, sp, d<reg>
		 */
		if (instruction == (0x91270127 | (reg << 11))) {
			debug_printk("sub.4 sp,sp,d%d\n", reg);
			return imm;
		}

		/* Fall through */
	}

	/*
	 * Decode for pdec sp, offset(sp)
	 */
	if ((instruction & 0xfffffce0) == (uint32_t)0x0127f4e0) {
		int16_t offset = ((instruction&0x0300) >> 3) | (instruction & 0x1f);
		offset |= 0xff80;
		offset *= -4;
		debug_printk("pdec sp,%d(sp)\n", offset);
		return offset;
	}

	/*
	 * Decode for move.4 with a destination offset(sp)++
	 */
	if ((instruction & 0xfff0f800) == (uint32_t)0x02f06000) {
		int16_t offset = (instruction & 0xf0000) >> 16;
		if (offset & 0x08) {
			offset |= 0xfff0;
		}
		offset *= 4;
		debug_printk("move.4 %d(sp)++,xx\n", offset);
		return -offset;
	}
	debug_printk("instruction[%08lx] = %08lx: unknown instruction\n", target, instruction);
	return 0;
}

/*
 * stacktrace_get_call_target()
 *	Returns the called address from the call insturction
 *
 * The call instruction encodes the callee as an offset from the current
 * PC.
 */
STATIC unsigned long stacktrace_get_call_target(unsigned long address, unsigned long instruction)
{
	int offset = ((instruction & 0x07000000) >> 3 ) | (instruction & 0x001fffff);

	/*
	 * Sign extend.
	 */
	if (offset & 0x800000) {
		offset = offset | 0xff000000;
	}
	return address + (offset << 2);
}

/*
 * stacktrace_adjust_frame()
 *	Adjust the frame to the previous frame
 *
 * NOTE: Our ability to do this depends a great deal on the space (user/kernel)
 * and the configuration (FRAME_POINTER) or not.
 */
STATIC void stacktrace_adjust_frame(struct stacktrace_frame *frame, unsigned long *sp, unsigned long pc, unsigned long target)
{
	unsigned long orig_sp = frame->sp;
	/*
	 * Based on the current compiler (5/4/2011), the frame pointers 
	 * are not a linked list, thus we can not use frame pointers to
	 * find the previous frame.  We default to taking a best guess
	 * by using the first instruction at the traget of the call as the
	 * one that adjusts SP.  We decode this instruction to find the
	 * size that SP was modified by.
	 */
	debug_printk("old frame[%08lx:%08lx]\n", frame->sp, frame->pc);
	if (target) {
		int offset;
		while ((offset = stacktrace_decode_size_from_target(target)) != 0) {
			frame->sp += offset;
			target += 4;
		}
		/* Fall through */
	}
	/*
	 * This routine MUST update the stack pointer or we will end up
	 * processing the same frame again. If we failed to decode the
	 * stack pointer offset then we use the sp argument as best guess.
	 */
	if (frame->sp == orig_sp) {
		frame->sp = (unsigned long)sp;
	}
	frame->pc = pc;
	debug_printk("new frame[%08lx:%08lx]\n", frame->sp, frame->pc);
}

/*
 * stacktrace_iterate()
 *	Walk the stack looking for call and calli instructions on an aligned
 *	boundary.
 *
 * frame.sp must point to the top of the current stack frame.
 * frame.pc must be the current pc or STACKTRACE_DEFAULT_PC.
 *
 * TODO: Consider the case of leaf functions where a5 is not
 * stored.
 */
unsigned long stacktrace_iterate(struct task_struct *tsk,
				 struct stacktrace_bounds *bounds,
				 struct stacktrace_frame *frame)
{
	unsigned int thread_trap_en, instruction;
	unsigned long address;
	unsigned long paddr;
	unsigned int limit = 0;
	unsigned long result = 0;
	unsigned long *sp = (unsigned long *)frame->sp;

	/* TODO: Check that preemption is disabled, do I need that or not? */

#if CONFIG_MMU
	/*
	 * In the MMU case, get us down to the pgd before we start
	 * the loop.
	 */
	pgd_t *pgd = init_mm.pgd;
	if ((tsk->mm) && (tsk->mm->pgd)) {
		pgd = tsk->mm->pgd;
	}
#endif

	/*
	 * Exclude the current thread from being monitored for traps.
	 */
	asm volatile (
	"	thread_get_self_mask d15		\n\t"
		/* save current trap status */
	"	and.4	%0, MT_TRAP_EN, d15		\n\t"
	"	not.4	d15, d15			\n\t"
		/* disable trap */
	"	and.4	MT_TRAP_EN, MT_TRAP_EN, d15	\n\t"
	"	pipe_flush 0				\n\t"
		: "=r" (thread_trap_en)
		:
		: "d15", "cc"
	);

	/*
	 * Only look back a small number of words based on the fact that the
	 * compiler stores the stack point directly before or after
	 * the calle save registers (d10-d14, a1, a2, a6) + return addres
	 * + 1 word for immediates = 10 words.
	 *
	 * Of course, without CONFIG_FRAME_POINTER we don't know the size of the
	 * previous stack frame and we stopped the iteration when the previous PC was
	 * found, so we need some amount of "guess work" plus the 10.
	 */
	while (limit++ < STACKTRACE_DEPTH_COUNT) {

		/*
		 * See if we have a valid stack.
		 */
		if (!between((unsigned long)sp, bounds->sstack, bounds->estack)) {
			debug_printk("[%d]: stack address is out of range - "
			       "sp: %x, sstack: %x, estack: %x\n",
				limit,
				(unsigned int)sp, (unsigned int)bounds->sstack,
				(unsigned int)bounds->estack);
			result = 0;
			break;
		}

		/*
		 * Get the value off the stack and back up 4 bytes to what
		 * should be the address of a call or calli.
		 */
		address = (*sp++) - 4;
		paddr = address;

		/*
		 * If the address is not on an aligned boundary it can not be a
		 * return address.
		 */
		if (paddr & 0x3) {
			debug_printk("[%d]: unaligned value: 0x%lx\n", limit, paddr);
			continue;
		}

#if CONFIG_MMU
		/*
		 * If this is a userspace process, we need to examine the physical
		 * address not the virtual address.
		 */
		if (bounds->type == STACKTRACE_KERNEL) {
			paddr = virt_to_phys(address);
		} else {
			paddr = translate_to_physical(pgd, address);
			if (!paddr) {
				debug_printk("[%d]: translate_to_physical 0x%lx -> error\n", limit, address);
				continue;
			}
		}
#endif
		/*
		 * If the address is not within the text segment, skip this
		 * value.
		 *
		 * TODO: Consider checking when VA if this is a code page.
		 */
		if (!between(address, bounds->stext, bounds->etext) &&
			!between(paddr, bounds->ocm_stext, bounds->ocm_etext)) {
				debug_printk("[%d]: not a text address - "
			       "address: %08x, stext: %08x, etext: %08x\n"
			       "ocm_stext: %08x, ocm_etext: %08x\n",
					limit,
					(unsigned int)address,
					(unsigned int)bounds->stext,
					(unsigned int)bounds->etext,
					(unsigned int)bounds->ocm_stext,
					(unsigned int)bounds->ocm_etext);
			continue;

		}

		/*
		 * Read the probable instruction
		 */
		instruction = *(unsigned int *)paddr;

		/*
		 * Print out the current frame information.
		 */
		debug_printk("[%d]: f[%08lx:%08lx] - [%08lx:%08lx]: %08lx\n",
				limit,
				frame->sp,
				frame->pc,
				(unsigned long)sp,
				(unsigned long)address,
				(unsigned long)instruction);

		/*
		 * Is this a call?
		 */
		if ((instruction & 0xF8000000) == (u32_t)(0x1B << 27)) {
			/*
			 * Check that the computed target is less than the previous pc
			 * since we know that our previous pc value is inside of the function.
			 * This is only true if you have compiled without sibling support.
			 */
			unsigned long target = stacktrace_get_call_target(address, instruction);
			if (target > frame->pc) {
				debug_printk("[%d]: address=%lx, instruction=%x\n", limit, address, instruction);
				debug_printk("[%d]: target [%lx] > PC [%lx]\n", limit, target, frame->pc);
				continue;
			}

			debug_printk("call inst. result: %x, instruction: %x\n",
				(unsigned int)address,
				(unsigned int)instruction);
#if CONFIG_MMU
			/*
			 * If this is a userspace process, we need to examine the physical
			 * address not the virtual address.
			 */
			if (bounds->type == STACKTRACE_KERNEL) {
				target = virt_to_phys(target);
			} else {
				target = translate_to_physical(pgd, target);
			}
#endif
			stacktrace_adjust_frame(frame, sp, address, target);
			result = address;
			break;
		}

		/*
		 * Read the probable instruction and check for calli which
		 * is not being used for a return.
		 */
		if ((instruction != 0xf0a000a0) && ((instruction & 0xF8000000) == (u32_t)(0x1E << 27))) {
			debug_printk("calli inst. result: %x, instruction: %x\n",
				(unsigned int)address,
				(unsigned int)instruction);
			stacktrace_adjust_frame(frame, sp, address, (unsigned long)NULL);
			result = address;
			break;
		}

		debug_printk("[%d]: not a call or calli - %lx, %x\n", limit, address, instruction);
	}

	/*
	 * Restore the current thread to be monitored for traps.
	 */
	if (thread_trap_en) {
		asm volatile(
		"	thread_get_self_mask d15		\n\t"
		"	or.4	MT_TRAP_EN, MT_TRAP_EN, d15	\n\t"
			:
			:
			: "d15", "cc"
		);
	}
	return result;
}

#ifdef CONFIG_STACKTRACE
/*
 * stacktrace_save_entries()
 *	Save stack back trace information into the provided trace structure.
 */
static noinline void stacktrace_save_entries(struct task_struct *tsk,
			     struct stack_trace *trace,
			     unsigned long sp)
{
	unsigned long ocm_code_start = (unsigned long)&__ocm_text_run_begin;
	unsigned long ocm_code_end = (unsigned long)&__data_begin;
	unsigned long stack_end = (unsigned long)(tsk->stack + THREAD_SIZE - 8);
	unsigned long code_start;
	unsigned long code_end;
	struct stacktrace_frame frame;
	struct stacktrace_bounds bounds;
	int skip = trace->skip;

	debug_printk("stacktrace_save_entries: skip=%d, max_entries=%d\n", trace->skip, trace->max_entries);

	/*
	 * Setup the bounds for the kernel.
	 */
	processor_dram(&code_start, &code_end);
	bounds.type = STACKTRACE_KERNEL;
	bounds.stext = code_start;
	bounds.etext = code_end;
	bounds.ocm_stext = ocm_code_start;
	bounds.ocm_etext = ocm_code_end;
	bounds.sstack = sp;
	bounds.estack = stack_end;

	/*
	 * Init the frame object that will move
	 * during the iteration.
	 */
	frame.sp = sp;
	frame.pc = STACKTRACE_DEFAULT_PC;
	trace->nr_entries = 0; 
	while (trace->nr_entries < trace->max_entries) {
		trace->entries[trace->nr_entries] = stacktrace_iterate(
					tsk, 
					&bounds,
					&frame);

		if (trace->entries[trace->nr_entries] == 0) {
			break;
		}

		if (skip) {
			skip--;
			continue;
		}
		trace->nr_entries++;
	}
}

/*
 * save_stack_trace()
 *	Save the specified amount of the kernel stack trace information
 *	for the current task.
 */
void save_stack_trace(struct stack_trace *trace)
{
	stacktrace_save_entries(current, trace, current_stack_pointer);
}
EXPORT_SYMBOL_GPL(save_stack_trace);

/*
 * save_stack_trace_tsk()
 *	Save the specified amount of the kernel stack trace information
 *	for the specified task.
 *
 * Note: We assume the specified task is not currently running.
 */
void save_stack_trace_tsk(struct task_struct *tsk, struct stack_trace *trace)
{
	stacktrace_save_entries(tsk, trace, tsk->thread.sp);
}
EXPORT_SYMBOL_GPL(save_stack_trace_tsk);

/*
 * return_address()
 *	Return the depth address in the call stack.
 */
unsigned long return_address(unsigned int depth)
{
	struct stack_trace trace;
	unsigned long entries[10];

	BUG_ON(depth >= ARRAY_SIZE(entries));

	/*
	 * Clear the data on our stack by clearing
	 * our 2 local variables; so that stale pointers
	 * on our stack are not found as return values
	 */
#if !defined(CONFIG_FRAME_POINTER)
	memset(&trace, 0, sizeof(trace));
	memset(entries, 0, ARRAY_SIZE(entries) * sizeof(unsigned long));
#endif

	/*
	 * Only get enough entries to respond to the request
	 */
	trace.max_entries = depth + 1;
	trace.entries = entries;
	trace.skip = 2;
	trace.nr_entries = 0;
	save_stack_trace(&trace);

	if (depth > trace.nr_entries) {
		return 0;
	}
	return trace.entries[depth];
}
EXPORT_SYMBOL_GPL(return_address);

#if defined(STACKTRACE_SELFTEST)
/*
 * stacktrace_selftest_c()
 *	Most inner function of the stacktrace selftest code.
 */
static void noinline stacktrace_selftest_c(void)
{
	unsigned long addr = return_address(0);
	printk(KERN_CRIT "[%pf]: called by: %pf\n",
		(void *)stacktrace_selftest_c,
		(void *)addr);

	addr = return_address(1);
	printk(KERN_CRIT "[%pf]: grandparent is: %pf\n",
		(void *)stacktrace_selftest_c,
		(void *)addr);

	addr = return_address(2);
	printk(KERN_CRIT "[%pf]: great grandparent is: %pf\n",
		(void *)stacktrace_selftest_c,
		(void *)addr);
}

/*
 * stacktrace_selftest_b()
 *	Nested function of the stacktrace selftest code.
 */
static void noinline stacktrace_selftest_b(void)
{
	unsigned long addr = return_address(0);

	printk(KERN_CRIT "[%pf]: called by: %pf\n",
		(void *)stacktrace_selftest_b,
		(void *)addr);

	addr = return_address(1);
	printk(KERN_CRIT "[%pf]: grandparent is: %pf\n",
		(void *)stacktrace_selftest_b,
		(void *)addr);

	stacktrace_selftest_c();
}

/*
 * stacktrace_selftest_a()
 *	Outter function of the stacktrace selftest code.
 */
static void noinline stacktrace_selftest_a(void)
{
	unsigned long addr = return_address(0);
	printk(KERN_CRIT "[%pf]: called by: %pf\n",
		(void *)stacktrace_selftest_a, 
		(void *)addr);
	stacktrace_selftest_b();
}

/*
 * stacktrace_selftest_init()
 *	Run a quick test of the backtrace code
 */
static int stacktrace_selftest_init(void)
{
	printk(KERN_CRIT "[%pf]: selfttest for return_address() function\n",
		(void *)stacktrace_selftest_init);
	stacktrace_selftest_a();
	return 0;
}
arch_initcall(stacktrace_selftest_init);
#endif /* STACKTRACE_SELFTEST */
#endif /* CONFIG_STACKTRACE */
