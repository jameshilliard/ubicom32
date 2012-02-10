/* BFD support for the Ubicom32 processor.
   Copyright (C) 2000 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

static const bfd_arch_info_type *
ubicom32_arch_compatible (const bfd_arch_info_type *a,
			const bfd_arch_info_type *b)
{
  if (a->arch != b->arch)
    return NULL;

  if (a->bits_per_word != b->bits_per_word)
    return NULL;

  if (b->mach != a->mach) {
    if (b->mach == bfd_mach_ubicom32ver4  &&
	strcmp("ubicom32uclinux", b->printable_name) == 0) {
      printf("Returing 0x%08x\n", b);
      return b;
    } else if (a->mach == bfd_mach_ubicom32ver4  &&
	strcmp("ubicom32uclinux", a->printable_name) == 0) {
      printf("Returing 0x%08x\n", a);
      return a;
    }
  }

  if (a->mach > b->mach)
    return a;

  if (b->mach > a->mach)
    return b;

  if (b->mach == bfd_mach_ubicom32ver4  && 
      strcmp("ubicom32uclinux", b->printable_name) == 0) {
	  return b;
  }
  
  return a;
}

const bfd_arch_info_type bfd_ubicom32_ver5_arch =
{
  32,				/* bits per word */
  32,				/* bits per address */
  8,				/* bits per byte */
  bfd_arch_ubicom32,		/* architecture */
  bfd_mach_ubicom32ver5,	/* machine */
  "ubicom32",			/* architecture name */
  "ubicom32ver5",		/* printable name */
  3,				/* section align power */
  FALSE,			/* the default ? */
  ubicom32_arch_compatible,	/* architecture comparison fn */
  bfd_default_scan,		/* string to architecture convert fn */
  NULL				/* next in list */
};

const bfd_arch_info_type bfd_ubicom32_uclinux_arch =
{
  32,				/* bits per word */
  32,				/* bits per address */
  8,				/* bits per byte */
  bfd_arch_ubicom32,		/* architecture */
  bfd_mach_ubicom32ver4,	/* machine */
  "ubicom32",			/* architecture name */
  "ubicom32uclinux",		/* printable name */
  3,				/* section align power */
  FALSE,			/* the default ? */
  ubicom32_arch_compatible,	/* architecture comparison fn */
  bfd_default_scan,		/* string to architecture convert fn */
  &bfd_ubicom32_ver5_arch	/* next in list */
};

const bfd_arch_info_type bfd_ubicom32_posix_arch =
{
  32,				/* bits per word */
  32,				/* bits per address */
  8,				/* bits per byte */
  bfd_arch_ubicom32,		/* architecture */
  bfd_mach_ubicom32ver4,	/* machine */
  "ubicom32",			/* architecture name */
  "ubicom32posix",		/* printable name */
  3,			        /* section align power */
  FALSE,			/* the default ? */
  bfd_default_compatible,	/* architecture comparison fn */
  bfd_default_scan,		/* string to architecture convert fn */
  &bfd_ubicom32_uclinux_arch,	/* next in list */
};

const bfd_arch_info_type bfd_ubicom32_ver4_arch =
{
  32,				/* bits per word */
  32,				/* bits per address */
  8,				/* bits per byte */
  bfd_arch_ubicom32,		/* architecture */
  bfd_mach_ubicom32ver4,	/* machine */
  "ubicom32",			/* architecture name */
  "ubicom32ver4",		/* printable name */
  3,				/* section align power */
  FALSE,			/* the default ? */
  ubicom32_arch_compatible,	/* architecture comparison fn */
  bfd_default_scan,		/* string to architecture convert fn */
  &bfd_ubicom32_posix_arch	/* next in list */
};

const bfd_arch_info_type bfd_ubicom32_nonext_arch =
{
  32,				/* bits per word */
  32,				/* bits per address */
  8,				/* bits per byte */
  bfd_arch_ubicom32,		/* architecture */
  bfd_mach_ubicom32dsp,		/* machine */
  "ubicom32",			/* architecture name */
  "ubicom32dsp",		/* printable name */
  3,				/* section align power */
  FALSE,			/* the default ? */
  bfd_default_compatible,	/* architecture comparison fn */
  bfd_default_scan,		/* string to architecture convert fn */
  & bfd_ubicom32_ver4_arch	/* next in list */
};

const bfd_arch_info_type bfd_ubicom32_arch =
{
  32,				/* bits per word */
  32,				/* bits per address */
  8,				/* bits per byte */
  bfd_arch_ubicom32,		/* architecture */
  bfd_mach_ubicom32,		/* machine */
  "ubicom32",			/* architecture name */
  "ubicom32",			/* printable name */
  3,				/* section align power */
  TRUE,				/* the default ? */
  bfd_default_compatible,	/* architecture comparison fn */
  bfd_default_scan,		/* string to architecture convert fn */
  & bfd_ubicom32_nonext_arch	/* next in list */
};
