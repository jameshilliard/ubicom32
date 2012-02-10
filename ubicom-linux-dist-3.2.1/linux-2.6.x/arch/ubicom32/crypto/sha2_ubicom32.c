/*
 * arch/ubicom32/crypto/sha2_ubicom32.c
 *   Ubicom32 implementation of the SHA2 Secure Hash Algorithm.
 *
 * (C) Copyright 2010 Ubicom, Inc.
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
 *   arch/s390
 */
#include <linux/init.h>
#include <linux/module.h>
#include <crypto/internal/hash.h>
#include <crypto/sha.h>
#include <asm/linkage.h>

#include "crypto_ubicom32.h"

#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
#define __STOPWATCH_USE__ 1
#endif
#include <asm/stopwatch.h>
#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
/*
 *  Add one for 1024 transfer, and one for 512 transfer.
 */
DEFINE_STATIC_STOPWATCH_ARRAY(sha2_watches, 2);
#endif

struct ubicom32_sha512_ctx {
	u64 count[2];		/* message length in bytes*/
	u64 state[8];		/* Space for digest */
	u8 buf[2 * SHA512_BLOCK_SIZE];
	u32 digest_size, block_size; /* Both sizes are in bytes */
	u32 control_word;
};

/*
 * This macro is attempting wipe out the count, state and at least
 * half of the buffer in ubicom32_sha512_ctx defined above.
 */
#define sha512_wipe_out(sctx) memset((u8_t *)(sctx), 0x00, \
				     (2*sizeof(u64) + 8*sizeof(u64) + SHA512_BLOCK_SIZE))

struct ubicom32_sha256_ctx {
	u64 count;		/* message length in bytes*/
	u32 state[8];		/* Space for digest */
	u8 buf[2 * SHA256_BLOCK_SIZE];
	u32 digest_size, block_size; /* Both sizes are in bytes */
	u32 control_word;
};

/*
 * This macro is attempting wipe out the count, state and at least
 * half of the buffer in ubicom32_sha256_ctx defined above.
 */
#define sha256_wipe_out(sctx) memset((u8_t *)(sctx), 0x00, \
				     (sizeof(u64) + 8*sizeof(u32) + SHA256_BLOCK_SIZE))

/*
 * sha2_init_digest_512()
 *	Initialize for SHA512/SHA384
 *
 * This routines sets the SHA2 control register for SHA-512 or SHA-384.
 * It will then load the 512 bits of the hash to prime the engine.
 */
static inline void sha2_init_digest_512(u64 *digest, u32 control)
{
	/*
	 * Load control word into engine.
	 */
	hw_crypto_set_sha2_ctrl(control);

	/*
	 * Load digest into engine.
	 */
	asm volatile (
	"	; move digests to hash_output regs	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_HI_0)"(%[s_base]), 0x00(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_HI_1)"(%[s_base]), 0x04(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_HI_2)"(%[s_base]), 0x08(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_HI_3)"(%[s_base]), 0x0c(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_HI_4)"(%[s_base]), 0x10(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_HI_5)"(%[s_base]), 0x14(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_HI_6)"(%[s_base]), 0x18(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_HI_7)"(%[s_base]), 0x1c(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_0)"(%[s_base]), 0x20(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_1)"(%[s_base]), 0x24(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_2)"(%[s_base]), 0x28(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_3)"(%[s_base]), 0x2c(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_4)"(%[s_base]), 0x30(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_5)"(%[s_base]), 0x34(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_6)"(%[s_base]), 0x38(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_7)"(%[s_base]), 0x3c(%[dgst])		\n\t"
		:
		: [s_base]"a" (SEC_BASE), [dgst]"a" (digest)
	);
}

/*
 * sha2_init_digest_256()
 *	Initialize for SHA256/SHA224
 *
 * This routines sets the SHA2 control register for SHA-256 or SHA-224.
 * It will then load the 256 bits of the hash to prime the engine.
 */
