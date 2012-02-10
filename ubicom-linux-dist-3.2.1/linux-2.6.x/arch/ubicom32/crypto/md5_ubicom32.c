/*
 * arch/ubicom32/crypto/md5_ubicom32.c
 *   Ubicom32 implementation of the MD5 Secure Hash Algorithm
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
 *   arch/s390
 */
#include <linux/init.h>
#include <linux/module.h>
#include <crypto/internal/hash.h>

#include "crypto_ubicom32.h"

#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
#define __STOPWATCH_USE__ 1
#endif
#include <asm/stopwatch.h>
#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
/*
 *  Add one for md5 transfer.
 */
DEFINE_STATIC_STOPWATCH_ARRAY(md5_watch, 1);
#endif

#define MD5_DIGEST_SIZE	16
#define MD5_BLOCK_SIZE	64
#define MD5_HASH_WORDS	4

#define HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION 2
#define HASH_SECURITY_BLOCK_CONTROL_INIT_MD5 ((1 << 4) | HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION)

struct ubicom32_md5_ctx {
	u64 count;		/* message length */
	u32 state[MD5_HASH_WORDS];
	u8 buf[2 * MD5_BLOCK_SIZE];
	u32 digest_size, block_size; /* Both sizes are in bytes */
	u32 control_word;
};

/*
 * This macro is attempting wipe out the count, state and at least
 * half of the buffer in ubicom32_md5_ctx defined above.
 */
#define md5_wipe_out(sctx) memset((u8_t *)(sctx), 0x00,			\
				  (sizeof(u64) + MD5_HASH_WORDS*sizeof(u32) + MD5_BLOCK_SIZE))

/*
 * md5_init()
 *	Setup initial state for MD5 comutation.
 */
static int md5_init(struct shash_desc *desc)
{
	struct ubicom32_md5_ctx *mctx = shash_desc_ctx(desc);
	mctx->state[0] = 0x01234567;
	mctx->state[1] = 0x89abcdef;
	mctx->state[2] = 0xfedcba98;
	mctx->state[3] = 0x76543210;

	mctx->count = 0;
	mctx->digest_size = MD5_DIGEST_SIZE;
	mctx->block_size = MD5_BLOCK_SIZE;
	mctx->control_word = HASH_SECURITY_BLOCK_CONTROL_INIT_MD5;

	return 0;
}

/*
 * md5_init_digest()
 *	Initialize for MD5 digest computation.
 *
 * This routines sets the SHA1 engine control register for MD5 computation
 * It will then load the 128 bits of the hash to prime the engine.
 */
static inline void md5_init_digest(u32 *digest, u32 control)
{
	hw_crypto_set_ctrl(control);
	asm volatile (
	"	; move digests to hash_output regs	\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_0)"(%[s_base]), 0x00(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_1)"(%[s_base]), 0x04(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_2)"(%[s_base]), 0x08(%[dgst])		\n\t"
	"	move.4	"D(UBI32_SECURITY_SHA1_HASH_OUT_3)"(%[s_base]), 0x0C(%[dgst])		\n\t"
		:
		: [s_base]"a" (SEC_BASE), [dgst]"a" (digest)
	);
}

/*
 * md5_extract_digest ()
 *	Pull intermediate digest out of engine.
 *
 * Extract 128 bits of digest from sha1 engine if
 * BUSY_BIT in SHA1_STAT register is zero.
 */
static inline void md5_extract_digest(u32 *digest)
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
		: 
		: [dgst]"a" (digest) ,[s_base]"a" (SEC_BASE)
		: "cc", "memory"
	);
}

/*
 * md5_update()
 *	Update SHA1 engine with new data.
 *
 * This routine is called by external callers after the corresponding
 * md5_init has been invoked by extern callers. This routine
 * can be called multiple times before md5_final will be called
 * by the external caller to indicate termination of the digest calculation.
 */
