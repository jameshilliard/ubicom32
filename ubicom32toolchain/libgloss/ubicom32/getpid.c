#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


_getpid (n)
{
  return 1;
}

__getpid (n)
{
  return 1;
}