static inline void sha2_init_digest_256(u32 *digest, u32 control)
{
	/*
	 * Load control word into engine.
	 */
	hw_crypto_set_sha2_ctrl(control);

	/*
	 * Load digest into engine.
	 */
	asm volatile (
	"	; move digests to hash_output regs	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_0)"(%[s_base]), 0x00(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_1)"(%[s_base]), 0x04(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_2)"(%[s_base]), 0x08(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_3)"(%[s_base]), 0x0c(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_4)"(%[s_base]), 0x10(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_5)"(%[s_base]), 0x14(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_6)"(%[s_base]), 0x18(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_HASH_LO_7)"(%[s_base]), 0x1c(%[dgst])		\n\t"
		:
		: [s_base]"a" (SEC_BASE), [dgst]"a" (digest)
	);
}

/*
 * sha2_wait_for_space()
 *	Loop waiting for INQUEUE_FULL_BIT in SHA2_STAT register to go to 0.
 *	Temporary workaround a suspected hardware problem is to wait till the SHA2_STAT_BUSY_BIT
 *	goes to zero.
 */
static inline void sha2_wait_for_space(void)
{
	asm volatile (
	"	; Check for space in the queue  \n\t"
	"	; Wait until there is space     \n\t"
	"					\n\t"
	"	;btst	"D(UBI32_SECURITY_SHA2_STAT)"(%[s_base]), #"D(UBI32_SECURITY_SHA2_STAT_INQUEUE_FULL_BIT)"		\n\t"
	"	;jmpne.f	-4			\n\t"
	"					\n\t"
	"	btst	"D(UBI32_SECURITY_SHA2_STAT)"(%[s_base]), #"D(UBI32_SECURITY_SHA2_STAT_BUSY_BIT)"		\n\t"
	"	jmpne.f -4			\n\t"
	"					\n\t"
		:
		: [s_base]"a"(SEC_BASE)
		: "cc"
	);
}

/*
 * sha2_feed_1024()
 *	Loads SHA2 engine input queue.
 *
 * This routine feeds 1024 bits of data to the SHA2 input queue.
 * This routine is used during computation of SHA-512 and SHA-384 digests.
 */
static inline void sha2_feed_1024(const u8 *in)
{
	asm volatile (
	"	; write the 1st 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x00(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x04(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x08(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x0c(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 2nd 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x10(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x14(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x18(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x1c(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 3rd 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x20(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x24(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x28(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x2c(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 4th 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x30(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x34(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x38(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x3c(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 5th 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x40(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x44(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x48(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x4c(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 6th 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x50(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x54(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x58(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x5c(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 7th 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x60(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x64(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x68(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x6c(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 8th 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x70(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x74(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x78(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x7c(%[input])	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%0), #0		\n\t"
		:
		: [s_base]"a" (SEC_BASE), [input]"a" (in)
		: "cc"
	);
}

/*
 * sha2_feed_512()
 *	Loads SHA2 engine input queue.
 *
 * This routine feeds 512 bits of data to the SHA2 input queue.
 * This routine is used during computation of SHA-256 and SHA-224 digests.
 */
static inline void sha2_feed_512(const u8 *in)
{
	asm volatile (
	"	; write the 1st 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x00(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x04(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x08(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x0c(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 2nd 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x10(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x14(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x18(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x1c(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 3rd 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x20(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x24(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x28(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x2c(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
	"	; write the 4th 16 bytes	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_0)"(%[s_base]), 0x30(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_1)"(%[s_base]), 0x34(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_2)"(%[s_base]), 0x38(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_3)"(%[s_base]), 0x3c(%1)	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA2_IN_4)"(%[s_base]), #0		\n\t"
		:
		: [s_base]"a" (SEC_BASE), [input]"a" (in)
		: "cc"
	);
}

/*
 * sha2_extract_digest_512 ()
 *	Pull intermediate digest out of engine.
 *
 * Extract 512 bits of digest from sha2 engine if
 * BUSY_BIT in SHA2_STAT register is zero.
 */
