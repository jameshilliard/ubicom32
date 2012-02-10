/*
 * arch/ubicom32/crypto/sha1_ubicom32.c
 *   Ubicom32 implementation of the SHA1 Secure Hash Algorithm.
 *
 * (C) Copyright 2009 - 2010, Ubicom, Inc.
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
 *  Add one for sha1 transfer.
 */
DEFINE_STATIC_STOPWATCH_ARRAY(sha1_watch, 1);
#endif

#define HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION 2
#define HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1 ((1 << 5) | HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION)

struct ubicom32_sha1_ctx {
	u64 count;		/* message length in bytes */
	u32 state[5];
	u8 buf[2 * SHA1_BLOCK_SIZE];
	u32 digest_size, block_size; /* Both sizes are in bytes */
	u32 control_word;
};

/*
 * This macro is attempting wipe out the count, state and at least
 * half of the buffer in ubicom32_sha1_ctx defined above.
 */
#define sha1_wipe_out(sctx) memset((u8_t *)(sctx), 0x00, \
				   (sizeof(u64) + 5*sizeof(u32) + SHA1_BLOCK_SIZE))

/*
 * sha1_init()
 *	Setup initial state for SHA1 comutation.
 */
static int __ocm_text sha1_init(struct shash_desc *desc)
{
	struct ubicom32_sha1_ctx *sctx = shash_desc_ctx(desc);

	sctx->state[0] = SHA1_H0;
	sctx->state[1] = SHA1_H1;
	sctx->state[2] = SHA1_H2;
	sctx->state[3] = SHA1_H3;
	sctx->state[4] = SHA1_H4;
	sctx->count = 0;
	sctx->digest_size = SHA1_DIGEST_SIZE;
	sctx->block_size = SHA1_BLOCK_SIZE;
	sctx->control_word = HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1;

	return 0;
}

/*
 * sha1_init_digest()
 *	Initialize for SHA1 digest computation.
 *
 * This routines sets the SHA1 engine control register for SHA1 computation
 * It will then load the 160 bits of the hash to prime the engine.
 */
static inline void sha1_init_digest(u32 *digest, u32 control)
{
	hw_crypto_set_ctrl(control);
	asm volatile (
	"	; move digests to hash_output regs	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_0)"(%[s_base]), 0x00(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_1)"(%[s_base]), 0x04(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_2)"(%[s_base]), 0x08(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_3)"(%[s_base]), 0x0C(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_4)"(%[s_base]), 0x10(%[dgst])		\n\t"
		:
		: [s_base]"a" (SEC_BASE), [dgst]"a" (digest)
	);
}

/*
 * sha1_extract_digest ()
 *	Pull intermediate digest out of engine.
 *
 * Extract 160 bits of digest from sha1 engine if
 * BUSY_BIT in SHA1_STAT register is zero.
 */
static inline void sha1_extract_digest(u32 *digest)
{
	asm volatile (
	"; wait for engine to finish processing last block	\n\t"
	"	btst	"D(UBI32_SECURITY_SHA1_STAT)"(%[s_base]), #"D(UBI32_SECURITY_SHA1_STAT_BUSY_BIT)"		\n\t"
	"	jmpne.f -4			\n\t"
	"					\n\t"
	"	move.4	0x00(%[dgst]), "D(UBI32_SECURITY_SHA1_HASH_OUT_0)"(%[s_base])		\n\t"
	"	move.4	0x04(%[dgst]), "D(UBI32_SECURITY_SHA1_HASH_OUT_1)"(%[s_base])		\n\t"
	"	move.4	0x08(%[dgst]), "D(UBI32_SECURITY_SHA1_HASH_OUT_2)"(%[s_base])		\n\t"
	"	move.4	0x0c(%[dgst]), "D(UBI32_SECURITY_SHA1_HASH_OUT_3)"(%[s_base])		\n\t"
	"	move.4	0x10(%[dgst]), "D(UBI32_SECURITY_SHA1_HASH_OUT_4)"(%[s_base])		\n\t"
		: 
		: [dgst]"a" (digest) ,[s_base]"a" (SEC_BASE)
		: "cc", "memory"
	);
}

/*
 * sha1_update()
 *	Update SHA1 engine with new data.
 *
 * This routine is called by external callers after the corresponding
 * sha1_init has been invoked by extern callers. This routine
 * can be called multiple times before sha1_final will be called
 * by the external caller to indicate termination of the digest calculation.
 */
