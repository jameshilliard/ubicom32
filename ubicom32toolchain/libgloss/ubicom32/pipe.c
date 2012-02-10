#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
pipe (int fd)
{
  return INT_SYS (SYS_pipe, fd, 0, 0);
}
