#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
_stat (const char *path, struct stat *st)

{
  return INT_SYS (SYS_stat, path, st, 0);
}

int
__stat (const char *path, struct stat *st)

{
  return INT_SYS (SYS_stat, path, st, 0);
}

