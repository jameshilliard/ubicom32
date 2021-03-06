/* shm.cc: XSI IPC interface for Cygwin.

   Copyright 2003 Red Hat, Inc.

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#include "winsup.h"
#include "cygerrno.h"
#include <signal.h>
#ifdef USE_SERVER
#include <sys/types.h>
#include <sys/queue.h>
#include <stdio.h>
#include <unistd.h>

#include "pinfo.h"
#include "sigproc.h"

#include "cygserver_ipc.h"
#include "cygserver_shm.h"
#include "cygtls.h"

/*
 * client_request_shm Constructors
 */

client_request_shm::client_request_shm (int shmid,
					const void *shmaddr,
					int shmflg)
  : client_request (CYGSERVER_REQUEST_SHM, &_parameters, sizeof (_parameters))
{
  _parameters.in.shmop = SHMOP_shmat;
  ipc_set_proc_info (_parameters.in.ipcblk);

  _parameters.in.atargs.shmid = shmid;
  _parameters.in.atargs.shmaddr = shmaddr;
  _parameters.in.atargs.shmflg = shmflg;

  msglen (sizeof (_parameters.in));
}

client_request_shm::client_request_shm (int shmid,
					int cmd,
					struct shmid_ds *buf)
  : client_request (CYGSERVER_REQUEST_SHM, &_parameters, sizeof (_parameters))
{
  _parameters.in.shmop = SHMOP_shmctl;
  ipc_set_proc_info (_parameters.in.ipcblk);

   _parameters.in.ctlargs.shmid = shmid;
   _parameters.in.ctlargs.cmd = cmd;
   _parameters.in.ctlargs.buf = buf;

  msglen (sizeof (_parameters.in));
}

client_request_shm::client_request_shm (const void *shmaddr)
  : client_request (CYGSERVER_REQUEST_SHM, &_parameters, sizeof (_parameters))
{
  _parameters.in.shmop = SHMOP_shmdt;
  ipc_set_proc_info (_parameters.in.ipcblk);

  _parameters.in.dtargs.shmaddr = shmaddr;

  msglen (sizeof (_parameters.in));
}

client_request_shm::client_request_shm (key_t key,
					size_t size,
					int shmflg)
  : client_request (CYGSERVER_REQUEST_SHM, &_parameters, sizeof (_parameters))
{
  _parameters.in.shmop = SHMOP_shmget;
  ipc_set_proc_info (_parameters.in.ipcblk);

  _parameters.in.getargs.key = key;
  _parameters.in.getargs.size = size;
  _parameters.in.getargs.shmflg = shmflg;

  msglen (sizeof (_parameters.in));
}

client_request_shm::client_request_shm (proc *p1)
  : client_request (CYGSERVER_REQUEST_SHM, &_parameters, sizeof (_parameters))
{
  _parameters.in.shmop = SHMOP_shmfork;
  ipc_set_proc_info (_parameters.in.ipcblk);

  _parameters.in.forkargs = *p1;
}

/* List of shmid's with file mapping HANDLE and size, returned by shmget. */
struct shm_shmid_list {
  SLIST_ENTRY (shm_shmid_list) ssh_next;
  int shmid;
  vm_object_t hdl;
  size_t size;
};

static SLIST_HEAD (, shm_shmid_list) ssh_list;

/* List of attached mappings, as returned by shmat. */
struct shm_attached_list {
  SLIST_ENTRY (shm_attached_list) sph_next;
  vm_object_t ptr;
  vm_object_t hdl;
  size_t size;
  int access;
};

static SLIST_HEAD (, shm_attached_list) sph_list;

int __stdcall
fixup_shms_after_fork ()
{
  if (!SLIST_FIRST (&sph_list))
    return 0;
  pinfo p (myself->ppid);
  proc parent = { myself->ppid, p->dwProcessId, p->uid, p->gid };

  client_request_shm request (&parent);
  if (request.make_request () == -1 || request.retval () == -1)
    {
      syscall_printf ("-1 [%d] = fixup_shms_after_fork ()", request.error_code ());
      set_errno (request.error_code ());
      return 0;
    }
  shm_attached_list *sph_entry;
  /* Remove map from list... */
  SLIST_FOREACH (sph_entry, &sph_list, sph_next)
    {
      vm_object_t ptr = MapViewOfFileEx (sph_entry->hdl, sph_entry->access,
					 0, 0, sph_entry->size, sph_entry->ptr);
      if (ptr != sph_entry->ptr)
	api_fatal ("MapViewOfFileEx (%p), %E.  Terminating.", sph_entry->ptr);
    }
  return 0;
}
#endif /* USE_SERVER */