static inline void sha2_extract_digest_512(u64 *digest)
{
	asm volatile (
	"; wait for engine to finish processing last block	\n\t"
	"	btst	"D(UBI32_SECURITY_SHA2_STAT)"(%[s_base]), #"D(UBI32_SECURITY_SHA2_STAT_BUSY_BIT)"		\n\t"
	"	jmpne.f -4			\n\t"
	"					\n\t"
	"	move.4	0x00(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_HI_0)"(%[s_base])		\n\t"
	"	move.4	0x04(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_HI_1)"(%[s_base])		\n\t"
	"	move.4	0x08(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_HI_2)"(%[s_base])		\n\t"
	"	move.4	0x0c(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_HI_3)"(%[s_base])		\n\t"
	"	move.4	0x10(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_HI_4)"(%[s_base])		\n\t"
	"	move.4	0x14(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_HI_5)"(%[s_base])		\n\t"
	"	move.4	0x18(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_HI_6)"(%[s_base])		\n\t"
	"	move.4	0x1c(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_HI_7)"(%[s_base])		\n\t"
	"	move.4	0x20(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_0)"(%[s_base])		\n\t"
	"	move.4	0x24(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_1)"(%[s_base])		\n\t"
	"	move.4	0x28(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_2)"(%[s_base])		\n\t"
	"	move.4	0x2c(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_3)"(%[s_base])		\n\t"
	"	move.4	0x30(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_4)"(%[s_base])		\n\t"
	"	move.4	0x34(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_5)"(%[s_base])		\n\t"
	"	move.4	0x38(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_6)"(%[s_base])		\n\t"
	"	move.4	0x3c(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_7)"(%[s_base])		\n\t"
		: 
		: [dgst]"a" (digest) ,[s_base]"a" (SEC_BASE)
		: "cc", "memory"
	);
}

/*
 * sha2_extract_digest_256 ()
 *	Pull intermediate digest out of engine.
 *
 * Extract 256 bits of digest from sha2 engine if
 * BUSY_BIT in SHA2_STAT register is zero.
 */
static inline void sha2_extract_digest_256(u32 *digest)
{
	asm volatile (
	"; wait for engine to finish processing last block	\n\t"
	"	btst	"D(UBI32_SECURITY_SHA2_STAT)"(%[s_base]), #"D(UBI32_SECURITY_SHA2_STAT_BUSY_BIT)"		\n\t"
	"	jmpne.f -4			\n\t"
	"					\n\t"
	"	move.4	0x00(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_0)"(%[s_base])		\n\t"
	"	move.4	0x04(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_1)"(%[s_base])		\n\t"
	"	move.4	0x08(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_2)"(%[s_base])		\n\t"
	"	move.4	0x0c(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_3)"(%[s_base])		\n\t"
	"	move.4	0x10(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_4)"(%[s_base])		\n\t"
	"	move.4	0x14(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_5)"(%[s_base])		\n\t"
	"	move.4	0x18(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_6)"(%[s_base])		\n\t"
	"	move.4	0x1c(%[dgst]), "D(UBI32_SECURITY_SHA2_HASH_LO_7)"(%[s_base])		\n\t"
		: 
		: [dgst]"a" (digest) ,[s_base]"a" (SEC_BASE)
		: "cc", "memory"
	);
}

/*
 * sha2_init_512()
 *	Setup initial state for SHA-512.
 */
static int __ocm_text sha2_init_512(struct shash_desc *desc)
{
	struct ubicom32_sha512_ctx *ctx = shash_desc_ctx(desc);

	ctx->state[0] = SHA512_H0;
	ctx->state[1] = SHA512_H1;
	ctx->state[2] = SHA512_H2;
	ctx->state[3] = SHA512_H3;
	ctx->state[4] = SHA512_H4;
	ctx->state[5] = SHA512_H5;
	ctx->state[6] = SHA512_H6;
	ctx->state[7] = SHA512_H7;
	ctx->count[0] = ctx->count[1] = 0;
	ctx->digest_size = SHA512_DIGEST_SIZE;
	ctx->block_size = SHA512_BLOCK_SIZE;
	ctx->control_word = UBI32_SECURITY_SHA2_CTRL_SHA_512;

	return 0;
}

/*
 * sha2_init_384()
 *	Setup initial state for SHA-384.
 */
static int __ocm_text sha2_init_384(struct shash_desc *desc)
{
	struct ubicom32_sha512_ctx *ctx = shash_desc_ctx(desc);

	ctx->state[0] = SHA384_H0;
	ctx->state[1] = SHA384_H1;
	ctx->state[2] = SHA384_H2;
	ctx->state[3] = SHA384_H3;
	ctx->state[4] = SHA384_H4;
	ctx->state[5] = SHA384_H5;
	ctx->state[6] = SHA384_H6;
	ctx->state[7] = SHA384_H7;
	ctx->count[0] = ctx->count[1] = 0;
	ctx->digest_size = SHA384_DIGEST_SIZE;
	ctx->block_size = SHA384_BLOCK_SIZE;
	ctx->control_word = UBI32_SECURITY_SHA2_CTRL_SHA_384;

	return 0;
}

