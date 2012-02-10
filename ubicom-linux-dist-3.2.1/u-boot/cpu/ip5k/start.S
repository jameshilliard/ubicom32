
#include <config.h>
#include <version.h>

#include <asm/ip5000.h>

#define SRC_AN A3
#define DST_AN A4

#define PARAM_DN D0
#define TMP_DN D15

/*
 * Allow access to the entire memory range for u-boot to aid hardware bringup.
 */
#define RANGE_LO	0
#define RANGE_HI	0x7ffffffc

.global _start
_start:
	moveai		A0, #0
	move.4		SCRATCHPAD1, #0

	moveai		SRC_AN, #%hi(RANGE_LO)
	lea.4		SRC_AN, %lo(RANGE_LO)(SRC_AN)
	move.4		D_RANGE1_LO, SRC_AN
	move.4		I_RANGE0_LO, SRC_AN

	moveai		SRC_AN, #%hi(RANGE_HI)
	lea.4		SRC_AN, %lo(RANGE_HI)(SRC_AN)
	move.4		D_RANGE1_HI, SRC_AN
	move.4		I_RANGE0_HI, SRC_AN

; Move the stack to end of RAM
	moveai		SRC_AN, #%hi(CONFIG_RAMBASE+CONFIG_RAMSIZE-CONFIG_ARGS_SZ-4)
	lea.4		SRC_AN, %lo(CONFIG_RAMBASE+CONFIG_RAMSIZE-CONFIG_ARGS_SZ-4)(SRC_AN)
	move.4		A6, SRC_AN
	move.4		SP, SRC_AN

; Clear .bss
;
	moveai		SRC_AN, #%hi(_end)
	lea.4		TMP_DN, %lo(_end)(SRC_AN)
	moveai		DST_AN, #%hi(__bss_start)
	lea.4		DST_AN, %lo(__bss_start)(DST_AN)
	jmpt.t		2f

1:	move.4		(DST_AN)4++, #0

2:	sub.4		#0, DST_AN, TMP_DN
	jmpne.t		1b

; save our parameter to devtree (after clearing .bss)
	moveai		DST_AN, #%hi(devtree)
	lea.4		DST_AN, %lo(devtree)(DST_AN)
	move.4		(DST_AN), PARAM_DN

#if 0
	moveai		sp, #%hi(init_thread_union)
	lea.4		sp, %lo(init_thread_union)(sp)
	movei		TMP_DN, #ASM_THREAD_SIZE
	add.4		sp, sp, TMP_DN
	move.4		-4(sp)++, #0 ; nesting level = 0
	move.4		-4(sp)++, #1 ; KERNEL_THREAD
#endif

;; ip3k-elf-gdb backend now sets scratchpad3 to 1 when either continue
;; or single step commands are issued. scratchpad3 is set to 0 when the
;; debugger detaches from the board.
        move.4          TMP_DN, scratchpad3
        lsl.4           TMP_DN, TMP_DN, #0x0
        jmpeq.f         _jump_to_start_u_boot
_ok_to_set_break_points_in_linux:
;; THREAD_STALL
        move.4          mt_dbg_active_clr,#-1
;; stalling the threads isn't instantaneous.. need to flush the pipe.
        pipe_flush      0
        pipe_flush      0

_jump_to_start_u_boot:
	moveai		SRC_AN, #%hi(board_init)
	lea.4		SRC_AN, %lo(board_init)(SRC_AN)
	ret		SRC_AN

