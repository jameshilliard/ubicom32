long __udivsi3 (long, long) __attribute__((naked));
long __umodsi3 (long, long) __attribute__((naked));
long __divsi3 (long, long) __attribute__((naked));
long __modsi3 (long, long) __attribute__((naked));

long
__udivsi3 (long a, long b)
{
	asm volatile (
	/* See if our divisor is 0 or 1.  If it's either of these then we
	   take a fast-path to exit.  */
	"	add.4		#0, #-2, d1	\n\t"
	"	jmpcc.w.f	8f		\n\t"

	"	move.4		d3, d0		\n\t"

	/* Is the divide result zero?  We need to check this now because
	   the next part of the algorithm relies on there being a non-zero
	   result.  */
	"	movei		d0, #0		\n\t"
	"	sub.4		d2, d3, d1	\n\t"
	"	jmplo.w.f	8f		\n\t"

	/*
	 * Now we do an early-out optimization.  Lots of divides end up with
	 * a result of 1 and we can check for this really easily.  If we
	 * do have such a result then we bypass a huge amount of work which
	 * will on average more than compensate for slowing the other cases
	 * by two instructions.
	 */
	"	sub.4		#0, d2, d1	\n\t"
	"	jmplo.w.f	9f		\n\t"

#ifndef __UBICOM32_V5__
	/* We need to find how many bits the result will contain.  We do this
	   by finding a bitmask where all trailing bits after the first set bit
	   of both the dividend and divisor are set to ones.  We invert the mask
	   from the divisor and anding the two together gives a block of one
	   bits equal in size to the result.  We can then use a population count
	   algorithm to count the number of one bits.

	   As we don't have any good ways to work with MSbits we reverse things
	   and work with LSbits instead.

	   Start with the dividend.  */
	"	bfrvrs		d7, d3, #0	\n\t"
	"	sub.4		d8, #0, d7	\n\t"
	"	and.4		d8, d8, d7	\n\t"
	"	add.4		d8, #-1, d8	\n\t"
	"	bfrvrs		d7, d1, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	add.4		d9, #-1, d9	\n\t"

	/* Invert the divisor mask and create the result mask.  */
	"	not.4		d8, d8		\n\t"
	"	and.4		d2, d8, d9	\n\t"

	/* Use a SWAR algorithm to do the population (ones) count.  */
	"	lsr.4		d7, d2, #1	\n\t"
	"	movei		d9, #0x5555	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d7, d7, d9	\n\t"
	"	sub.4		d2, d2, d7	\n\t"
	"	lsr.4		d7, d2, #2	\n\t"
	"	movei		d9, #0x3333	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d7, d7, d9	\n\t"
	"	and.4		d8, d2, d9	\n\t"
	"	add.4		d2, d7, d8	\n\t"
	"	lsr.4		d7, d2, #4	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	movei		d9, #0x0f0f	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d2, d2, d9	\n\t"
	"	lsr.4		d7, d2, #8	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	lsr.4		d7, d2, #16	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	and.4		d2, #0x3f, d2	\n\t"
#else
	/* We need to find how many bits the result will contain.  We do this
	   by finding the MSBits of both the dividend and divisor and
	   subtracting one from the other to form a bit shift.

	   As we don't have an instruction to compute the leading zeroes we
	   reverse the two words and compute trailing zeroes.  We don't have
	   an instruction for that as such either but we can use the FPU!

	   Start with the dividend.  */
	"	bfrvrs		d7, d3, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	move.4		d8, #0		\n\t"
	"	fl2s		acc0_lo, d8	\n\t"
	"	bfrvrs		d7, d1, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	fl2s		acc1_lo, d8	\n\t"
	"	nop				\n\t"
	"	lsr.4		d8, acc0_lo, #23\n\t"
	"	add.4		d8, #-127, d8	\n\t"
	"	nop				\n\t"
	"	nop				\n\t"
	"	lsr.4		d9, acc1_lo, #23\n\t"
	"	add.4		d9, #-127, d9	\n\t"
	"	sub.4		d2, d9, d8	\n\t"
#endif

	/* We're going to unroll by a factor of 8 but we need to jump in
	   part way through on the first iteration.  */
	"	and.4		d9, #7, d2	\n\t"
	"	sub.4		d9, #7, d9	\n\t"
	"	lsl.4		d8, d9, #2	\n\t"
	"	add.4		d9, d9, d8	\n\t"

	/* Align divisor to the upper-most bit of the result.  */
	"	lsl.4		d1, d1, d2	\n\t"

	/* Jump into our partially unrolled long division body.  */
	"	call		a4, 1f		\n\t"

	"1:	lea.4		a4, (a4, d9)	\n\t"
	"	calli		a4, 8(a4)	\n\t"

	/* Now do a long division.  */
	"3:	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	/* Close the loop - do any additional blocks of 8 bits, but we're predicting
	   that we rarely take any additional trips.  Winning for small results and
	   doesn't lose much for large ones.  */
	"	add.4		d2, #-8, d2	\n\t"
	"	jmppl.w.f	3b		\n\t"

	"8:	calli		a5, 0(a5)	\n\t"

	/* Special case exit path for the result being 1.  */
	"9:	movei		d0, #1		\n\t"
	"	calli		a5, 0(a5)	\n\t"
		:
		:
#ifndef __UBICOM32_V5__
		: "d0", "d1", "d2", "d3", "d7", "d8", "d9", "a4", "source3"
#else
		: "d0", "d1", "d2", "d3", "d7", "d8", "d9", "a4", "source3",
		  "acc0_lo", "acc1_lo"
#endif
	);
}

