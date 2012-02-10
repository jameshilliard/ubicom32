#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
chmod (const char *path, mode_t mode)
{
  return INT_SYS (SYS_chmod, path, mode, 0);
}
