/*
 * arch/ubicom32/include/asm/ubicom32-asm.h
 *	Instruction macros for the UBICOM32 processor.
 *
 * (C) Copyright 2009-2010, Ubicom, Inc.
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

#ifndef _ASM_UBICOM32_UBICOM32_ASM_H
#define _ASM_UBICOM32_UBICOM32_ASM_H

/*
 * Timing
 */
#if defined(IP5000) || defined(IP7000)
#define JMPT_PENALTY 3
#define JMPF_PENALTY 7
#define RET_PENALTY 7
#elif defined(IP8000)
#define JMPT_PENALTY 3
#define JMPF_PENALTY 8
#define RET_PENALTY 8
#else
#error "Unknown UBICOM32 silicon"
#endif

#if !defined(__LINKER__)

#if defined(__ASSEMBLY__)
	.macro	nops	quant
	.if	\quant > 0
	.rept	\quant
		nop
	.endr
	.endif
	.endm
#else
/*
 * Same macro as above just in C inline asm
 */
asm (
"	.macro	nops	quant		\n"
"	.if	\\quant > 0		\n"
"	.rept	\\quant			\n"
"		nop			\n"
"	.endr				\n"
"	.endif				\n"
"	.endm				\n"
);
#endif

/*
 * IP5000 and IP7000 takes 8 cycles for jmpt.f.
 * IP8000 takes 9 cycles for jmpt.f
 */
#if defined(__ASSEMBLY__)
	.macro	cycles	quant
	.if	(\quant) == 1
		nop
	.else
	.if	(((\quant) + 3) / 8) > 0
	.rept	(((\quant) + 3) / 8)
		jmpt.f		.+4
	.endr
	.endif
	.if	((((\quant) + 3) % 8) / 4) > 0
		jmpt.t		.+4
	.endif
	.endif
	.endm
#else
/*
 * Same macro as above just in C inline asm
 */
asm (
"						\n"
"	.macro	cycles	quant			\n"
"	.if	(\\quant) == 1			\n"
"		nop				\n"
"	.else					\n"
"	.if	(((\\quant) + 3) / 8) > 0	\n"
"	.rept	(((\\quant) + 3) / 8)		\n"
"		jmpt.f		.+4		\n"
"	.endr					\n"
"	.endif					\n"
"	.if	((((\\quant) + 3) % 8) / 4) > 0	\n"
"		jmpt.t		.+4		\n"
"	.endif					\n"
"	.endif					\n"
"	.endm					\n"
);
#endif

/*
 * IP5000 and IP7000 requires 11 cycles
 * IP8000 requires 12 cycles
 */
#if defined(__ASSEMBLY__)
	.macro	pipe_flush	cyc
		cycles		12 - (\cyc)
	.endm
#else
/*
 * Same macro as above just in C inline asm
 */
asm (
"	.macro	pipe_flush	cyc		\n"
"		cycles		12 - (\\cyc)	\n"
"	.endm					\n"
);
#define ubicom32_pipe_flush() asm volatile("pipe_flush 0")
#endif

/*
 * IP5000 and IP7000 requires 5 cycles
 * IP8000 requires 6 cycles
 */
#if defined(__ASSEMBLY__)
	.macro	setcsr_flush	cyc
		cycles		6 - (\cyc)
	.endm
#else
/*
 * Same macro as above just in C inline asm
 */
asm (
"	.macro	setcsr_flush	cyc		\n"
"		cycles		6 - (\\cyc)	\n"
"	.endm					\n"
);
#define ubicom32_setcsr_flush() asm("setcsr_flush 0")
#endif

/*
 * Macros for prefetch (using miss-aligned memory write)
 */
#if defined(__ASSEMBLY__)

#if defined(IP5000) || defined(IP7000)
	.macro	pre_fetch_macro	thread_num Ascratch Aaddress length
		bclr		MT_TRAP_EN, MT_TRAP_EN, #(\thread_num)
		bset		\Ascratch, \Aaddress, #0	; force a miss-aligned address
		jmpt.t		.+4				; delay for both address setup and trap disable
		move.4		(\Ascratch), #0
		.if		(\length > 32)
		move.4		32(\Ascratch), #0
		.endif
		.if		(\length > 64)
		move.4		64(\Ascratch), #0
		.endif
		.if		(\length > 96)
		move.4		96(\Ascratch), #0
		.endif
		.if		(\length > 128)
		invalid_instruction				; maximum pre-fetch size is 4 cache lines
		.endif
		bset		MT_TRAP_EN, MT_TRAP_EN, #(\thread_num)
	.endm
#elif defined(IP8000)
	.macro	pre_fetch_macro	thread_num Ascratch Aaddress length
		prefetch	(\Aaddress)
		.if		(\length > 32)
		prefetch	32(\Aaddress)
		.endif
		.if		(\length > 64)
		prefetch	64(\Aaddress)
		.endif
		.if		(\length > 96)
		prefetch	96(\Aaddress)
		.endif
		.if		(\length > 128)
		invalid_instruction				; maximum pre-fetch size is 4 cache lines
		.endif
	.endm