static int md5_update(struct shash_desc *desc, const u8 *data,
			unsigned int len)
{
	struct ubicom32_md5_ctx *mctx = shash_desc_ctx(desc);
	int index, clen;
	unsigned long flags;

	STOPWATCH_DECLARE;

	/*
	 * The data is always processed in mctx->block_size. Any left over data is stored
	 * in the mctx and processed during the next call to update or final.
	 *
	 * how much is already in the buffer? 
	 */
	index = mctx->count & (mctx->block_size - 1);

	mctx->count += len;

	if (index + len < mctx->block_size) {
		/*
		 * Insufficient data. Store what was sent in mctx->buf and exit.
		 */
		if (len) {
			memcpy(mctx->buf + index , data, len);
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
	md5_init_digest(mctx->state, mctx->control_word);

	/*
	 * Consume the previous left over data by constructing a single
	 * complete block and feeding it to the engine.
	 */
	if (index) {
		clen = mctx->block_size - index;
		memcpy(mctx->buf + index, data, clen);
		sha1_feed_512(mctx->buf);
		data += clen;
		len -= clen;
		index = 0;
	}

	if (likely(SEC_ALIGNED(data))) {
		if (likely(len >= mctx->block_size)) {
		  register unsigned int cnt = len / mctx->block_size;
			while (cnt--) {
				/*
				 * Wait for space in fifo and Load 512 bits into the engine.
				 */
				sha1_wait();
				sha1_feed_512(data);
				data += mctx->block_size;
			}

			/*
			 * All the full size blocks have been sent.
			 * reduce length to length % block_size.
			 */
			len = len & (mctx->block_size - 1);
		}
	} else {
		/* 
		 * Misaligned data, process as many blocks as possible.
		 */
		while (likely(len >= mctx->block_size)) {
			/*
			 * Copy data into an aligned buffer.
			 */
			memcpy(mctx->buf, data, mctx->block_size);

			/*
			 * Wait for space in fifo and Load 512 bits into the engine.
			 */
			sha1_wait();
			sha1_feed_512(mctx->buf);
			data += mctx->block_size;
			len -= mctx->block_size;
		}
	}

	/*
	 * Extract digest and shutdown the crypto engine.
	 */
	md5_extract_digest(mctx->state);
	STOPWATCH_END(&md5_watch[0]);
	hw_crypto_unlock(flags);

	/* 
	 * Anything left? Copy it into the buffer. We will process it in the next
	 * go around or during finialization.
	 */
	if (len) {
		memcpy(mctx->buf + index , data, len);
	}

	return 0;
}

/* 
 * md5_final()
 *	Finalize digest computation.
 *
 * Called by external caller indicating end of digest calculation.
 * Add padding to increase the size of the tail to be a multiple of 512 bits.
 * Add the length to the last 8 bytes of the buffer in little endian format and pass it to the engine.
 * Once the engine has processed the last block then extract and return the message digest. 
 */
static int md5_final(struct shash_desc *desc, u8 *out)
{
	struct ubicom32_md5_ctx *mctx = shash_desc_ctx(desc);
	u32 bits[2];
	unsigned int index, end;

	/* 
	 * Must perform manual padding.
	 */
	index = mctx->count & (mctx->block_size - 1);
	end =  (index < (mctx->block_size - sizeof(bits))) ? MD5_BLOCK_SIZE : (2 * MD5_BLOCK_SIZE);
	
	/*
	 * Start pad with 1
	 */
	mctx->buf[index] = 0x80;

	/*
	 * Pad with zeros
	 */
	index++;
	memset(mctx->buf + index, 0x00, end - index - 8);

	/*
	 * Append message length
	 */
	bits[0] = mctx->count << 3;
	bits[1] = mctx->count >> 29;
	__cpu_to_le32s(bits);
	__cpu_to_le32s(bits + 1);

	memcpy(mctx->buf + end - 8, &bits, sizeof(bits));
	
	/*
	 * Force to use the mctx->buf and ignore the partial buf
	 */
	mctx->count = mctx->count & ~(mctx->block_size -1);
	md5_update(desc, mctx->buf, end);

	/*
	 * Copy digest to out
	 */
	memcpy(out, mctx->state, MD5_DIGEST_SIZE);

	/*
	 * Wipe context
	 */
	md5_wipe_out(mctx);

	return 0;
}

static int md5_export(struct shash_desc *desc, void *out)
{
	struct ubicom32_md5_ctx *ctx = shash_desc_ctx(desc);

	memcpy(out, ctx, sizeof(*ctx));
	return 0;
}

static int md5_import(struct shash_desc *desc, const void *in)
{
	struct ubicom32_md5_ctx *ctx = shash_desc_ctx(desc);

	memcpy(ctx, in, sizeof(*ctx));
	return 0;
}

static struct shash_alg alg = {
	.digestsize	=       MD5_DIGEST_SIZE,
	.init		=       md5_init,
	.update		=       md5_update,
	.final		=       md5_final,
	.export		=	md5_export,
	.import		=	md5_import,
	.descsize	=	sizeof(struct ubicom32_md5_ctx),
	.statesize	=	sizeof(struct ubicom32_md5_ctx),
	.base		=	{
		.cra_name	=	"md5",
		.cra_driver_name=	"md5-ubicom32",
		.cra_priority	=	CRYPTO_UBICOM32_PRIORITY,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	MD5_BLOCK_SIZE,
		.cra_ctxsize	=	sizeof(struct ubicom32_md5_ctx),
		.cra_module	=	THIS_MODULE,
		.cra_list	=	LIST_HEAD_INIT(alg.base.cra_list),
	}
};

#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
static const char *md5_names [1] = {
	"md5-update"
};

/*
 * md5_watch_show()
 *	Print out the min, avg, max values for each MD5
 */
static int md5_watch_show(struct seq_file *p, void *v)
{
	int number = *((loff_t *) v);

	if (number == 0) {
		seq_puts(p, "\tmin\tavg\tmax\t(micro-seconds)\n");
	}

	stopwatch_show(&md5_watch[number], p, md5_names[number], STOPWATCH_SHOW_MICRO);
	return 0;
}

static int __init md5_watch_init(void)
{
	return stopwatch_register("md5", 1, md5_watch_show);
}

#endif

/*
 * init()
 *	Module initialization.
 */
static int __init init(void)
{
#if defined(CONFIG_UBICOM32_STOPWATCH_CRYPTO)
	md5_watch_init();
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

MODULE_ALIAS("md5");

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MD5 Secure Hash Algorithm");
