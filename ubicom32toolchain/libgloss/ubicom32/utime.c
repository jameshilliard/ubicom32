#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
utime (path, times)
     const char *path;
     char *times;
{
  return INT_SYS (SYS_utime, path, times, 0);
}
