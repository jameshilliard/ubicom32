#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
_open (const char *path, int flags, int mode)
{
  return INT_SYS (SYS_open, path, flags, mode);
}

int
__open (const char *path, int flags, int mode)
{
  return INT_SYS (SYS_open, path, flags, mode);
}

