#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


isatty (fd)
     int fd;
{
  return 1;
}
