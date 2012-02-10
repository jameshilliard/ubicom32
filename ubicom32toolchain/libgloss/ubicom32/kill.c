#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


_kill (n, m)
{
  return INT_SYS (SYS_exit, 0xdead, 0, 0);
}

__kill (n, m)
{
  return INT_SYS (SYS_exit, 0xdead, 0, 0);
}

