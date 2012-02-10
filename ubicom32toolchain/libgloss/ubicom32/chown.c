#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"



int
chown (const char *path, short owner, short group)
{
  return INT_SYS (SYS_chown, path, owner, group);
}
