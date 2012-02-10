/*
 * arch/ubicom32/kernel/head.S
 *	Primary entry point for stating the Kernel.
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
#include <generated/asm-offsets.h>
#include <asm/page_offset.h>
#include <asm/ubicom32.h>


#define SRC_AN A3
#define DST_AN A4

#define PARAM_DN D0
#define TMP_DN D15
#define TMP2_DN D14

/*
 * The following code is placed at the start of the Linux section of memory.
 * This is the primary entry point for Linux.
 *
 * However, we also want the syscall entry/exit code to be at a fixed address.
 * So we take the primary entry point and reserve 16 bytes.  That address is
 * where the system_call entry point exists.  This 16 bytes basically allows
 * us to jump around the system_call entry point code to the actual startup
 * code.
 *
 * Linux Memory Map (see vlinux.lds.S):
 * 0x40400000 - Primary Entry Point for Linux (jump around code below).
 * 0x40400010 - Old syscall Entry Point.
 */

	.sect	.skip_syscall, "ax", @progbits
	.global	__skip_syscall_section
__skip_syscall_section:
	moveai		A3, #%hi(_start)
	lea.1		A3, %lo(_start)(A3)
	ret		A3
/*
 * __os_node_offset contains the offset from KERNELBASE to the os_node, it is
 * not intended to be used by anything except the boot code.
 */
__os_node_offset:
.long	(_os_node - KERNELSTART)

.text
.global	_start

/*
 * start()
 *	This is the start of the Linux kernel.
 */
_start:
	move.4		SCRATCHPAD1, #0

#if defined(IP8000)
	/*
	 * The IP8000 family added 2 more scratchpad registers
	 * which we use for syscall interface.
	 */
	move.4		SCRATCHPAD4, #0
	move.4		SCRATCHPAD5, #0

	/*
	 * System call entry point register (using new register from arch version 5).
	 */
	moveai		a5, #%hi(OS_SYSCALL_BEGIN)
	lea.1		sep, %lo(OS_SYSCALL_BEGIN)(a5)
#endif

	/*
	 * We can't initialize any hardware until we verify that this has been built
	 * for the correct processor.
	 */
	cmpi		CHIP_ID, #0xffff	; If QEMU, skip software type check
	jmpeq.s.f	_ok_to_continue
	
#if defined(IP5000)
	/*
	 * This is the verification when the build is for the IP5K
 	 */
	lsr.4		TMP_DN, CHIP_ID, #16
	cmpi		TMP_DN, #2
	jmpeq.s.t	_ok_to_continue
#endif
#if defined(IP7000)
	/*
	 * This is the verification when the build is for the IP7K
 	 */
	lsr.4		TMP_DN, CHIP_ID, #16
	cmpi		TMP_DN, #3
	jmpeq.s.t	_ok_to_continue
#endif
#if defined(IP8000)
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	/*
	 * This is the verification when the build is for the IP8K shuttle chip
 	 */
	movei		TMP_DN, #0x4		; Shuttle chip is 0x40000
	lsl.4		TMP_DN, TMP_DN, #16
	sub.4		#0, CHIP_ID, TMP_DN
	jmpeq.t		_ok_to_continue
#else /* defined(CONFIG_UBICOM32_IP8K_SHUTTLE) */
	/*
	 * This is the verification when the build is for the IP8K production chip
	 */
	lsr.4		TMP_DN, CHIP_ID, #16	; IP8K is architecture 4
	sub.4		#0, #4, TMP_DN
	jmpne.f		_linux_not_built_for_this_chip
	bfextu		TMP_DN, CHIP_ID, #16	; Chip revision should not be 0
	jmpne.t		_ok_to_continue
#endif /* defined(CONFIG_UBICOM32_IP8K_SHUTTLE) */
#endif /* defined(IP8000) */

        .global _linux_not_built_for_this_chip
_linux_not_built_for_this_chip:
	bkpt		#-1			; Die if this is the wrong software

_ok_to_continue:

	/*
	 * If we would like to enable the BTB for Linux CPUs, then
	 * we should enable the BTB for the current hardware thread.
	 */
#if defined(CONFIG_UBICOM32_ENABLE_BTB)
	tbset		mt_btb_en, mt_btb_en
#endif

