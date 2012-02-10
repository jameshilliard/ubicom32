#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
_write ( int file,
	 char *ptr,
	 size_t len)
{
  return INT_SYS (SYS_write, file, ptr, len);
}

int
__write ( int file,
	 char *ptr,
	 size_t len)
{
  return INT_SYS (SYS_write, file, ptr, len);
}
