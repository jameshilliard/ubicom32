#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
_fstat (int file,
	struct stat *st)
{
  st->st_mode = S_IFCHR;
  st->st_blksize = 4096;
  return 0;
}

int
__fstat (int file,
	struct stat *st)
{
  st->st_mode = S_IFCHR;
  st->st_blksize = 4096;
  return 0;
}