/*
 * sha2_init_256()
 *	Setup initial state for SHA-256.
 */
static int __ocm_text sha2_init_256(struct shash_desc *desc)
{
	struct ubicom32_sha256_ctx *sctx = shash_desc_ctx(desc);

	sctx->state[0] = SHA256_H0;
	sctx->state[1] = SHA256_H1;
	sctx->state[2] = SHA256_H2;
	sctx->state[3] = SHA256_H3;
	sctx->state[4] = SHA256_H4;
	sctx->state[5] = SHA256_H5;
	sctx->state[6] = SHA256_H6;
	sctx->state[7] = SHA256_H7;
	sctx->count = 0;
	sctx->digest_size = SHA256_DIGEST_SIZE;
	sctx->block_size = SHA256_BLOCK_SIZE;
	sctx->control_word = UBI32_SECURITY_SHA2_CTRL_SHA_256;

	return 0;
}

/*
 * sha2_init_224()
 *	Setup initial state for SHA-224.
 */
static int __ocm_text sha2_init_224(struct shash_desc *desc)
{
	struct ubicom32_sha256_ctx *sctx = shash_desc_ctx(desc);

	sctx->state[0] = SHA224_H0;
	sctx->state[1] = SHA224_H1;
	sctx->state[2] = SHA224_H2;
	sctx->state[3] = SHA224_H3;
	sctx->state[4] = SHA224_H4;
	sctx->state[5] = SHA224_H5;
	sctx->state[6] = SHA224_H6;
	sctx->state[7] = SHA224_H7;
	sctx->count = 0;
	sctx->digest_size = SHA224_DIGEST_SIZE;
	sctx->block_size = SHA224_BLOCK_SIZE;
	sctx->control_word = UBI32_SECURITY_SHA2_CTRL_SHA_224;

	return 0;
}

/*
 * sha2_update_1024()
 *	Update SHA2 engine with new data.
 *
 * This routine is called by external callers after the corresponding
 * sha2_init_512 or sha2_init_384 has been invoked by extern callers. This routine
 * can be called multiple times before sha2_final_1024 will be called
 * by the external caller to indicate termination of the digest calculation.
 */
static int __ocm_text sha2_update_1024(struct shash_desc *desc, const u8 *data,
			unsigned int len)
{
	struct ubicom32_sha512_ctx *sctx = shash_desc_ctx(desc);
	int index, clen;
	unsigned long flags;

	STOPWATCH_DECLARE;

	/*
	 * The data is always processed in sctx->block_size. Any left over 
	 * data is stored in the sctx and processed during the next call to 
	 * update or final.
	 *
	 * How much is already in the buffer? 
	 */
	index = sctx->count[1] & (sctx->block_size - 1);

	sctx->count[1] += len;
	if (sctx->count[1] < (u64) len) {
		/*
		 * Counter rolled over.
		 */
		sctx->count[0]++;
	}

	if (index + len < sctx->block_size) {
		/*
		 * Insufficient data. Store what was sent in sctx->buf and exit.
		 */
		if (len) {
			memcpy(sctx->buf + index , data, len);
		}
		return 0;
	}

	/*
	 * Turn on the crypto engine. We are going to use it.
	 */
	flags = hw_crypto_lock();
	hw_crypto_check();

	STOPWATCH_START();
 
	/* 
	 * Set sha2 control and prime the engine by loading 512 bits (64 words) 
	 * to initialize state of the engine.
	 */
	sha2_init_digest_512(sctx->state, sctx->control_word);

	/*
	 * Consume the previous left over data by constructing a single
	 * complete block and feeding it to the engine.
	 */
	if (index) {
		clen = sctx->block_size - index;
		memcpy(sctx->buf + index, data, clen);
		sha2_feed_1024(sctx->buf);
		data += clen;
		len -= clen;
		index = 0;
	}

	if (likely(SEC_ALIGNED(data))) {
		if (likely(len >= sctx->block_size)) {
			register unsigned int cnt = len / sctx->block_size;
			
			while (cnt--) {
				/*
				 * Wait for space in fifo and Load 1024 bits into the engine.
				 */
				sha2_wait_for_space();
				sha2_feed_1024(data);
				data += sctx->block_size;
			}

			/*
			 * All the full size blocks have been sent.
			 * reduce length to length % block_size.
			 */
			len = len & (sctx->block_size - 1);
		}
	} else {
		/* 
		 * Misaligned data, process as many blocks as possible.
		 */
		while (likely(len >= sctx->block_size)) {
			/*
			 * Copy data to an aligned buffer.
			 */
			memcpy(sctx->buf, data, sctx->block_size);

			/*
			 * Wait for space in fifo and Load 1024 bits into the engine.
			 */
			sha2_wait_for_space();
			sha2_feed_1024(sctx->buf);
			data += sctx->block_size;
			len -= sctx->block_size;
		}
	}

	/*
	 * Extract digest and shutdown the crypto engine.
	 */
	sha2_extract_digest_512(sctx->state);
	STOPWATCH_END(&sha2_watches[0]);
	hw_crypto_unlock(flags);

	/* 
	 * Anything left? Copy it into the buffer. We will process it in the next
	 * go around of during finialization.
	 */
	if (len) {
		memcpy(sctx->buf + index , data, len);
	}

	return 0;
}


