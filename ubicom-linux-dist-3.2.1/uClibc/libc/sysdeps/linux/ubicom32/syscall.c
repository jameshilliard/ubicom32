/* vi: set sw=4 ts=4: */
/* syscall for ubicom32/uClibc
 *
 * Copyright (C) 2008-2009 by Ubicom Inc.
 * Copyright (C) 2002 by Erik Andersen <andersen@uclibc.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <features.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>

extern long syscall(long sysnum, long a, long b, long c, long d, long e, long f);
long syscall(long sysnum, long a, long b, long c, long d, long e, long f)
{
	int __NR_syscall_number = sysnum;
	return (long) INLINE_SYSCALL(syscall_number, 6, a, b, c, d, e, f);
}
