# This source was used to generate ip3kmpc_main.out

	.section ".flram1","ax"
	.global _start
_start:
        movei d4,#12
        moveai a2,#%hi(msg)
        lea.4 d12,%lo(msg)(a2)

	movei   d10,#0		; clear d10
	; Compute base address of coprocessor registers
	moveai	a0,#%hi(0x400000)
	lea.1	a0,%lo(0x400000)(a0)
	movei	d1,#1		; index of control register
	movei	d2,#4		; index of cpPC    register
	movei	d5,#2		; index of cp bank map register
	movei	d6,#3		; index of mp bank map register
	movei	d7,#5		; index of exception pc register
	movei	d8,#6		; index of exception status
	movei	d9,#7		; index of watchpoint register
	
	; Compute base address of coprocessor insn memory
	moveai	a1,#%hi(0x410000)
	lea.1	a1,%lo(0x410000)(a1)
	
	; Compute base address of coprocessor data memory bank 0
	moveai	a2,#%hi(0x430000)
	lea.1	a2,%lo(0x430000)(a2)
	
	; Compute base address of coprocessor data memory bank 1
	moveai	a3,#%hi(0x432000)
	lea.1	a3,%lo(0x432000)(a3)

	; Compute base address of coprocessor data memory bank 2
	moveai	a4,#%hi(0x434000)
	lea.1	a4,%lo(0x434000)(a4)

	; Make sure the coprocessor is offline
	movei	d3,#0x0080
	move.4	(a0,d1),d3	; set coprocessor control register

	; Map coprocessor logical memory banks to their corresponding
	; physical banks
	moveai	a5,#%hi(0x76543210)
	lea.1	d3,%lo(0x76543210)(a5)
	move.4	(a0,d5),d3	; set cp bank map register
	move.4	(a0,d6),d3	; set mp bank map register
	; Load the first 64 integers into the coprocessor data memory bank 1
	move.4	a5,a3		; address of coprocessor data memory bank 1
	movei	d3,#64
dloop:	move.2	(a5)2++,d3	; move the data
	add.2	d3,#-1,d3	; decrement the index
	jmpne.s	dloop		; loop again?

	;; Load the base address into copro bank 0, offset 0
	movei	0(a2),#0x2000

	;; Enable coprocessor interrupts
	movei	d3,#0x3		; completion and exception interrupts
	or.4	int_clr1,int_clr1,d3	; clear these interrupts
	or.4	int_mask1,int_mask1,d3	; enable these interrupts
	movei	d3,#0x1			; enable interrupts in general
	or.4	global_ctrl,global_ctrl,d3

	;; Start the coprocessor and tell it to interrupt us.
	movei	d3,#0x0000
	move.4	(a0,d2),d3	; set starting pc
	movei	d3,#0x0081
	move.4	(a0,d1),d3	; start the coprocessor

	;; Wait for the coprocessor
	suspend

	;; Verify that the coprocessor finished
	move.4 d3,int_stat1
	and.4 #0,#0x1,d3	; Check coprocessor interrupt bits
	jmpeq.w bad

	;; Verify that the coprocessor did not finish with an exception
	and.4 #0,#0x2,d3	; Check coprocessor interrupt bits
	jmpne.w bad

	;; get the result of the calc
	move.2	d10,(a3)
	movei d7,#2081
	sub.4 #0,d7,d10
	jmpne.w bad

        move.4 d3,d12
        movei d2,#1
        movei d1,#5
        nop     ; replace with bset int_set1,#0,#31

bad:	
	movei d2,#0
	movei d1,#1
	bset int_set1,#0,#31

        .data
msg:
        .ascii "hello world\n"
        .end
