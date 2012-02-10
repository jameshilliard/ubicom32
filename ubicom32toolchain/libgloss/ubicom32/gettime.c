#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"
#include "sys/time.h"


int
_gettimeofday (struct timeval *tp, void *tzp)
{
  return INT_SYS (SYS_gettimeofday, tp, tzp, 0);
}

int
__gettimeofday (struct timeval *tp, void *tzp)
{
  return INT_SYS (SYS_gettimeofday, tp, tzp, 0);
}

