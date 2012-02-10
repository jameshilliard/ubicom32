/*
 * ip5000.inc
 *
 * Copyright © 2005-2011 Ubicom Inc. <www.ubicom.com>.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
#define ubicom32_pipe_flush() asm("pipe_flush")
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
#define ubicom32_setcsr_flush() asm("setcsr_flush")
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

#endif /* !defined(__LINKER__) */
#endif /* defined _ASM_UBICOM32_UBICOM32_ASM_H */