long
__umodsi3 (long a, long b)
{
	asm volatile (
	/* Check if the divisor is zero.  If it is then quit.  We should
	   probably abort or something.  */
	"	lsl.4		d3, d1, #0	\n\t"
	"	jmpeq.w.f	9f		\n\t"

	/* Is the divide result zero?  We need to check this now because
	   the next part of the algorithm relies on there being a non-zero
	   result.  */
	"	sub.4		#0, d0, d1	\n\t"
	"	jmplo.w.f	9f		\n\t"

#ifndef __UBICOM32_V5__
	/* We need to find how many bits the result will contain.  We do this
	   by finding a bitmask where all trailing bits after the first set bit
	   of both the dividend and divisor are set to ones.  We invert the mask
	   from the divisor and anding the two together gives a block of one
	   bits equal in size to the result.  We can then use a population count
	   algorithm to count the number of one bits.

	   As we don't have any good ways to work with MSbits we reverse things
	   and work with LSbits instead.

	   Start with the dividend.  */
	"	bfrvrs		d7, d0, #0	\n\t"
	"	sub.4		d8, #0, d7	\n\t"
	"	and.4		d8, d8, d7	\n\t"
	"	add.4		d8, #-1, d8	\n\t"
	"	bfrvrs		d7, d1, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	add.4		d9, #-1, d9	\n\t"

	/* Invert the divisor mask and create the result mask.  */
	"	not.4		d8, d8		\n\t"
	"	and.4		d2, d8, d9	\n\t"

	/* Use a SWAR algorithm to do the population (ones) count.  */
	"	lsr.4		d7, d2, #1	\n\t"
	"	movei		d9, #0x5555	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d7, d7, d9	\n\t"
	"	sub.4		d2, d2, d7	\n\t"
	"	lsr.4		d7, d2, #2	\n\t"
	"	movei		d9, #0x3333	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d7, d7, d9	\n\t"
	"	and.4		d8, d2, d9	\n\t"
	"	add.4		d2, d7, d8	\n\t"
	"	lsr.4		d7, d2, #4	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	movei		d9, #0x0f0f	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d2, d2, d9	\n\t"
	"	lsr.4		d7, d2, #8	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	lsr.4		d7, d2, #16	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	and.4		d2, #0x3f, d2	\n\t"
#else
	/* We need to find how many bits the result will contain.  We do this
	   by finding the MSBits of both the dividend and divisor and
	   subtracting one from the other to form a bit shift.

	   As we don't have an instruction to compute the leading zeroes we
	   reverse the two words and compute trailing zeroes.  We don't have
	   an instruction for that as such either but we can use the FPU!

	   Start with the dividend.  */
	"	bfrvrs		d7, d0, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	move.4		d8, #0		\n\t"
	"	fl2s		acc0_lo, d8	\n\t"
	"	bfrvrs		d7, d1, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	fl2s		acc1_lo, d8	\n\t"
	"	nop				\n\t"
	"	lsr.4		d8, acc0_lo, #23\n\t"
	"	add.4		d8, #-127, d8	\n\t"
	"	nop				\n\t"
	"	nop				\n\t"
	"	lsr.4		d9, acc1_lo, #23\n\t"
	"	add.4		d9, #-127, d9	\n\t"
	"	sub.4		d2, d9, d8	\n\t"
#endif

	/* We're going to unroll by a factor of 8 but we need to jump in
	   part way through on the first iteration.  */
	"	and.4		d9, #7, d2	\n\t"
	"	sub.4		d9, #7, d9	\n\t"
	"	lsl.4		d9, d9, #2	\n\t"

	/* Align divisor to the upper-most bit of the result.  */
	"	lsl.4		d1, d1, d2	\n\t"

	/* Jump into our partially unrolled long division body.  */
	"	call		a4, 1f		\n\t"

	"1:	lea.4		a4, (a4, d9)	\n\t"
	"	calli		a4, 8(a4)	\n\t"

	/* Now do a long division.  */
	"3:	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	/* Close the loop - do any additional blocks of 8 bits, but we're predicting
	   that we rarely take any additional trips.  Winning for small results and
	   doesn't lose much for large ones.  */
	"	add.4		d2, #-8, d2	\n\t"
	"	jmppl.w.f	3b		\n\t"

	"9:	calli		a5, 0(a5)	\n\t"
		:
		:
#ifndef __UBICOM32_V5__
		: "d0", "d1", "d2", "d3", "d7", "d8", "d9", "a4", "source3"
#else
		: "d0", "d1", "d2", "d3", "d7", "d8", "d9", "a4", "source3",
		  "acc0_lo", "acc1_lo"
#endif
	);
}