static int __ocm_text sha1_update(struct shash_desc *desc, const u8 *data,
			unsigned int len)
{
	struct ubicom32_sha1_ctx *sctx = shash_desc_ctx(desc);
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
	 * Turn the crypto engine on. We are going to use it.
	 */
	flags = hw_crypto_lock();
	hw_crypto_check();

	STOPWATCH_START();

	/* 
	 * Set control and prime the engine by loading the digest
	 * to initialize state of the engine.
	 */
	sha1_init_digest(sctx->state, sctx->control_word);

	/*
	 * Consume the previous left over data by constructing a single
	 * complete block and feeding it to the engine.
	 */
	if (index) {
		clen = sctx->block_size - index;
		memcpy(sctx->buf + index, data, clen);
		sha1_feed_512(sctx->buf);
		data += clen;
		len -= clen;
		index = 0;
	}

	if (likely(SEC_ALIGNED(data))) {
		if (likely(len >= sctx->block_size)) {
		  register unsigned int cnt = len / sctx->block_size;
			while (cnt--) {
				/*
				 * Wait and Load 512 bits into the engine.
				 */
				sha1_wait();
				sha1_feed_512(data);
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
			memcpy(sctx->buf, data, sctx->block_size);

			/*
			 * Wait for space in fifo and Load 512 bits into the engine.
			 */
			sha1_wait();
			sha1_feed_512(sctx->buf);
			data += sctx->block_size;
			len -= sctx->block_size;
		}
	}

	/*
	 * Extract digest and shutdown the crypto engine.
	 */
	sha1_extract_digest(sctx->state);
	STOPWATCH_END(&sha1_watch[0]);
	hw_crypto_unlock(flags);

	/* 
	 * Anything left? Copy it into the buffer. We will process it in the next
	 * go around or during finialization.
	 */
	if (len) {
		memcpy(sctx->buf + index , data, len);
	}

	return 0;
}

/* 
 * sha1_final()
 *	Finalize digest computation.
 *
 * Called by external caller indicating end of digest calculation..
 * Add padding to increase the size of the tail to be a multiple of 512 bits.
 * Add the length to the last 8 bytes of the buffer and pass it to the engine.
 * Once the engine has processed the last block then extract and return the 
 * message digest. 
 */
static int __ocm_text sha1_final(struct shash_desc *desc, u8 *out)
{
	struct ubicom32_sha1_ctx *sctx = shash_desc_ctx(desc);
	u64 bits;
	unsigned int index, end;

	/* 
	 * Must perform manual padding.
	 */
	index = sctx->count & (sctx->block_size - 1);
	end =  (index < (sctx->block_size - sizeof(bits))) ? SHA1_BLOCK_SIZE : (2 * SHA1_BLOCK_SIZE);

	/*
	 * Start pad with 1
	 */
	sctx->buf[index] = 0x80;

	/*
	 * Pad with zeros
	 */
	index++;
	memset(sctx->buf + index, 0x00, end - index - sizeof(bits));

	/*
	 * Append message length
	 */
	bits = sctx->count << 3 ;
	memcpy(sctx->buf + end - sizeof(bits), &bits, sizeof(bits));

	/*
	 * Force to use the sctx->buf and ignore the partial buf
	 */
	sctx->count = sctx->count & ~(sctx->block_size - 1);
	sha1_update(desc, sctx->buf, end);

	/*
	 * Copy digest to out
	 */
	memcpy(out, sctx->state, sctx->digest_size);

	/*
	 * Wipe context
	 */
	sha1_wipe_out(sctx);

	return 0;
}

static int sha1_export(struct shash_desc *desc, void *out)
{
	struct ubicom32_sha1_ctx *sctx = shash_desc_ctx(desc);

	memcpy(out, sctx, sizeof(*sctx));
	return 0;
}

static int sha1_import(struct shash_desc *desc, const void *in)
{
	struct ubicom32_sha1_ctx *sctx = shash_desc_ctx(desc);

	memcpy(sctx, in, sizeof(*sctx));
	return 0;
}

static struct shash_alg alg = {
	.digestsize	=	SHA1_DIGEST_SIZE,
	.init		=	sha1_init,
	.update		=	sha1_update,
	.final		=	sha1_final,
	.export		=	sha1_export,
	.import		=	sha1_import,
	.descsize	=	sizeof(struct ubicom32_sha1_ctx),
	.statesize	=	sizeof(struct ubicom32_sha1_ctx),
	.base		=	{
		.cra_name	=	"sha1",
		.cra_driver_name=	"sha1-ubicom32",
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_priority   =       CRYPTO_UBICOM32_PRIORITY,
		.cra_blocksize	=	SHA1_BLOCK_SIZE,
		.cra_ctxsize	=	sizeof(struct ubicom32_sha1_ctx),
		.cra_module	=	THIS_MODULE,
		.cra_list	=	LIST_HEAD_INIT(alg.base.cra_list),
	}
};

#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
static const char *sha1_names [1] = {
	"sha1-update"
};

/*
 * sha1_watch_show()
 *	Print out the min, avg, max values for each SHA1
 */
static int sha1_watch_show(struct seq_file *p, void *v)
{
	int number = *((loff_t *) v);

	if (number == 0) {
		seq_puts(p, "\tmin\tavg\tmax\t(micro-seconds)\n");
	}

	stopwatch_show(&sha1_watch[number], p, sha1_names[number], STOPWATCH_SHOW_MICRO);
	return 0;
}

static int __init sha1_watch_init(void)
{
	return stopwatch_register("sha1", 1, sha1_watch_show);
}

#endif

/*
 * init()
 *	Module initialization.
 */
static int __init init(void)
{
#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
	sha1_watch_init();
#endif
	hw_crypto_init();
	return crypto_register_shash(&alg);
}

/*
 * fini()
 *	Module teardown.
 */
static void __exit fini(void)
{
	crypto_unregister_shash(&alg);
}

module_init(init);
module_exit(fini);

MODULE_ALIAS("sha1");

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SHA1 Secure Hash Algorithm");
