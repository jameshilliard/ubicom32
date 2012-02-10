#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "intsys.h"


off_t
_lseek (int file,
	off_t ptr,
	int dir)
{
  return INT_SYS (SYS_lseek, file, ptr, dir);
}

off_t
__lseek (int file,
	off_t ptr,
	int dir)
{
  return INT_SYS (SYS_lseek, file, ptr, dir);
}

