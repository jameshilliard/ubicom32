# This file is used generate ip4kcopro.out
	
	.data
hellomsg:
	.ascii "hello world\n"
	.text
	.global __start
__start:
	ldi r2,#%data(hellomsg)
	ldi r3,#12
	ldi r1,#1
	ldi r0,#5
	nop
	ldi r1,#0
	ldi r0,#1
	stop #37
	.end