/*
 * XSI shmaphore API.  These are exported by the DLL.
 */

extern "C" void *
shmat (int shmid, const void *shmaddr, int shmflg)
{
#ifdef USE_SERVER
  syscall_printf ("shmat (shmid = %d, shmaddr = %p, shmflg = 0x%x)",
		  shmid, shmaddr, shmflg);

  shm_shmid_list *ssh_entry;
  SLIST_FOREACH (ssh_entry, &ssh_list, ssh_next)
    {
      if (ssh_entry->shmid == shmid)
	break;
    }
  if (!ssh_entry)
    {
      /* The shmid is unknown to this process so far.  Try to get it from
	 the server if it exists.  Use special internal call to shmget,
	 which interprets the key as a shmid and only returns a valid
	 shmid if one exists.  Since shmctl inserts a new entry for this
	 shmid into ssh_list automatically, we just have to go through
	 that list again.  If that still fails, well, bad luck. */
      if (shmid && shmget ((key_t) shmid, 0, IPC_KEY_IS_SHMID) != -1)
	{
	  SLIST_FOREACH (ssh_entry, &ssh_list, ssh_next)
	    {
	      if (ssh_entry->shmid == shmid)
		break;
	    }
	}
      if (!ssh_entry)
	{
	  /* Invalid shmid */
	  set_errno (EINVAL);
	  return (void *) -1;
	}
    }
  vm_object_t attach_va = NULL;
  if (shmaddr)
    {
      if (shmflg & SHM_RND)
	attach_va = (vm_object_t)((vm_offset_t)shmaddr & ~(SHMLBA-1));
      else
	attach_va = (vm_object_t)shmaddr;
      /* Don't even bother to call anything if shmaddr is NULL or
	 not aligned. */
      if (!attach_va || (vm_offset_t)attach_va % SHMLBA)
	{
	  set_errno (EINVAL);
	  return (void *) -1;
	}
    }
  /* Try allocating memory before calling cygserver. */
  shm_attached_list *sph_entry = new (shm_attached_list);
  if (!sph_entry)
    {
      set_errno (ENOMEM);
      return (void *) -1;
    }
  DWORD access = (shmflg & SHM_RDONLY) ? FILE_MAP_READ : FILE_MAP_WRITE;
  vm_object_t ptr = MapViewOfFileEx (ssh_entry->hdl, access, 0, 0,
				     ssh_entry->size, attach_va);
  if (!ptr)
    {
      __seterrno ();
      delete sph_entry;
      return (void *) -1;
    }
  /* Use returned ptr address as is, so it's stored using the exact value
     in cygserver. */
  client_request_shm request (shmid, ptr, shmflg & ~SHM_RND);
  if (request.make_request () == -1 || request.ptrval () == NULL)
    {
      syscall_printf ("-1 [%d] = shmat ()", request.error_code ());
      UnmapViewOfFile (ptr);
      delete sph_entry;
      set_errno (request.error_code ());
      if (request.error_code () == ENOSYS)
	raise (SIGSYS);
      return (void *) -1;
    }
  sph_entry->ptr = ptr;
  sph_entry->hdl = ssh_entry->hdl;
  sph_entry->size = ssh_entry->size;
  sph_entry->access = access;
  SLIST_INSERT_HEAD (&sph_list, sph_entry, sph_next);
  return ptr;
#else
  set_errno (ENOSYS);
  raise (SIGSYS);
  return (void *) -1;
#endif
}