/* 
 * sha2_final_1024()
 *	Finalize digest computation.
 *
 * Called by external caller indicating end of digest calculation.
 * Add padding to increase the size of the tail to be a multiple of 1024 bits.
 * Add the length to the last 16 bytes of the buffer and pass it to the engine.
 * Once the engine has processed the last block then extract and return the message digest. 
 */
static int __ocm_text sha2_final_1024(struct shash_desc *desc, u8 *out)
{
	struct ubicom32_sha512_ctx *sctx = shash_desc_ctx(desc);
	u64 bits[2];
	unsigned int index, end;

	/* 
	 * must perform manual padding 
	 */
	index = sctx->count[1] & (sctx->block_size - 1);
	end =  (index < (sctx->block_size - sizeof(bits))) ? sctx->block_size : (2 * sctx->block_size);

	/* 
	 * start pad with 1 
	 */
	sctx->buf[index] = 0x80;

	/* 
	 * pad with zeros 
	 */
	index++;
	memset(sctx->buf + index, 0x00, end - index - sizeof(bits));

	/* 
	 * append message length 
	 */
	bits[0] = sctx->count[0] << 3;
	bits[0] |= sctx->count[1] >> (64-3);
	bits[1] = sctx->count[1] << 3 ;
	memcpy(sctx->buf + end - sizeof(bits), (void *)&bits, sizeof(bits));

	/* 
	 * force to use the sctx->buf and ignore the partial buf 
	 */
	sctx->count[1] = sctx->count[1] & ~(((u64)sctx->block_size) - 1);
	sha2_update_1024(desc, sctx->buf, end);

	/* 
	 *copy digest to out 
	 */
	memcpy(out, sctx->state, sctx->digest_size);

	/* 
	 * wipe context 
	 */
	sha512_wipe_out(sctx);

	return 0;
}

/*
 * sha2_update_512()
 *	Update SHA2 engine with new data.
 *
 * This routine is called by external callers after the corresponding
 * sha2_init_256/sha_init_224 has been invoked by extern callers. This routine
 * can be called multiple times before sha2_final_512 will be called
 * by the external caller to indicate termination of the digest calculation.
 */
