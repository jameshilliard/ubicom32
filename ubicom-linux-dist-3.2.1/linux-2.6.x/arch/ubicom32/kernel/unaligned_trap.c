/*
 * arch/ubicom32/kernel/unaligned_trap.c
 *   Handle unaligned traps in both user or kernel space.
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

#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/cacheflush.h>
#include <asm/traps.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>

#define FALSE 0
#define TRUE 1

// #define DEBUG_UNALIGNED 1
#if defined(DEBUG_UNALIGNED)
#define DEBUG_ASSERT(cond) BUG_ON(!(cond));
#define STATIC
#else
#define DEBUG_ASSERT(cond)
#define STATIC static
#endif

/*
 * Create a macro for trace level debugging.  Critical messages
 * directly use printk since we don't turn those off.
 */
#if !defined(DEBUG_UNALIGNED)
#define unaligned_printk(...)
#else

#define UNALIGNED_DBG_ERROR 10
#define UNALIGNED_DBG_TRACE 20
STATIC unsigned int unaligned_dbg_lvl = 20;
#define unaligned_printk(lvl, printkargs...)	\
	if ((lvl) <= unaligned_dbg_lvl) {	\
		printk(KERN_INFO printkargs);	\
	}
#endif

/* no possible trap */
#define UNUSED 0
/* possible source operand trap */
#define SRC 1
#define SRC_2 2
/* possible destination operand trap */
#define DEST 3
#define DEST_2 4
/* can be either source or destination or both */
#define TWO_OP 5
#define TWO_OP_2 6

/* FPU operations */
#define OP_FPU 7

/* TODO: What is the real value here, put something in to make it compile for
 * now */
#define MOVE_2	0x0d
#define LSL_2	0x11
#define LSR_2	0x13
#define MOVEI	0x19
#define CMPI	0x18

static int op_format[32] =
{
	TWO_OP,		/* 0x00 */
	UNUSED,
	SRC,
	OP_FPU,
	TWO_OP,		/* 0x04 */
	TWO_OP,
	SRC,
	UNUSED,
	TWO_OP_2,	/* 0x08 */
	TWO_OP,
	TWO_OP_2,
	TWO_OP,
	TWO_OP_2,	/* 0x0C */
	TWO_OP,
	TWO_OP_2,
	TWO_OP,
	TWO_OP,		/* 0x10 */
	TWO_OP_2,
	TWO_OP,
	TWO_OP,
	UNUSED,		/* 0x14 */
	UNUSED,
	UNUSED,
	UNUSED,
	SRC_2,		/* 0x18 */
	DEST_2,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x1C */
	UNUSED,
	UNUSED,		/* unaligned CALLI will not be fixed. */
	UNUSED
};

static int op_0_format[32] =
{
	UNUSED,		/* 0x00 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x04 - ret don't fix - bad ret is always wrong */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x08 */
	UNUSED,
	TWO_OP,
	TWO_OP_2,
	TWO_OP,		/* 0x0c */
	TWO_OP_2,
	TWO_OP,
	UNUSED,		/* .1 can't trap */
	UNUSED,		/* 0x10 */
	UNUSED,
	SRC,
	UNUSED,
	UNUSED,		/* 0x14 */
	TWO_OP_2,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x18 */
	UNUSED,
	UNUSED,
	UNUSED,
	DEST,		/* 0x1c */
	DEST,
	DEST,
	DEST,		/* all lea have 32-bit destination */
};

static int op_2_format[32] =
{
	UNUSED,		/* 0x00 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x04 */
	UNUSED,
	SRC,
	UNUSED,
	UNUSED,		/* 0x08 crcgen is .1 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x0c */
	UNUSED,
	UNUSED,
	UNUSED,
	SRC,		/* 0x10 */
	SRC_2,
	SRC,
	SRC_2,
	SRC,		/* 0x14 */
	SRC_2,
	SRC,
	UNUSED,
	UNUSED,		/* 0x18 */
	UNUSED,
	SRC,
	UNUSED,
	SRC,		/* 0x1c */
	UNUSED,
	SRC_2,
	UNUSED,
};

