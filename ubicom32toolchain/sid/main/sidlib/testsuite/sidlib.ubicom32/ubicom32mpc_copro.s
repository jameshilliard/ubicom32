# This was used to generate ip3kmpc_copro.out

	.global __start
__start:

	.text
	.global cop
cop:
	//  Add all the integers stored in addresses base + 0-126
	ldi	r1,#2		// accumulator (change to load 0)
	ld	r4,0(r1)	// base
	ldi	r2,#4		// loop decrement (change to load 2)
	ldi	r3,#128		// offset
	add	r4,r4,r3	// actual address
loop:	
	sub	r4,r4,r2	// decrement address
	ld	r6,0(r4)	// load value
	sub	r3,r3,r2	// decrement index
	jmpne	loop
	add	r1,r1,r6	// result -- in delay slot

	st	0(r4),r1	// store the result
	stop	#123
