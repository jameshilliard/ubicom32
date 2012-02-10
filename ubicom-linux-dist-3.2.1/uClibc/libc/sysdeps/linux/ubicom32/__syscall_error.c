#include <errno.h>
#include <features.h>

/* This routine is jumped to by some of the syscall handlers, to stash
 * an error number into errno.  */
int __syscall_error(int err_no) attribute_hidden;
int __syscall_error(int err_no)
{
	__set_errno(err_no);
	return -1;
}
