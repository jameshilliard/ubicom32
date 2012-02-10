#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"

int
_read (int file,
       char *ptr,
       size_t len)
{
  return INT_SYS (SYS_read, file, ptr, len);
}

int
__read (int file,
       char *ptr,
       size_t len)
{
  return INT_SYS (SYS_read, file, ptr, len);
}