#endif
#endif

/*
 * scratchpad1 is owned by the PIC.
 *
 * The upper bits provide 16 global spinlocks.  Acquiring one of these
 * global spinlocks synchornizes across multiple threads and prevents
 * the PIC from delivering any interrupts while the lock is held.
 * Use these locks only when absolutely required.
 *
 * The lower 16 bits of scratchpad1 are used as per thread interrupt
 * enable/disable bits.  These bits will prevent a thread from receiving
 * any interrupts.
 *
 * PROFILER:  The profiler displays statistics on lock usage, so if these bit
 * assignments are changed, please inform the profiler developer
 *
 * Bit Usage:
 * - MT_EN_LOCK_BIT	- Protects writes to MT_EN, so code can read current value
 *			then write a new value atomically (profiler for example)
 * - ATOMIC_LOCK_BIT	- Used to provide general purpose atomic handling.
 * - PIC_LOCK_BIT	- Used by the PIC exclusively to provide protection.
 * - DCCR_LOCK_BIT	- Used to limit access to the DCCR cache control peripheral
 * - ICCR_LOCK_BIT	- Used to limit access to the ICCR cache control peripheral
 * - TIMER_LOCK_BIT	- Use by the timer code since the lock proof code needs timers
 * - USER_ATOMIC_LOCK_BIT - Used by the lwatomic code for user space atomics
 * - USB_AERROR_BIT	- Hardware work around for shuttle IP8K
 * - LSB 16 bits     - Used by the PIC to represent thread enable/disable bits.
 */
#define MT_EN_LOCK_BIT		31
#define ATOMIC_LOCK_BIT 	30
#define PIC_LOCK_BIT		29
#define PCI_LOCK_BIT		28
#define ICCR_LOCK_BIT		27
#define DCCR_LOCK_BIT		26
#define TLB_LOCK_BIT		25
#define TIMER_LOCK_BIT		24
#define USER_ATOMIC_LOCK_BIT	23
#define USB_AERROR_LOCK_BIT	22
#define GMAC_LOCK_BIT		21

#if !defined(__ASSEMBLY__)

#define UBICOM32_IS_LOCKED(bit) \
	asm volatile (						      \
	"	move.4 %0, #0					\n\t" \
	"	btst scratchpad1, #"D(bit)"			\n\t" \
	"	jmpeq.f	1f					\n\t" \
	"	move.4 %0, #1					\n\t" \
	"1:							\n\t" \
		: "=r" (ret)					      \
		:						      \
		: "cc", "memory"				      \
	)							      \

#define UBICOM32_TRYLOCK(bit) \
	asm volatile (						      \
	"	move.4 %0, #0					\n\t" \
	"	bset scratchpad1, scratchpad1, #"D(bit)"	\n\t" \
	"	jmpne.f	1f					\n\t" \
	"	move.4 %0, #1					\n\t" \
	"1:							\n\t" \
		: "=r" (ret)					      \
		:						      \
		: "cc", "memory"				      \
	)							      \

#define UBICOM32_UNLOCK(bit) \
	asm volatile (						      \
	"	bclr scratchpad1, scratchpad1, #"D(bit)"	\n\t" \
		:						      \
		:						      \
		: "cc", "memory"				      \
	)							      \

#define UBICOM32_LOCK(bit) \
	asm volatile (						      \
	"1:	bset scratchpad1, scratchpad1, #"D(bit)"	\n\t" \
	"	jmpne.f	1b					\n\t" \
		:						      \
		:						      \
		: "cc", "memory"				      \
	)							      \

/*
 * __atomic_lock_trylock()
 *	Attempt to acquire the lock, return TRUE if acquired.
 */
static inline int __atomic_lock_trylock(void)
{
	int ret;
	UBICOM32_TRYLOCK(ATOMIC_LOCK_BIT);
	return ret;
}

/*
 * __atomic_lock_release()
 *	Release the global atomic lock.
 *
 * Note: no one is suspended waiting since this lock is a spinning lock.
 */
static inline void __atomic_lock_release(void)
{
	UBICOM32_UNLOCK(ATOMIC_LOCK_BIT);
}

/*
 * __atomic_lock_acquire()
 *	Acquire the global atomic lock, spin if not available.
 */
static inline void __atomic_lock_acquire(void)
{
	UBICOM32_LOCK(ATOMIC_LOCK_BIT);
}
#else /* __ASSEMBLY__ */

/*
 * atomic_lock_acquire macro
 *	Equivalent to __atomic_lock_acquire()
 */
	.macro atomic_lock_acquire
		bset scratchpad1, scratchpad1, #ATOMIC_LOCK_BIT
		jmpne.f	.-4
	.endm

/*
 * atomic_lock_release macro
 *	Equivalent to __atomic_lock_release()
 */
	.macro atomic_lock_release
		bclr scratchpad1, scratchpad1, #ATOMIC_LOCK_BIT
	.endm

#endif /* __ASSEMBLY__ */

#endif /* !defined(__LINKER__) */
#endif /* defined _ASM_UBICOM32_UBICOM32_ASM_H */