long
__divsi3 (long a, long b)
{
	asm volatile (
	/* See if our divisor is 0 or 1.  If it's either of these then we
	   take a fast-path to exit.  */
	"	add.4		#0, #-2, d1	\n\t"
	"	jmpcc.w.f	8f		\n\t"

	/* Set D5 equal to -1 if we have a negative dividend (a), and set D3
	   equal to the absolute value of the dividend.  */
	"	asr.4		d5, d0, #31	\n\t"
	"	xor.4		d4, d5, d0	\n\t"
	"	sub.4		d3, d4, d5	\n\t"

	/* Set D6 equal to -1 if we have a negative divisor (b), and set D1
	   equal to the absolute value of the divisor.  */
	"	asr.4		d6, d1, #31	\n\t"
	"	xor.4		d4, d6, d1	\n\t"
	"	sub.4		d1, d4, d6	\n\t"

	/* Is the divide result zero?  We need to check this now because
	   the next part of the algorithm relies on there being a non-zero
	   result.  */
	"	movei		d0, #0		\n\t"
	"	sub.4		d2, d3, d1	\n\t"
	"	jmplo.w.f	8f		\n\t"

	/*
	 * Now we do an early-out optimization.  Lots of divides end up with
	 * a result of +/-1 and we can check for this really easily.  If we
	 * do have such a result then we bypass a huge amount of work which
	 * will on average more than compensate for slowing the other cases
	 * by two instructions.
	 */
	"	sub.4		#0, d2, d1	\n\t"
	"	jmplo.w.f	9f		\n\t"

#ifndef __UBICOM32_V5__
	/* We need to find how many bits the result will contain.  We do this
	   by finding a bitmask where all trailing bits after the first set bit
	   of both the dividend and divisor are set to ones.  We invert the mask
	   from the divisor and anding the two together gives a block of one
	   bits equal in size to the result.  We can then use a population count
	   algorithm to count the number of one bits.

	   As we don't have any good ways to work with MSbits we reverse things
	   and work with LSbits instead.

	   Start with the dividend.  */
	"	bfrvrs		d7, d3, #0	\n\t"
	"	sub.4		d8, #0, d7	\n\t"
	"	and.4		d8, d8, d7	\n\t"
	"	add.4		d8, #-1, d8	\n\t"
	"	bfrvrs		d7, d1, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	add.4		d9, #-1, d9	\n\t"

	/* Invert the divisor mask and create the result mask.  */
	"	not.4		d8, d8		\n\t"
	"	and.4		d2, d8, d9	\n\t"

	/* Use a SWAR algorithm to do the population (ones) count.  */
	"	lsr.4		d7, d2, #1	\n\t"
	"	movei		d9, #0x5555	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d7, d7, d9	\n\t"
	"	sub.4		d2, d2, d7	\n\t"
	"	lsr.4		d7, d2, #2	\n\t"
	"	movei		d9, #0x3333	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d7, d7, d9	\n\t"
	"	and.4		d8, d2, d9	\n\t"
	"	add.4		d2, d7, d8	\n\t"
	"	lsr.4		d7, d2, #4	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	movei		d9, #0x0f0f	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d2, d2, d9	\n\t"
	"	lsr.4		d7, d2, #8	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	lsr.4		d7, d2, #16	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	and.4		d2, #0x3f, d2	\n\t"
#else
	/* We need to find how many bits the result will contain.  We do this
	   by finding the MSBits of both the dividend and divisor and
	   subtracting one from the other to form a bit shift.

	   As we don't have an instruction to compute the leading zeroes we
	   reverse the two words and compute trailing zeroes.  We don't have
	   an instruction for that as such either but we can use the FPU!

	   Start with the dividend.  */
	"	bfrvrs		d7, d3, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	move.4		d8, #0		\n\t"
	"	fl2s		acc0_lo, d8	\n\t"
	"	bfrvrs		d7, d1, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	fl2s		acc1_lo, d8	\n\t"
	"	nop				\n\t"
	"	lsr.4		d8, acc0_lo, #23\n\t"
	"	add.4		d8, #-127, d8	\n\t"
	"	nop				\n\t"
	"	nop				\n\t"
	"	lsr.4		d9, acc1_lo, #23\n\t"
	"	add.4		d9, #-127, d9	\n\t"
	"	sub.4		d2, d9, d8	\n\t"
#endif

	/* We're going to unroll by a factor of 8 but we need to jump in
	   part way through on the first iteration.  */
	"	and.4		d9, #7, d2	\n\t"
	"	sub.4		d9, #7, d9	\n\t"
	"	lsl.4		d8, d9, #2	\n\t"
	"	add.4		d9, d9, d8	\n\t"

	/* Align divisor to the upper-most bit of the result.  */
	"	lsl.4		d1, d1, d2	\n\t"

	/* Jump into our partially unrolled long division body.  */
	"	call		a4, 1f		\n\t"

	"1:	lea.4		a4, (a4, d9)	\n\t"
	"	calli		a4, 8(a4)	\n\t"

	/* Now do a long division.  */
	"3:	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d3, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d3, d3, d7	\n\t"
	"	addc		d0, d0, d0	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	/* Close the loop - do any additional blocks of 8 bits, but we're predicting
	   that we rarely take any additional trips.  Winning for small results and
	   doesn't lose much for large ones.  */
	"	add.4		d2, #-8, d2	\n\t"
	"	jmppl.w.f	3b		\n\t"

	/* If necessary negate the result.  */
	"	xor.4		d5, d5, d6	\n\t"
	"	xor.4		d4, d0, d5	\n\t"
	"	sub.4		d0, d4, d5	\n\t"

	"8:	calli		a5, 0(a5)	\n\t"

	/* Special case exit path for the result being +/-1.  We have a fixed result
	   of +1 from the unsigned inner body of the divide but we may need to negate
	   that.  */
	"9:	xor.4		d5, d5, d6	\n\t"
	"	xor.4		d4, #1, d5	\n\t"
	"	sub.4		d0, d4, d5	\n\t"
	"	calli		a5, 0(a5)	\n\t"
		:
		:
#ifndef __UBICOM32_V5__
		: "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
		  "d8", "d9", "a4", "source3"
#else
		: "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
		  "d8", "d9", "a4", "source3", "acc0_lo", "acc1_lo"
#endif
	);
}

