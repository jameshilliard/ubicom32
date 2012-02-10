/*
 * arch/ubicom32/kernel/emulate.S
 *	Implements unaligned emulator
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
#include <linux/sys.h>
#include <linux/linkage.h>
#include <asm/ubicom32.h>

#if (UBICOM32_ARCH_VERSION < 5)
/*
 * 5k/7k Version
 * ubicom32_emulate_insn()
 *	Emulates the instruction.
 *
 * Call with:
 *	unsigned int ubicom32_emulate_insn(int source1, int source2, 
 *		int source3, int *save_acc, int *save_csr);
 */
	.section .text.ubicom32_emulate_insn, "ax", @progbits
	.global ubicom32_emulate_insn
	.global trap_emulate
ubicom32_emulate_insn:
	movea	a3, d3		; a3 holds save_acc pointer
	movea	a4, d4		; a4 hods save_csr pointer
	move.4	CSR, (a4)
	setcsr_flush 0
	move.4	source3, d2
	move.4	acc0_hi, (a3)
	move.4	acc0_lo, 4(a3)
	move.4	mac_rc16, 8(a3)
	move.4	acc1_hi, 12(a3)
	move.4	acc1_lo, 16(a3)

trap_emulate:
	move.4	d0, d1
	setcsr_flush 0
	move.4	(a3), acc0_hi
	move.4	4(a3), acc0_lo
	move.4	8(a3), mac_rc16
	move.4	12(a3), acc1_hi
	move.4	16(a3), acc1_lo
	move.4	(a4), CSR	; Save csr
	ret	a5
	.size ubicom32_emulate_insn, . - ubicom32_emulate_insn
#else
/*
 * 8k version
 * ubicom32_emulate_insn()
 *	Emulates the instruction.
 *
 * Call with:
 *	unsigned int ubicom32_emulate_insn(int source1, int source2, 
 *		int source3, int *save_acc, int *save_ucsr);
 */
	.section .text.ubicom32_emulate_insn, "ax", @progbits
	.global ubicom32_emulate_insn
	.global trap_emulate
ubicom32_emulate_insn:
	movea	a3, d3		; a3 holds save_acc pointer
	movea	a4, d4		; a4 hods save_csr pointer
	move.4	UCSR, (a4)
	move.4	source3, d2
	move.4	acc0_hi, (a3)
	move.4	acc0_lo, 4(a3)
	move.4	mac_rc16, 8(a3)
	move.4	acc1_hi, 12(a3)
	move.4	acc1_lo, 16(a3)

trap_emulate:
	move.4	d0, d1
	move.4	(a3), acc0_hi
	move.4	4(a3), acc0_lo
	move.4	8(a3), mac_rc16
	move.4	12(a3), acc1_hi
	move.4	16(a3), acc1_lo
	move.4	(a4), UCSR	; Save ucsr
	ret	a5
	.size ubicom32_emulate_insn, . - ubicom32_emulate_insn
#endif
