# This file is used to generate ip4kcpu.out

	.section ".flram1","ax"
	.global _start

_start:
        movei d4,#12
        moveai a2,#%hi(msg)
        lea.4 d3,%lo(msg)(a2)
        movei d2,#1
        movei d1,#5
        nop	; replace with bset int_set1,#0,#31
        movei d2,#3
        movei d1,#1
        bset int_set1,#0,#31

	.data
msg:  
	.ascii "hello world\n"
	.end