long
__modsi3 (long a, long b)
{
	asm volatile (
	/* Set D5 equal to -1 if we have a negative dividend (a), and set D0
	   equal to the absolute value of the dividend.  */
	"	asr.4		d5, d0, #31	\n\t"
	"	xor.4		d4, d5, d0	\n\t"
	"	sub.4		d0, d4, d5	\n\t"

	/* Set D6 equal to -1 if we have a negative divisor (b), and set D1
	   equal to the absolute value of the divisor.  */
	"	asr.4		d6, d1, #31	\n\t"
	"	xor.4		d4, d6, d1	\n\t"
	"	sub.4		d1, d4, d6	\n\t"

	/* Check if the divisor is zero.  If it is then quit.  We should
	   probably abort or something.  */
	"	lsl.4		d3, d1, #0	\n\t"
	"	jmpeq.w.f	9f		\n\t"

	/* Is the divide result zero?  We need to check this now because
	   the next part of the algorithm relies on there being a non-zero
	   result.  */
	"	sub.4		#0, d0, d1	\n\t"
	"	jmplo.w.f	8f		\n\t"

#ifndef __UBICOM32_V5__
	/* We need to find how many bits the result will contain.  We do this
	   by finding a bitmask where all trailing bits after the first set bit
	   of both the dividend and divisor are set to ones.  We invert the mask
	   from the divisor and anding the two together gives a block of one
	   bits equal in size to the result.  We can then use a population count
	   algorithm to count the number of one bits.

	   As we don't have any good ways to work with MSbits we reverse things
	   and work with LSbits instead.

	   Start with the dividend.  */
	"	bfrvrs		d7, d0, #0	\n\t"
	"	sub.4		d8, #0, d7	\n\t"
	"	and.4		d8, d8, d7	\n\t"
	"	add.4		d8, #-1, d8	\n\t"
	"	bfrvrs		d7, d1, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	add.4		d9, #-1, d9	\n\t"

	/* Invert the divisor mask and create the result mask.  */
	"	not.4		d8, d8		\n\t"
	"	and.4		d2, d8, d9	\n\t"

	/* Use a SWAR algorithm to do the population (ones) count.  */
	"	lsr.4		d7, d2, #1	\n\t"
	"	movei		d9, #0x5555	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d7, d7, d9	\n\t"
	"	sub.4		d2, d2, d7	\n\t"
	"	lsr.4		d7, d2, #2	\n\t"
	"	movei		d9, #0x3333	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d7, d7, d9	\n\t"
	"	and.4		d8, d2, d9	\n\t"
	"	add.4		d2, d7, d8	\n\t"
	"	lsr.4		d7, d2, #4	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	movei		d9, #0x0f0f	\n\t"
	"	shmrg.2		d9, d9, d9	\n\t"
	"	and.4		d2, d2, d9	\n\t"
	"	lsr.4		d7, d2, #8	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	lsr.4		d7, d2, #16	\n\t"
	"	add.4		d2, d2, d7	\n\t"
	"	and.4		d2, #0x3f, d2	\n\t"
#else
	/* We need to find how many bits the result will contain.  We do this
	   by finding the MSBits of both the dividend and divisor and
	   subtracting one from the other to form a bit shift.

	   As we don't have an instruction to compute the leading zeroes we
	   reverse the two words and compute trailing zeroes.  We don't have
	   an instruction for that as such either but we can use the FPU!

	   Start with the dividend.  */
	"	bfrvrs		d7, d0, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	move.4		d8, #0		\n\t"
	"	fl2s		acc0_lo, d8	\n\t"
	"	bfrvrs		d7, d1, #0	\n\t"
	"	sub.4		d9, #0, d7	\n\t"
	"	and.4		d9, d9, d7	\n\t"
	"	fl2s		acc1_lo, d8	\n\t"
	"	nop				\n\t"
	"	lsr.4		d8, acc0_lo, #23\n\t"
	"	add.4		d8, #-127, d8	\n\t"
	"	nop				\n\t"
	"	nop				\n\t"
	"	lsr.4		d9, acc1_lo, #23\n\t"
	"	add.4		d9, #-127, d9	\n\t"
	"	sub.4		d2, d9, d8	\n\t"
#endif

	/* We're going to unroll by a factor of 8 but we need to jump in
	   part way through on the first iteration.  */
	"	and.4		d9, #7, d2	\n\t"
	"	sub.4		d9, #7, d9	\n\t"
	"	lsl.4		d9, d9, #2	\n\t"

	/* Align divisor to the upper-most bit of the result.  */
	"	lsl.4		d1, d1, d2	\n\t"

	/* Jump into our partially unrolled long division body.  */
	"	call		a4, 1f		\n\t"

	"1:	lea.4		a4, (a4, d9)	\n\t"
	"	calli		a4, 8(a4)	\n\t"

	/* Now do a long division.  */
	"3:	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	"	sub.4		d7, d0, d1	\n\t"
	"	subc		source3, d1, d1	\n\t"
	"	merge		d0, d0, d7	\n\t"
	"	lsr.4		d1, d1, #1	\n\t"

	/* Close the loop - do any additional blocks of 8 bits, but we're predicting
	   that we rarely take any additional trips.  Winning for small results and
	   doesn't lose much for large ones.  */
	"	add.4		d2, #-8, d2	\n\t"
	"	jmppl.w.f	3b		\n\t"

	/* If necessary negate the result.  */
	"8:	xor.4		d4, d0, d5	\n\t"
	"	sub.4		d0, d4, d5	\n\t"

	"9:	calli		a5, 0(a5)	\n\t"
		:
		:
#ifndef __UBICOM32_V5__
		: "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
		  "d8", "d9", "a4", "source3"
#else
		: "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
		  "d8", "d9", "a4", "source3", "acc0_lo", "acc1_lo"
#endif
	);
}