/*
 * Setup the range registers... the loader has setup a few, but we will go ahead
 * and correct them for our own limits. Note that once set these are never
 * changed again.  The ranges are as follows
 *
 *  D_RANGE0 - io block (set up by loaded)
 *
 *  I_RANGE0 and D_RANGE1 - kernel/ultra loader address space bottom of ocm-> top
 *	of ram typically 0x3ffc0000 - 0x440000000
 *  I_RANGE1 - kernel / userspace transition area (aka syscalls, context switches)
 *	typically 0x3FFC0030 - ~0x3FFC0200
 *  I_RANGE2 / D_RANGE2 - slab area
 *	typically 0x40A00000 - ~0x44000000
 *  I_RANGE3 - unused
 *  D_RANGE3, D_RANGE4 and D_RANGE5 - unused.
 */
	moveai		SRC_AN, #%hi(OCMSTART)
	lea.4		SRC_AN, %lo(OCMSTART)(SRC_AN)
	move.4		D_RANGE0_LO, SRC_AN
	move.4		I_RANGE0_LO, SRC_AN

; don't try to calculate I_RANGE_HI, see below
;	moveai		SRC_AN, #%hi(___init_end-4)
;	lea.4		SRC_AN, %lo(___init_end-4)(SRC_AN)
;	move.4		I_RANGE0_HI, SRC_AN

	moveai		SRC_AN, #%hi(SDRAMSTART + CONFIG_MIN_RAMSIZE-4)
	lea.4		SRC_AN, %lo(SDRAMSTART + CONFIG_MIN_RAMSIZE-4)(SRC_AN)
	move.4		D_RANGE0_HI, SRC_AN

; for now allow the whole ram to be executable as well so we don't run into problems
; once we load user more code.
	move.4		I_RANGE0_HI, SRC_AN

#ifdef CONFIG_PROTECT_KERNEL
; when kernel protection is enabled, we only open up syscall and non kernel text
; for userspace apps, for now only irange registers registers 1 and 2 are used for userspace.

	;; syscall range
	moveai		SRC_AN, #%hi(__syscall_text_run_begin)
	lea.4		SRC_AN, %lo(__syscall_text_run_begin)(SRC_AN)
	move.4		I_RANGE3_LO, SRC_AN
	moveai		SRC_AN, #%hi(__syscall_text_run_end)
	lea.4		SRC_AN, %lo(__syscall_text_run_end)(SRC_AN)
	move.4		I_RANGE3_HI, SRC_AN

	;; slab instructions
	moveai		SRC_AN, #%hi(_edata)
	lea.4		SRC_AN, %lo(_edata)(SRC_AN)
	move.4		I_RANGE1_LO, SRC_AN
	;; End of DDR is already in range0 hi so just copy it.
	move.4		I_RANGE1_HI, I_RANGE0_HI

	;; slab data (same as slab instructions but starting a little earlier).
	moveai		SRC_AN, #%hi(_data_protection_end)
	lea.4		SRC_AN, %lo(_data_protection_end)(SRC_AN)
	move.4		D_RANGE1_LO, SRC_AN
	move.4		D_RANGE1_HI, I_RANGE0_HI

;; enable ranges
	;; skip I_RANGE0_EN
	move.4		I_RANGE0_EN, #-1
	move.4		I_RANGE1_EN, #-1
	;move.4		I_RANGE3_EN, #0

	;; skip D_RANGE0_EN or D_RANGE1_EN
	move.4		D_RANGE0_EN, #-1
	move.4		D_RANGE1_EN, #-1
	;move.4		D_RANGE4_EN, #0
#endif

;
; If __ocm_free_begin is smaller than __ocm_free_end the
; setup OCM text and data ram banks properly
;
	moveai		DST_AN, #%hi(__ocm_free_begin)
	lea.4		TMP_DN, %lo(__ocm_free_begin)(DST_AN)
	moveai		DST_AN, #%hi(__ocm_free_end)
	lea.4		TMP2_DN, %lo(__ocm_free_end)(DST_AN)
	sub.4		#0, TMP2_DN, TMP_DN
	jmple.f		2f
	moveai		DST_AN, #%hi(__data_begin)
	lea.4		TMP_DN, %lo(__data_begin)(DST_AN)
	moveai		DST_AN, #%hi(OCMSTART)
	lea.4		TMP2_DN, %lo(OCMSTART)(DST_AN)
	sub.4		TMP_DN, TMP_DN, TMP2_DN
	lsr.4		TMP_DN, TMP_DN, #15
	lsl.4		TMP_DN, #1, TMP_DN
	moveai		DST_AN, #%hi(OCMC_BASE)
	add.4		OCMC_BANK_MASK(DST_AN), #-1, TMP_DN
	pipe_flush	0
