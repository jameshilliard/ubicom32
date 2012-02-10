#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
creat (const char *path,
	int mode)
{
  return INT_SYS (SYS_creat, path, mode, 0);
}
