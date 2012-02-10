#include <_ansi.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"
#include "regs.h"


caddr_t
_sbrk (size_t incr)
{
  extern char _end;		/* Defined by the linker */
  extern char _heaplimit;       /* Defined by the linker */
  static char *heap_end;
  char *prev_heap_end;
  char *sp = (char *)&sp;

  if (heap_end == 0)
    {
      heap_end = &_end;
    }
  prev_heap_end = heap_end;

  /* Check for stack and heap collision in which case quit immediately. */
  /* This will only catch the problem if the stack grows into the heap  */
  /* and even then only when memory is requested from the heap, so this */
  /* could go undetected for quite some time.  That's bad.              */
  if (heap_end > sp)
    {
      _write (1, "Heap and stack collision\n", 25);
      abort();
    }

  /* Check to see if fulfilling the request is going to grow the heap */
  /* beyond the limit specified in the linker script.                 */
  if (heap_end + incr > &_heaplimit)
    {
      errno = ENOMEM;
      return (caddr_t)-1;
    }

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

caddr_t
__sbrk (size_t incr)
{
  extern char _end;		/* Defined by the linker */
  extern char _heaplimit;       /* Defined by the linker */
  static char *heap_end;
  char *prev_heap_end;
  char *sp = (char *)&sp;

  if (heap_end == 0)
    {
      heap_end = &_end;
    }
  prev_heap_end = heap_end;

  /* Check for stack and heap collision in which case quit immediately. */
  /* This will only catch the problem if the stack grows into the heap  */
  /* and even then only when memory is requested from the heap, so this */
  /* could go undetected for quite some time.  That's bad.              */
  if (heap_end > sp)
    {
      _write (1, "Heap and stack collision\n", 25);
      abort();
    }

  /* Check to see if fulfilling the request is going to grow the heap */
  /* beyond the limit specified in the linker script.                 */
  if (heap_end + incr > &_heaplimit)
    {
      errno = ENOMEM;
      return (caddr_t)-1;
    }

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