static int __ocm_text sha2_update_512(struct shash_desc *desc, const u8 *data,
			unsigned int len)
{
	struct ubicom32_sha256_ctx *sctx = shash_desc_ctx(desc);
	int index, clen;
	unsigned long flags;

	STOPWATCH_DECLARE;

	/*
	 * The data is always processed in sctx->block_size. Any left over data is stored
	 * in the sctx and processed during the next call to update or final.
	 *
	 * how much is already in the buffer? 
	 */
	index = sctx->count & (sctx->block_size - 1);

	sctx->count += len;

	if (index + len < sctx->block_size) {
		/*
		 * Insufficient data. Store what was sent in sctx->buf and exit.
		 */
		if (len) {
			memcpy(sctx->buf + index , data, len);
		}
		return 0;
	}

	/*
	 * Turn on the crypto engine. We are going to use it.
	 */
	flags = hw_crypto_lock();
	hw_crypto_check();

	STOPWATCH_START();

	/* 
	 * Set control and prime the engine by loading 256 bits (32 words) 
	 * to initialize state of the engine.
	 */
	sha2_init_digest_256(sctx->state, sctx->control_word);

	/*
	 * Consume the previous left over data by constructing a single
	 * complete block and feeding it to the engine.
	 */
	if (index) {
		clen = sctx->block_size - index;
		memcpy(sctx->buf + index, data, clen);
		sha2_feed_512(sctx->buf);
		data += clen;
		len -= clen;
		index = 0;
	}

	if (likely(SEC_ALIGNED(data))) {
		if (likely(len >= sctx->block_size)) {
			register unsigned int cnt = len / sctx->block_size;
			
			while (cnt--) {
				/*
				 * Wait for space and Load  512 bits into the engine.
				 */
				sha2_wait_for_space();
				sha2_feed_512(data);
				data += sctx->block_size;
			}

			/*
			 * All the full size blocks have been sent.
			 * reduce length to length % block_size.
			 */
			len = len & (sctx->block_size - 1);
		}
	} else {
		/* 
		 * Misaligned data, process as many blocks as possible.
		 */
		while (likely(len >= sctx->block_size)) {
			/*
			 * Move data to an aligned buffer.
			 */
			memcpy(sctx->buf, data, sctx->block_size);

			/*
			 * Wait for space and Load  512 bits into the engine.
			 */
			sha2_wait_for_space();
			sha2_feed_512(sctx->buf);
			data += sctx->block_size;
			len -= sctx->block_size;
		}
	}

	/*
	 * Extract digest and shutdown the crypto engine.
	 */
	sha2_extract_digest_256(sctx->state);
	STOPWATCH_END(&sha2_watches[1]);
	hw_crypto_unlock(flags);

	/* 
	 * Anything left? Copy it into the buffer. We will process it in the next
	 * go around of during finialization.
	 */
	if (len) {
		memcpy(sctx->buf + index , data, len);
	}

	return 0;
}

/* 
 * sha2_final_512()
 *	Finalize digest computation.
 *
 * Called by external caller indicating end of digest calculation..
 * Add padding to increase the size of the tail to be a multiple of 512 bits.
 * Add the length to the last 8 bytes of the buffer and pass it to the engine.
 * Once the engine has processed the last block then extract and return the 
 * message digest. 
 */
static int __ocm_text sha2_final_512(struct shash_desc *desc, u8 *out)
{
	struct ubicom32_sha256_ctx *sctx = shash_desc_ctx(desc);
	u64 bits;
	unsigned int index, end;

	/*
	 * must perform manual padding 
	 */
	index = sctx->count & (sctx->block_size - 1);
	end =  (index < (sctx->block_size - sizeof(bits))) ? sctx->block_size : (2 * sctx->block_size);

	/* 
	 * start pad with 1
	 */
	sctx->buf[index] = 0x80;

	/*
	 * pad with zeros 
	 */
	index++;
	memset(sctx->buf + index, 0x00, end - index - sizeof(bits));

	/*
	 * append message length
	 */
	bits = sctx->count << 3;
	memcpy(sctx->buf + end - sizeof(bits), (void *)&bits, sizeof(bits));

	/*
	 * force to use the sctx->buf and ignore the partial buf 
	 */
	sctx->count = sctx->count & ~(sctx->block_size - 1);
	sha2_update_512(desc, sctx->buf, end);

	/*
	 * copy digest to out
	 */
	memcpy(out, sctx->state, sctx->digest_size);

	/*
	 * wipe context 
	 */
	sha256_wipe_out(sctx);

	return 0;
}

static struct shash_alg alg_512 = {
	.digestsize	=       SHA512_DIGEST_SIZE,
	.init		=       sha2_init_512,
	.update		=       sha2_update_1024,
	.final		=       sha2_final_1024,
	.descsize	=	sizeof(struct ubicom32_sha512_ctx),
	.base		=	{
		.cra_name	=	"sha512",
		.cra_driver_name=	"sha512-ubicom32",
		.cra_priority	=	CRYPTO_UBICOM32_PRIORITY,
		.cra_flags	=	CRYPTO_ALG_TYPE_DIGEST,
		.cra_blocksize	=	SHA512_BLOCK_SIZE,
		.cra_ctxsize	=	sizeof(struct ubicom32_sha512_ctx),
		.cra_module	=	THIS_MODULE,
		.cra_list	=	LIST_HEAD_INIT(alg_512.base.cra_list),
	}
};

