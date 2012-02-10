#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


time_t
time (time_t *tloc)
{
  return INT_SYS (SYS_time, tloc, 0, 0);
}
