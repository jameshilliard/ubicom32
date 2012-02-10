#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
execv (const char *path, char *const argv[])
{
  return INT_SYS (SYS_execv, path, argv, 0);
}
