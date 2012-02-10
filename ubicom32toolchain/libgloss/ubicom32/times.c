#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"
#include "sys/times.h"


clock_t
_times (struct tms *buffer)
{
  return INT_SYS (SYS_times, buffer, 0, 0);
}