static struct shash_alg alg_384 = {
	.digestsize	=       SHA384_DIGEST_SIZE,
	.init		=       sha2_init_384,
	.update		=       sha2_update_1024,
	.final		=       sha2_final_1024,
	.descsize	=	sizeof(struct ubicom32_sha512_ctx),
	.base		=	{
		.cra_name	=	"sha384",
		.cra_driver_name=	"sha384-ubicom32",
		.cra_priority	=	CRYPTO_UBICOM32_PRIORITY,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA384_BLOCK_SIZE,
		.cra_ctxsize	=	sizeof(struct ubicom32_sha512_ctx),
		.cra_module	=	THIS_MODULE,
		.cra_list	=	LIST_HEAD_INIT(alg_384.base.cra_list),
	}
};

static struct shash_alg alg_256 = {
	.digestsize	=       SHA256_DIGEST_SIZE,
	.init		=       sha2_init_256,
	.update		=       sha2_update_512,
	.final		=       sha2_final_512,
	.descsize	=	sizeof(struct ubicom32_sha512_ctx),
	.base		=	{
		.cra_name	=	"sha256",
		.cra_driver_name=	"sha256-ubicom32",
		.cra_priority	=	CRYPTO_UBICOM32_PRIORITY,
		.cra_flags	=	CRYPTO_ALG_TYPE_DIGEST,
		.cra_blocksize	=	SHA256_BLOCK_SIZE,
		.cra_ctxsize	=	sizeof(struct ubicom32_sha256_ctx),
		.cra_module	=	THIS_MODULE,
		.cra_list	=	LIST_HEAD_INIT(alg_256.base.cra_list),
	}
};

static struct shash_alg alg_224 = {
	.digestsize	=       SHA224_DIGEST_SIZE,
	.init		=       sha2_init_224,
	.update		=       sha2_update_512,
	.final		=       sha2_final_512,
	.descsize	=	sizeof(struct ubicom32_sha512_ctx),
	.base		=	{
		.cra_name	=	"sha224",
		.cra_driver_name=	"sha224-ubicom32",
		.cra_priority	=	CRYPTO_UBICOM32_PRIORITY,
		.cra_flags	=	CRYPTO_ALG_TYPE_DIGEST,
		.cra_blocksize	=	SHA224_BLOCK_SIZE,
		.cra_ctxsize	=	sizeof(struct ubicom32_sha256_ctx),
		.cra_module	=	THIS_MODULE,
		.cra_list	=	LIST_HEAD_INIT(alg_224.base.cra_list),
	}
};

#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
static const char *sha2_names [2] = {
	"sha2-1024",
	"sha2-512"
};

/*
 * sha2_watch_show()
 *	Print out the min, avg, max values for each SHA2
 */
static int sha2_watch_show(struct seq_file *p, void *v)
{
	int number = *((loff_t *) v);

	if (number == 0) {
		seq_puts(p, "\tmin\tavg\tmax\t(micro-seconds)\n");
	}

	stopwatch_show(&sha2_watches[number], p, sha2_names[number], STOPWATCH_SHOW_MICRO);
	return 0;
}

static int __init sha2_watch_init(void)
{
	return stopwatch_register("sha2", 2, sha2_watch_show);
}

#endif

/*
 * init()
 *	Module initialization.
 */
static int __init init(void)
{
#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
	sha2_watch_init();
#endif
	hw_crypto_init();
 	crypto_register_shash(&alg_224);
 	crypto_register_shash(&alg_256);
 	crypto_register_shash(&alg_384);
	return crypto_register_shash(&alg_512);
}

/*
 * fini()
 *	Module teardown.
 */
static void __exit fini(void)
{
	crypto_unregister_shash(&alg_224);
	crypto_unregister_shash(&alg_256);
	crypto_unregister_shash(&alg_384);
	crypto_unregister_shash(&alg_512);
}

module_init(init);
module_exit(fini);

MODULE_ALIAS("sha2");

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SHA2 Secure Hash Algorithm");