2:
;
; Load .ocm_text
;
	moveai          DST_AN, #%hi(__ocm_text_run_end)
	lea.4           TMP_DN, %lo(__ocm_text_run_end)(DST_AN)
	moveai          DST_AN, #%hi(__ocm_text_run_begin)
	lea.4           DST_AN, %lo(__ocm_text_run_begin)(DST_AN)
	moveai          SRC_AN, #%hi(__ocm_text_load_begin)
	lea.4           SRC_AN, %lo(__ocm_text_load_begin)(SRC_AN)
	jmpt.t          2f

1:      move.4          (DST_AN)4++, (SRC_AN)4++

2:      sub.4           #0, DST_AN, TMP_DN
	jmpne.t         1b
;
; Load .syscall_text
;
	moveai          DST_AN, #%hi(__syscall_text_run_end)
	lea.4           TMP_DN, %lo(__syscall_text_run_end)(DST_AN)
	moveai          DST_AN, #%hi(__syscall_text_run_begin)
	lea.4           DST_AN, %lo(__syscall_text_run_begin)(DST_AN)
	moveai          SRC_AN, #%hi(__syscall_text_load_begin)
	lea.4           SRC_AN, %lo(__syscall_text_load_begin)(SRC_AN)
	jmpt.t          2f

1:	move.4          (DST_AN)4++, (SRC_AN)4++

2:	sub.4           #0, DST_AN, TMP_DN
	jmpne.t         1b

;
; Load .ocm_data
;
	moveai          DST_AN, #%hi(__ocm_data_run_end)
	lea.4           TMP_DN, %lo(__ocm_data_run_end)(DST_AN)
	moveai          DST_AN, #%hi(__ocm_data_run_begin)
	lea.4           DST_AN, %lo(__ocm_data_run_begin)(DST_AN)
	moveai          SRC_AN, #%hi(__ocm_data_load_begin)
	lea.4           SRC_AN, %lo(__ocm_data_load_begin)(SRC_AN)
	jmpt.t          2f

1:      move.4          (DST_AN)4++, (SRC_AN)4++

2:      sub.4           #0, DST_AN, TMP_DN
	jmpne.t         1b

; Clear .bss
;
	moveai		SRC_AN, #%hi(_ebss)
	lea.4		TMP_DN, %lo(_ebss)(SRC_AN)
	moveai		DST_AN, #%hi(_sbss)
	lea.4		DST_AN, %lo(_sbss)(DST_AN)
	jmpt.t		2f

1:	move.4		(DST_AN)4++, #0

2:	sub.4		#0, DST_AN, TMP_DN
	jmpne.t		1b

; save our parameter to devtree (after clearing .bss)
	moveai		DST_AN, #%hi(devtree)
	lea.4		DST_AN, %lo(devtree)(DST_AN)
	move.4		(DST_AN), PARAM_DN

; We are going to initialize a standard kernel stack frame.
; The format of this is described in detail in ubicom32_syscall.S
; We need to find the end of the page and then leave room for 8 bytes
; of padding and a pt_regs structure.  The stack grows backwards away
; from the pt_regs structure.
	moveai		sp, #%hi(init_thread_union)
	lea.4		sp, %lo(init_thread_union)(sp)
	movei		TMP_DN, #(ASM_THREAD_SIZE - 8 - PT_SIZE)
	add.4		sp, sp, TMP_DN
; Stack pointer is now at its starting point which also is the address of the
; pt_regs structure
	move.4		PT_NESTING_LEVEL(sp), #0
	move.4		PT_THREAD_TYPE(sp), #1 	; KERNEL_THREAD
	move.4		PT_FRAME_TYPE(sp), #4	; NEW_THREAD

;; ip3k-elf-gdb backend now sets scratchpad3 to 1 when either continue
;; or single step commands are issued. scratchpad3 is set to 0 when the
;; debugger detaches from the board.
	move.4		TMP_DN, scratchpad3
	lsl.4		TMP_DN, TMP_DN, #0x0
	jmpeq.f		_jump_to_start_kernel
_ok_to_set_break_points_in_linux:
;; THREAD_STALL
	move.4		mt_dbg_active_clr,#-1
;; stalling the threads isn't instantaneous.. need to flush the pipe.
	pipe_flush	0
	pipe_flush	0

_jump_to_start_kernel:
	moveai		SRC_AN, #%hi(start_kernel)
	lea.4		SRC_AN, %lo(start_kernel)(SRC_AN)
	ret		SRC_AN
