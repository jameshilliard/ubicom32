#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


int
_execve (const char *path, char *const argv[], char *const envp[])
{
  return INT_SYS (SYS_execve, path, argv, envp);
}

int
__execve (const char *path, char *const argv[], char *const envp[])
{
  return INT_SYS (SYS_execve, path, argv, envp);
}

