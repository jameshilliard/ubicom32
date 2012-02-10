/*
 * (C) Copyright 2010, Ubicom, Inc.
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
 */

#include <common.h>
#include <ubi_uboot.h>
#include <asm/byteorder.h>
#include <asm/ip5000.h>
#include <asm/memory.h>
#include <asm/spinlock.h>
#include <asm/unaligned.h>
#include <linux/types.h>
#include <linux/string.h>

#include "dwc_os.h"

#define	virt_to_phys(x) ((unsigned long)(x))
#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

void *DWC_MEMSET(void *dest, uint8_t byte, uint32_t size)
{
	return memset(dest, byte, size);
}

void *DWC_MEMCPY(void *dest, void const *src, uint32_t size)
{
	return memcpy(dest, src, size);
}

void *DWC_MEMMOVE(void *dest, void *src, uint32_t size)
{
	return memmove(dest, src, size);
}

int DWC_MEMCMP(void *m1, void *m2, uint32_t size)
{
	return memcmp(m1, m2, size);
}

int DWC_STRNCMP(void *s1, void *s2, uint32_t size)
{
	return strncmp(s1, s2, size);
}

int DWC_STRCMP(void *s1, void *s2)
{
	return strcmp(s1, s2);
}

int DWC_STRLEN(char const *str)
{
	return strlen(str);
}

char *DWC_STRCPY(char *to, const char *from)
{
	return strcpy(to, from);
}

char *DWC_STRDUP(char const *str)
{
	int len = DWC_STRLEN(str) + 1;
	char *new = DWC_ALLOC(len);
	if (!new) {
		return NULL;
	}
	DWC_MEMCPY(new, str, len);
	return new;
}

int DWC_ATOI(char *str, int32_t *value)
{
	char *end = NULL;
	*value = simple_strtol(str, &end, 0);
	if (*end == '\0') {
		return 0;
	}
	return -1;
}

int DWC_ATOUI(char *str, uint32_t *value)
{
	char *end = NULL;
	*value = simple_strtoul(str, &end, 0);
	if (*end == '\0') {
		return 0;
	}
	return -1;
}


/* From usbstring.c */
int DWC_UTF8_TO_UTF16LE(uint8_t const *s, uint16_t *cp, unsigned len)
{
	int	count = 0;
	u8	c;
	u16	uchar;

	/* this insists on correct encodings, though not minimal ones.
	 * BUT it currently rejects legit 4-byte UTF-8 code points,
	 * which need surrogate pairs.  (Unicode 3.1 can use them.)
	 */
	while (len != 0 && (c = (u8) *s++) != 0) {
		if (unlikely(c & 0x80)) {
			// 2-byte sequence:
			// 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
			if ((c & 0xe0) == 0xc0) {
				uchar = (c & 0x1f) << 6;

				c = (u8) *s++;
				if ((c & 0xc0) != 0xc0)
					goto fail;
				c &= 0x3f;
				uchar |= c;

			// 3-byte sequence (most CJKV characters):
			// zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
			} else if ((c & 0xf0) == 0xe0) {
				uchar = (c & 0x0f) << 12;

				c = (u8) *s++;
				if ((c & 0xc0) != 0xc0)
					goto fail;
				c &= 0x3f;
				uchar |= c << 6;

				c = (u8) *s++;
				if ((c & 0xc0) != 0xc0)
					goto fail;
				c &= 0x3f;
				uchar |= c;

				/* no bogus surrogates */
				if (0xd800 <= uchar && uchar <= 0xdfff)
					goto fail;

			// 4-byte sequence (surrogate pairs, currently rare):
			// 11101110wwwwzzzzyy + 110111yyyyxxxxxx
			//     = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
			// (uuuuu = wwww + 1)
			// FIXME accept the surrogate code points (only)

			} else
				goto fail;
		} else
			uchar = c;
		put_unaligned (cpu_to_le16 (uchar), cp++);
		count++;
		len--;
	}
	return count;
fail:
	return -1;
}

/* dwc_debug.h */

void DWC_VPRINTF(char *format, va_list args)
{
	vprintf(format, args);
}

void DWC_PRINTF(char *format, ...)
{
	va_list args;
	va_start(args, format);
	DWC_VPRINTF(format, args);
	va_end(args);
}

int DWC_SPRINTF(char *buffer, char *format, ...)
{
	int retval;
	va_list args;
	va_start(args, format);
	retval = vsprintf(buffer, format, args);
	va_end(args);
	return retval;
}

void __DWC_WARN(char *format, ...)
{
	va_list args;
	va_start(args, format);
	DWC_VPRINTF(format, args);
	va_end(args);
}

void __DWC_ERROR(char *format, ...)
{
	va_list args;
	va_start(args, format);
	DWC_VPRINTF(format, args);
	va_end(args);
}

void DWC_EXCEPTION(char *format, ...)
{
	va_list args;
	va_start(args, format);
	DWC_VPRINTF(format, args);
	va_end(args);
	BUG_ON(1);
}

#ifdef CONFIG_DWC_DEBUG
void __DWC_DEBUG(char *format, ...)
{
	va_list args;
	va_start(args, format);
	DWC_VPRINTF(format, args);
	va_end(args);
}
#endif