static int op_3_format[32] =
{
	SRC,		/* 0x00 */
	SRC,
	SRC,
	SRC,
	SRC,		/* 0x04 */
	SRC,
	SRC,
	SRC,
	SRC,		/* 0x08 crcgen is .1 */
	SRC,
	SRC,
	SRC,
	SRC,		/* 0x0c */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x10 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x14 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x18 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x1c */
	UNUSED,
	UNUSED,
	UNUSED,
};

static int op_6_format[32] =
{
	SRC_2,		/* 0x00 */
	SRC_2,
	SRC_2,
	SRC_2,
	SRC_2,		/* 0x04 */
	SRC_2,
	UNUSED,
	SRC_2,
	SRC,		/* 0x08 MULS.4 */
	SRC_2,
	SRC,
	UNUSED,
	UNUSED,		/* 0x0c */
	UNUSED,
	UNUSED,
	UNUSED,
	SRC,		/* 0x10 */
	SRC_2,
	SRC,
	SRC_2,
	UNUSED,		/* 0x14 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x18 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x1c */
	UNUSED,
	UNUSED,
	UNUSED,
};

/*
 * unaligned_get_address()
 *	get an address using save_an and save_dn registers, and updates save_an
 *	with side effects
 */
STATIC unsigned char *unaligned_get_address(int specifier, int four_byte,
				     unsigned int *save_an,
				     unsigned int *save_dn, int *write_back_an)
{
	unsigned char *address;

	int areg = (specifier >> 5) & 7;
	if ((specifier >> 8) == 2) {
		int offset = specifier & 0xf;
		offset = ((offset << 28) >> 28);
		if (likely(four_byte)) {
			offset <<= 2;
		} else {
			offset <<= 1;
		}
		if (specifier & 0x10) {
			address = (unsigned char *)(save_an[areg] + offset);
		} else {
			address = (unsigned char *)save_an[areg];
		}
		save_an[areg] = save_an[areg] + offset;

		/*
		 * Let caller know An registers have been modified.
		 */
		*write_back_an = 1;
	} else if ((specifier >> 8) == 3) {
		int dreg = specifier & 0xf;
		if (likely(four_byte)) {
			address = (unsigned char *)(save_an[areg] +
						    (save_dn[dreg] << 2));
		} else {
			address = (unsigned char *)(save_an[areg] +
						    (save_dn[dreg] << 1));
		}
	} else {
		int offset = ((specifier >> 3) & 0x60) | (specifier & 0x1f);
		if (likely(four_byte)) {
			address = (unsigned char *)(save_an[areg] +
						    (offset << 2));
		} else {
			address = (unsigned char *)(save_an[areg] +
						    (offset << 1));
		}
	}
	unaligned_printk(UNALIGNED_DBG_TRACE, "spec: %x, four_byte: %x, write_back: %x, address: %p\n",
	       specifier, four_byte, *write_back_an, address);
	return address;
}

/*
 * unaligned_emulate()
 *	emulate the instruction at thread's pc that has taken an unaligned data
 *	trap.
 *
 * source or destination or both might be unaligned
 * the instruction must have a memory source or destination or both
 * the emulated instruction is copied and executed in this thread
 *
 *	TODO: Protection is handled outside of this function
 *	TODO: handling simultaneous unaligned and memory protection traps
 *
 *	Get thread state
 *		the PC and instruction (and local copy, emulate_inst), and An
 *		and Dn registers
 *		All implicit soruce state (source3, CSR, accumulators)

 *	if the instruction has a memory source
 *		Use the instruction, An and Dn registers to form rc_address
 *		get unaligned source data from src_address (usually sign
 *		extended)
 *			(2 bytes, with or without sign extension, or 4 bytes)
 *		modify emulate_inst to use d0 as source
 *	else
 *		get the soure operand from one of thread's registers
 *	if instruction has a memory destination
 *		Use the instruction, An and Dn registers to form dest_address
 *		modify emulate_inst to use d0 as destination
 *	if there was a memory source
 *		put the source data in thread's d0
 *	get the source-2 Dn operand and source 3 operand from thread
 *	execute modified inst
 *		(save it, flush caches, set up local values for implicit
 *		sources, execute, save explicit and implicit results)
 *	if inst has destination address
 *		copy result to dest_address, possibly unaligned, 1, 2, or 4
 *		bytes
 *	restore thread's implicit results (modified address registers, CSR,
 *	accumulators) add 4 to thread's pc
 */
