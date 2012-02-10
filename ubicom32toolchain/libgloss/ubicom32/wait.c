#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
_wait (statusp)
     int *statusp;
{
  return INT_SYS (SYS_wait, 0, 0, 0);
}

int
__wait (statusp)
     int *statusp;
{
  return INT_SYS (SYS_wait, 0, 0, 0);
}

