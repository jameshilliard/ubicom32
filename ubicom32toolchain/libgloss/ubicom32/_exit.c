#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "intsys.h"


static void _do_dtors()
{
  /* The loop variable is static so that if a destructor calls exit, 
     and we return here, we simply continue with the next destructor. */
  typedef void (*pfunc) ();
  extern pfunc ___dtors[];
  extern pfunc ___dtors_end[];
  static pfunc *p = ___dtors;

#if 0
  if(*((long*)p) == 0) return;
#endif
  /* list must start with -1 */
  if(*((long*)p) != -1) return;
  if(*((long*)p) == -1) p++;
  
while (p < ___dtors_end && *((long*)p) != 0)
    (*p++) ();
}

void _exit (n)
{
  extern unsigned char ___eh_frame_begin[];
  extern void __deregister_frame_info (void *);

  /* Destructors should be done earlier because they need to be done before the
     files are closed, but here is better than nowhere (and this balances the
     constructors done in crt1.c. */
  _do_dtors();

  /* Deregister any eh frames.  */
  if(___eh_frame_begin && *___eh_frame_begin) 
    __deregister_frame_info (___eh_frame_begin);

  INT_SYS (SYS_exit, n, 0, 0);
}

#if 0
void __exit (n)
{
  extern unsigned char __eh_frame_begin[];
  extern void __deregister_frame_info (void *);

  /* Destructors should be done earlier because they need to be done before the
     files are closed, but here is better than nowhere (and this balances the
     constructors done in crt1.c. */
  _do_dtors();

  /* Deregister any eh frames.  */
  if(__eh_frame_begin && *__eh_frame_begin) 
    __deregister_frame_info (__eh_frame_begin);

  INT_SYS (SYS_exit, n, 0, 0);
}
#endif