extern "C" int
shmctl (int shmid, int cmd, struct shmid_ds *buf)
{
#ifdef USE_SERVER
  syscall_printf ("shmctl (shmid = %d, cmd = %d, buf = 0x%x)",
		  shmid, cmd, buf);
  myfault efault;
  if (efault.faulted (EFAULT))
    return -1;
  client_request_shm request (shmid, cmd, buf);
  if (request.make_request () == -1 || request.retval () == -1)
    {
      syscall_printf ("-1 [%d] = shmctl ()", request.error_code ());
      set_errno (request.error_code ());
      if (request.error_code () == ENOSYS)
	raise (SIGSYS);
      return -1;
    }
  if (cmd == IPC_RMID)
    {
      /* The process must cleanup its own storage... */
      shm_shmid_list *ssh_entry, *ssh_next_entry;
      SLIST_FOREACH_SAFE (ssh_entry, &ssh_list, ssh_next, ssh_next_entry)
	{
	  if (ssh_entry->shmid == shmid)
	    {
	      SLIST_REMOVE (&ssh_list, ssh_entry, shm_shmid_list, ssh_next);
	      /* ...and close the handle. */
	      CloseHandle (ssh_entry->hdl);
	      delete ssh_entry;
	      break;
	    }
	}
    }
  return request.retval ();
#else
  set_errno (ENOSYS);
  raise (SIGSYS);
  return -1;
#endif
}

extern "C" int
shmdt (const void *shmaddr)
{
#ifdef USE_SERVER
  syscall_printf ("shmdt (shmaddr = %p)", shmaddr);
  client_request_shm request (shmaddr);
  if (request.make_request () == -1 || request.retval () == -1)
    {
      syscall_printf ("-1 [%d] = shmdt ()", request.error_code ());
      set_errno (request.error_code ());
      if (request.error_code () == ENOSYS)
	raise (SIGSYS);
      return -1;
    }
  shm_attached_list *sph_entry, *sph_next_entry;
  /* Remove map from list... */
  SLIST_FOREACH_SAFE (sph_entry, &sph_list, sph_next, sph_next_entry)
    {
      if (sph_entry->ptr == shmaddr)
	{
	  SLIST_REMOVE (&sph_list, sph_entry, shm_attached_list, sph_next);
	  /* ...and unmap view. */
	  UnmapViewOfFile (sph_entry->ptr);
	  delete sph_entry;
	  break;
	}
    }
  return request.retval ();
#else
  set_errno (ENOSYS);
  raise (SIGSYS);
  return -1;
#endif
}

extern "C" int
shmget (key_t key, size_t size, int shmflg)
{
#ifdef USE_SERVER
  syscall_printf ("shmget (key = %U, size = %d, shmflg = 0x%x)",
		  key, size, shmflg);
  /* Try allocating memory before calling cygserver. */
  shm_shmid_list *ssh_new_entry = new (shm_shmid_list);
  if (!ssh_new_entry)
    {
      set_errno (ENOMEM);
      return -1;
    }
  client_request_shm request (key, size, shmflg);
  if (request.make_request () == -1 || request.retval () == -1)
    {
      syscall_printf ("-1 [%d] = shmget ()", request.error_code ());
      delete ssh_new_entry;
      set_errno (request.error_code ());
      if (request.error_code () == ENOSYS)
	raise (SIGSYS);
      return -1;
    }
  int shmid = request.retval ();	/* Shared mem ID */
  vm_object_t hdl = request.objval ();	/* HANDLE associated with it. */
  shm_shmid_list *ssh_entry;
  SLIST_FOREACH (ssh_entry, &ssh_list, ssh_next)
    {
      if (ssh_entry->shmid == shmid)
	{
	  /* We already maintain an entry for this shmid.  That means,
	     the hdl returned by cygserver is a superfluous duplicate
	     of the original hdl maintained by cygserver.  We can safely
	     delete it. */
	  CloseHandle (hdl);
	  delete ssh_new_entry;
	  return shmid;
	}
    }
  /* We arrive here only if shmid is a new one for this process.  Add the
     shmid and hdl value to the list. */
  ssh_new_entry->shmid = shmid;
  ssh_new_entry->hdl = hdl;
  ssh_new_entry->size = size;
  SLIST_INSERT_HEAD (&ssh_list, ssh_new_entry, ssh_next);
  return shmid;
#else
  set_errno (ENOSYS);
  raise (SIGSYS);
  return -1;
#endif
}