/* dwc_mem.h */

void *__DWC_DMA_ALLOC(uint32_t size, dwc_dma_t *dma_addr)
{
	void *buf = memalign(CACHE_LINE_SIZE, size);
	*dma_addr = (dma_addr_t)(virt_to_phys(buf));

	if (!buf) {
		return NULL;
	}
	memset(buf, 0, (size_t)size);

	return buf;
}

void __DWC_DMA_FREE(uint32_t size, void *virt_addr, dwc_dma_t dma_addr)
{
	free(virt_addr);
}

void *__DWC_ALLOC(uint32_t size)
{
	void *buf = malloc(size);

	if (!buf) {
		return NULL;
	}
	memset(buf, 0, (size_t)size);
	return buf;
}

void *__DWC_ALLOC_ATOMIC(uint32_t size)
{
	return __DWC_ALLOC(size);
}

void __DWC_FREE(void *addr)
{
	free(addr);
}

/* Byte Ordering Conversions. */
uint32_t DWC_CPU_TO_LE32(void *p)
{
#ifdef __LITTLE_ENDIAN
	return *((uint32_t *)p);
#else
	return cpu_to_le32(*(u32 *)p);
#endif
}

uint32_t DWC_CPU_TO_BE32(void *p)
{
#ifdef __BIG_ENDIAN
	return *((uint32_t *)p);
#else
	uint8_t *u_p = (uint8_t *)p;

	return (u_p[3] | (u_p[2] << 8) | (u_p[1] << 16) | (u_p[0] << 24));
#endif
}

uint32_t DWC_LE32_TO_CPU(void *p)
{
#ifdef __LITTLE_ENDIAN
	return *((uint32_t *)p);
#else
	return le32_to_cpu(*(u32 *)p);
#endif
}

uint32_t DWC_BE32_TO_CPU(void *p)
{
#ifdef __BIG_ENDIAN
	return *((uint32_t *)p);
#else
	uint8_t *u_p = (uint8_t *)p;

	return (u_p[3] | (u_p[2] << 8) | (u_p[1] << 16) | (u_p[0] << 24));
#endif
}

uint16_t DWC_CPU_TO_LE16(void *p)
{
#ifdef __LITTLE_ENDIAN
	return *((uint16_t *)p);
#else
	return cpu_to_le16(*(u16 *)p);
#endif
}

uint16_t DWC_CPU_TO_BE16(void *p)
{
#ifdef __BIG_ENDIAN
	return *((uint16_t *)p);
#else
	uint8_t *u_p = (uint8_t *)p;
	return (u_p[1] | (u_p[0] << 8));
#endif
}

uint16_t DWC_LE16_TO_CPU(void *p)
{
#ifdef __LITTLE_ENDIAN
	return *((uint16_t *)p);
#else
	return le16_to_cpu(*(u16 *)p);
#endif
}
EXPORT_SYMBOL(DWC_LE16_TO_CPU);

uint16_t DWC_BE16_TO_CPU(void *p)
{
#ifdef __BIG_ENDIAN
	return *((uint16_t *)p);
#else
	uint8_t *u_p = (uint8_t *)p;
	return (u_p[1] | (u_p[0] << 8));
#endif
}

/* Registers */

uint32_t DWC_READ_REG32(uint32_t volatile *reg)
{
  return ubi32_br_readl(reg);
}

#if 0
uint64_t DWC_READ_REG64(uint64_t volatile *reg)
{
}
#endif

void DWC_WRITE_REG32(uint32_t volatile *reg, uint32_t value)
{
  ubi32_br_writel(reg, value);
}

#if 0
void DWC_WRITE_REG64(uint64_t volatile *reg, uint64_t value)
{
}
#endif

void DWC_MODIFY_REG32(uint32_t volatile *reg, uint32_t clear_mask, uint32_t set_mask)
{
	ubi32_br_writel(reg, (ubi32_br_readl(reg) & ~clear_mask) | set_mask);
}

#if 0
void DWC_MODIFY_REG64(uint64_t volatile *reg, uint64_t value)
{
}
#endif

dwc_spinlock_t *DWC_SPINLOCK_ALLOC(void)
{
	struct ubi32_spinlock_t *sl = (struct ubi32_spinlock_t *)1;
	sl = DWC_ALLOC(sizeof(*sl));

	return (dwc_spinlock_t *)sl;
}

void DWC_SPINLOCK_FREE(dwc_spinlock_t *lock)
{
	DWC_FREE(lock);
}

void DWC_SPINLOCK(dwc_spinlock_t *lock)
{
}

void DWC_SPINUNLOCK(dwc_spinlock_t *lock)
{
}

void DWC_SPINLOCK_IRQSAVE(dwc_spinlock_t *lock, uint64_t *flags)
{
}

void DWC_SPINUNLOCK_IRQRESTORE(dwc_spinlock_t *lock, uint64_t flags)
{
}
/* Timing */

void DWC_UDELAY(uint32_t usecs)
{
	udelay(usecs);
}

void DWC_MDELAY(uint32_t msecs)
{
	udelay(msecs * 1000);
}