int unaligned_emulate(struct pt_regs *regs)
{
	unsigned int inst;
	unsigned int op;
	unsigned int subop;
	int format;
	unsigned int emulate_inst;
	int four_byte;
	int src_operand, dest_operand;
	int save_csr;
	int source3;
	unsigned int source1;
	unsigned int source_data;
	unsigned char *dest_address = NULL;
	int source2 = 0;
	unsigned int result;
	unsigned int write_back_an = 0;
	unsigned int *save_dn = (unsigned int *)regs->dn;
	unsigned int *save_an = (unsigned int *)regs->an;
	unsigned int *save_acc = (unsigned int *)regs->acc0;

	extern unsigned int trap_emulate;
	extern unsigned int ubicom32_emulate_insn(int source1, int source2,
						  int source3, int *save_acc,
						  int *save_csr);
	/*
	 * get the pc
	 */
	unaligned_printk(UNALIGNED_DBG_TRACE, "unaligned_fault: pc=%lx\n", regs->pc);
	if (copy_from_user(&inst, (void *)(regs->pc), sizeof(inst))) {
		/*
		 * The PC is bogus. Change the fault to TRAP_CAUSE_I_RANGE_ERR.
		 */
		regs->trap_cause = (1 << TRAP_CAUSE_I_RANGE_ERR);
		unaligned_printk(UNALIGNED_DBG_ERROR, "invalid pc: %lx\n", regs->pc);
		return -1;
	}

	op = inst >> 27;
	if (unlikely(op == 2 || op == 3 || op == 6)) {
		subop = (inst >> 21) & 0x1f;
	} else {
		subop = (inst >> 11) & 0x1f;
	}
	format = op_format[op];
	emulate_inst = inst;

	if (op == 0) {
		format = op_0_format[subop];
	} else if (op == 2) {
		format = op_2_format[subop];
	} else if (op == 3) {
		format = op_3_format[subop];
	} else if (op == 6) {
		format = op_6_format[subop];
	}

	if (unlikely(format == UNUSED)) {
		/*
		 * We are not going to emulate this. Bump PC by 4 and move on.
		 */
		regs->pc += 4;
		return 0;
	}

	four_byte = (format == TWO_OP || format == DEST || format == SRC);

	/*
	 * source or destination memory operand needs emulation
	 */
	src_operand = (format == SRC ||
		       format == SRC_2 ||
		       format == TWO_OP ||
		       format == TWO_OP_2) &&
		((inst >> 8) & 7) > 1;

	dest_operand = (format == DEST ||
			format == DEST_2 ||
			format == TWO_OP ||
			format == TWO_OP_2) &&
		((inst >> 24) & 7) > 1;
	source3 = regs->source3;
	save_csr = regs->csr;

	/*
	 * turn off thread select bits if they were on
	 */
	DEBUG_ASSERT((save_csr & 0x04100) == 0);
	if (unlikely(save_csr & 0x04100)) {
		/*
		 * Things are in funny state as thread select bits are on in
		 * Trigger a seg violation and move on.
		 */
		unaligned_printk(UNALIGNED_DBG_ERROR, "CSR has alternate thread select bits on.\n");
		return -1;
	}

#if (UBICOM32_ARCH_VERSION >= 5)
	save_csr = regs->ucsr;
#else
	save_csr = save_csr & 0x1000ff;
#endif

	/*
	 * get the source1 operand
	 */
	source1 = 0;
	if (src_operand) {
		unsigned char *src_address;

		/*
		 * source1 comes from memory
		 */
		DEBUG_ASSERT(format == TWO_OP || format == TWO_OP_2 ||
			 format == SRC || format == SRC_2);
		src_address = unaligned_get_address(inst & 0x7ff,
						    four_byte, save_an,
						    save_dn, &write_back_an);
		unaligned_printk(UNALIGNED_DBG_TRACE, "decode src_address: %p\n", (void *)src_address);

#if (UBICOM32_ARCH_VERSION >= 5)
		/*
		 * For 8k and future processors check for pivilege violations.
		 */
		if (user_mode(regs)) {
			/*
			 * In user mode the only addresses that user can read is mapped space. Anything else is a privilege violation.
			 */
			if (unlikely(!access_ok(VERIFY_READ, src_address, sizeof(u32)))) {
				/*
				 * Set  TRAP_CAUSE_PRIV_VIOLATION bit in regs->trap_cause
				 */
				regs->trap_cause = (1 << TRAP_CAUSE_PRIV_VIOLATION);
				unaligned_printk(UNALIGNED_DBG_ERROR, "src_address invalid: %p\n", src_address);
				return -1;
			}
		}
#endif
		/*
		 * get data (possibly unaligned)
		 */
		if (likely(four_byte)) {
			if (copy_from_user(&source_data, (void *)(src_address), 4)) {
				/*
				 * The source address is bad. Change the fault to TRAP_CAUSE_SRC1_DECOEE_ERR.
				 */
				regs->trap_cause = (1 << TRAP_CAUSE_SRC1_DECODE_ERR);
				unaligned_printk(UNALIGNED_DBG_ERROR, "read word failed, src_address: %p\n", src_address);
				return -1;
			}
			source1 = source_data;
		} else {
			unsigned short s_data;
			if (copy_from_user(&s_data, (void *)(src_address), 2)) {
				/*
				 * The source address is bad. Change the fault to TRAP_CAUSE_SRC1_DECOEE_ERR.
				 */
				regs->trap_cause = (1 << TRAP_CAUSE_SRC1_DECODE_ERR);
				unaligned_printk(UNALIGNED_DBG_ERROR, "read short failed, src_address: %p\n", src_address);
				return -1;
			}
			source1 = s_data;
			
			/*
			 * Source is not extended if the instrution is MOVE.2 or
			 * if the cpu CHIP ID >= 0x30000 and the instruction is
			 * either LSL.2 or LSR.2.  All other cases have to be
			 * sign extended.
			 */
			if ((!(op == 2 && subop == MOVE_2)) &&
			    (!((ubicom32_get_chip_id() >= 0x30000) &&
			       (subop == LSL_2 || subop == LSR_2)))) {
				/*
				 * Have to sign extend the .2 entry.
				 */
				source1 = ((unsigned int)
					   ((signed int)
					    ((signed short) source1)));
			}
		}
	} else if (likely(op != MOVEI)) {
		/*
		 * source1 is either an immediate value or it is from a register.z
		 */
		unsigned int s1 = inst & 0x7ff;

		if ((s1 >> 8) == 0) {
			/*
			 * 8 bit signed immediate.
			 */
			source1 = s1 & 0xff;
			if (source1 >> 7) {
				/*
				 * Negative number.
				 */
				source1 |= 0xffffff00;
			}
		} else if ((s1 >> 8) == 1) {
			/*
			 * Direct register
			 */
			s1 &= 0xff;
#if (UBICOM32_ARCH_VERSION >= 5)
			if (user_mode(regs)) {
				/*
				 * User mode has restrictions.
				 */
				if (((s1 >= 0x2d) && (s1 <= 0x35)) || (s1 == 0x38) || (s1 >= 0x40)) {
					/*
					 * Registers not visible in user mode. Create a privilege violation.
					 *
					 * Set  TRAP_CAUSE_PRIV_VIOLATION bit in regs->trap_cause
					 */
					regs->trap_cause = (1 << TRAP_CAUSE_PRIV_VIOLATION);
					unaligned_printk(UNALIGNED_DBG_ERROR, "bad register request, s1: %u\n", s1);
					return -1;
				}
			}
#endif

			if (s1 < 0x40) {
				/*
				 * Data is in the regs structure.
				 */

				if (s1 < 0x10) {
					/*
					 * Data registers.
					 */
					source1 = regs->dn[s1];
				} else if ((s1 >= 0x20) && (s1 <= 0x2a)) {
					/*
					 * from a0 to mac_rc16
					 */
					source1 = regs->an[s1 - 0x20];
				} else if (s1 <= 0x39) {
					switch(s1) {
					case 0x2b:
						source1 = regs->source3;
						break;
					case 0x2c:
						source1 = regs->inst_cnt;
						break;
					case 0x2d:
						source1 = regs->csr;
						break;
					case 0x2e:
						source1 = 0;
						break;
					case 0x2f:
						source1 = 0;
						break;
					case 0x30:
						source1 = regs->int_mask0;
						break;
					case 0x31:
						source1 = regs->int_mask1;
						break;
#if defined(IP8000)
					case 0x32:
						source1 = regs->int_mask2;
						break;
#endif
					case 0x34:
						source1 = regs->pc;
						break;
					case 0x35:
						source1 = regs->trap_cause;
						break;
					case 0x36:
						source1 = regs->acc1[0];
						break;
					case 0x37:
						source1 = regs->acc1[1];
						break;
					case 0x38:
						source1 = regs->previous_pc;
						break;
#if (UBICOM32_ARCH_VERSION >= 5)
					case 0x39:
						source1 = regs->ucsr;
						break;
#endif
					default:
						source1 = 0;
					}
				}
			} else {
				/*
				 * Globals. We have to emulate move.4  source1, register in the current thread to get it.
				 */
				extern unsigned int unaligned_emulate_get_src;
				*((int *)&unaligned_emulate_get_src) &= ~(0x7ff);
				*((int *)&unaligned_emulate_get_src) |= (inst & 0x7ff);
				flush_icache_range((unsigned long)(&unaligned_emulate_get_src),
						   (unsigned long)(&unaligned_emulate_get_src) + 4);

				asm volatile (
					/* source1 uses thread's registers */
				"unaligned_emulate_get_src:			\n\t"
				"	move.4	%0, #0				\n\t"
					: "=d" (source1)
				);
			}
		}
	}

	/*
	 * get the destination address
	 */
	if (dest_operand) {
		DEBUG_ASSERT(format == TWO_OP || format == TWO_OP_2 ||
			 format == DEST || format == DEST_2);
		dest_address = unaligned_get_address(((inst >> 16) & 0x7ff),
						     four_byte, save_an,
						     save_dn, &write_back_an);

#if (UBICOM32_ARCH_VERSION >= 5)
		/*
		 * For 8k and future processors check for pivilege violations.
		 */
		if (user_mode(regs)) {
			/*
			 * In user mode the only addresses that user can read is mapped space. Anything else is a privilege violation.
			 */
			if (unlikely(!access_ok(VERIFY_WRITE, dest_address, sizeof(u32)))) {
				/*
				 * Set  TRAP_CAUSE_PRIV_VIOLATION bit in regs->trap_cause
				 */
				regs->trap_cause = (1 << TRAP_CAUSE_PRIV_VIOLATION);
				unaligned_printk(UNALIGNED_DBG_ERROR, "invalid destination address, dest_address: %p\n", dest_address);
				return -1;
			}
		}
#endif
	}
	
	/*
	 * get source 2 register if needed, and modify inst to use d1 for
	 * source-2 source-2 will come from this thread, not the trapping thread
	 */
	source2 = 0;
	if ((op >= 8 && op <= 0x17) ||
	    ((op == 2 || op == 6) && (inst & 0x4000000))) {
		int src_dn = (inst >> 11) & 0xf;
		source2 = save_dn[src_dn];
		/*
		 * force the emulated instruction to use d1 for source2 operand
		 */
		emulate_inst = (emulate_inst & 0xffff07ff) | 0x800;
	} else if (op == 3) {
		/*
		 * FPU Ops. If bit 15 is 0 then 11 - 14 is the Dn.
		 * If bit 15 is 1 then 11 - 14 is the accumulators.
		 */
		int src2 = (inst >> 11) &0xf;
		if (inst & (1 << 15)) {
			/*
			 * Accumulators. (acc0_lo 0) (acc0_hi 1) (acc1_lo 2) (acc1_hi 3))
			 */
			switch(src2) {
			case 0:
				source2 = regs->acc0[1];
				break;
			case 1:
				source2 = regs->acc0[0];
				break;
			case 2:
				source2 = regs->acc1[1];
				break;
			case 3:
				source2 = regs->acc1[0];
				break;
			}
		} else {
			/*
			 * Data registers.
			 */
			source2 = regs->dn[src2];
		}

		/*
		 * force the emulated instruction to use d1 for source2 operand
		 */
		emulate_inst = (emulate_inst & 0xffff07ff) | 0x800;
	}

	if (likely(op != MOVEI)) {
		/*
		 * change emulated instruction source1 to d0
		 */
		emulate_inst &= ~0x7ff;
		emulate_inst |= 1 << 8;
	}

	if (unlikely(op == 2)) {
		/*
		 * Set destination to d0
		 */
		emulate_inst &= ~(0xf << 16);
	} else if (likely(op != CMPI) || likely(op != 6) || likely(op != 3)) {
		/*
		 * Set general destination field to d0.
		 */
		emulate_inst &= ~(0x7ff << 16);
		emulate_inst |= 1 << 24;
	}

	/*
	 * execute emulated instruction d0, to d0, no memory access
	 * source2 if needed will be in d1
	 * source3, CSR, and accumulators are set up before execution
	 */
	*((unsigned int *)&trap_emulate) = emulate_inst;
	flush_icache_range((unsigned long)(&trap_emulate),
			   (unsigned long)(&trap_emulate) + 4);

	result = ubicom32_emulate_insn(source1, source2, source3,
				       save_acc, &save_csr);

	/*
	 * set the result value
	 */
	if (dest_operand) {
		/*
		 * copy result to memory
		 */
		if (four_byte) {
			if (copy_to_user((void *)(dest_address), (void *)&result, 4)) {
				/*
				 * The destination address is bad. Change the fault to TRAP_CAUSE_DST_DECODE_ERR.
				 */
				regs->trap_cause = (1 << TRAP_CAUSE_DST_DECODE_ERR);
				unaligned_printk(UNALIGNED_DBG_ERROR, "write word failed, dest_address: %p\n", dest_address);
				return -1;
			}
		} else {
			unsigned short s_data = (unsigned short) s_data;
			if (copy_to_user((void *)(dest_address), (void *)&s_data, 2)) {
				/*
				 * The destination address is bad. Change the fault to TRAP_CAUSE_DST_DECODE_ERR.
				 */
				regs->trap_cause = (1 << TRAP_CAUSE_DST_DECODE_ERR);
				unaligned_printk(UNALIGNED_DBG_ERROR, "write short failed, dest_address: %p\n", dest_address);
				return -1;
			}
		}
	} else if (likely(op != CMPI)) {
		/*
		 * For major opcodes other than 3 (FPU) and 6 (DSP) the destination
		 * field is either an immiediate value or a direct register. For
		 * op 2 it is only a 4 bit Dn field.
		 */
		unsigned int is_register = (((inst >> 24) & 0x7) == 1);
		unsigned int reg_index = (inst >> 16) & 0xff;
		if ((op != 3) && (op != 6)) {
			/*
			 * Not FPU and Not DSP.
			 */
			if (op == 2) {
				is_register = 1;
				reg_index &= 0xf;
			}

			if (is_register) {
				/*
				 * Destination is a direct register.
				 */
#if (UBICOM32_ARCH_VERSION >= 5)
				if (user_mode(regs)) {
					/*
					 * User mode has restrictions.
					 */
					if (((reg_index >= 0x2d) && (reg_index <= 0x35)) || (reg_index == 0x38) || (reg_index >= 0x40)) {
						/*
						 * Registers not writable in user mode. Create a privilege violation.
						 *
						 * Set  TRAP_CAUSE_PRIV_VIOLATION bit in regs->trap_cause
						 */
						regs->trap_cause = (1 << TRAP_CAUSE_PRIV_VIOLATION);
						unaligned_printk(UNALIGNED_DBG_ERROR, "bad register request, reg_index: %u\n", reg_index);
						return -1;
					}
				}
#endif
				
				if (reg_index < 0x40) {
					/*
					 * Data goes to the regs structure.
					 */

					if (reg_index < 0x10) {
						/*
						 * Data registers.
						 */
						regs->dn[reg_index] = result;
					} else if ((reg_index >= 0x20) && (reg_index <= 0x2a)) {
						/*
						 * from a0 to mac_rc16
						 */
						regs->an[reg_index - 0x20] = result;
					} else if (reg_index <= 0x39) {
						switch(reg_index) {
						case 0x2b:
							regs->source3 = result;
							break;
						case 0x2c:
							regs->inst_cnt = result;
							break;
						case 0x2d:
							regs->csr = result;
							break;
						case 0x30:
							regs->int_mask0 = result;
							break;
						case 0x31:
							regs->int_mask1 = result;
							break;
#if defined(IP8000)
						case 0x32:
							regs->int_mask2 = result;
							break;
#endif
						case 0x34:
							regs->pc = result;
							break;
						case 0x35:
							regs->trap_cause = result;
							break;
						case 0x36:
							regs->acc1[0] = result;
							break;
						case 0x37:
							regs->acc1[1] = result;
							break;
						case 0x38:
							regs->previous_pc = result;
							break;
#if (UBICOM32_ARCH_VERSION >= 5)
						case 0x39:
							regs->ucsr = result;
							break;
#endif
						}
					}
				} else {
					/*
					 * copy result to a register, using move.4 dest, result
					 */
					extern unsigned int unaligned_trap_set_result;
					*((unsigned int *)&unaligned_trap_set_result) &= ~0x7ff0000;

					if (op == 2) {
						*((unsigned int *)&unaligned_trap_set_result) |=
							((inst & 0x000f0000) | 0x01000000);
					} else {
						*((unsigned int *)&unaligned_trap_set_result) |=
							(inst & 0x7ff0000);
					}
					flush_icache_range((unsigned long)&unaligned_trap_set_result,
							   ((unsigned long)(&unaligned_trap_set_result) + 4));

					asm volatile (
						      /* Write a global register */
					"	unaligned_trap_set_result:			\n\t"
					"	move.4 #0, %0				\n\t"
						:
						: "d" (result)
					);
				}
			}
		}
	}

	/*
	 * bump PC in thread and restore implicit register changes
	 */
#if (UBICOM32_ARCH_VERSION >= 5)
	regs->ucsr = save_csr;
#else
	regs->csr = save_csr;
#endif

	/*
	 * Increment the PC.
	 */
	regs->pc += 4;

	/*
	 * Clear out trap cause.
	 */
	regs->trap_cause = 0;

	return 0;
}

/*
 * unaligned_only()
 *	Return true if either of the unaligned causes are set (and no others).
 */
int unaligned_only(struct pt_regs *regs)
{
	unsigned int unaligned_cause_mask =
		(1 << TRAP_CAUSE_DST_MISALIGNED) |
		(1 << TRAP_CAUSE_SRC1_MISALIGNED);
	int space_ok = 1;
	unsigned int trap_cause = regs->trap_cause;

#if (UBICOM32_ARCH_VERSION >= 5)
	/*
	 * Floating point traps detected, ignore alignment
	 */
	if (regs->csr & UBICOM32_CSR_FPTRAP_MASK) {
		return 0;
	}
#endif

	BUG_ON(trap_cause == 0);
	
#if defined(CONFIG_UNALIGNED_ACCESS_USERSPACE_ONLY)
	/*
	 * Check if this is a kernel stack if so we will not
	 * handle the trap
	 */
	space_ok = user_mode(regs);
#endif
	return space_ok & ((trap_cause & unaligned_cause_mask) == trap_cause);
}
