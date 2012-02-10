/* Remote target communications for serial-line targets in custom GDB protocol

   Copyright 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996,
   1997, 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* See the GDB User Guide for details of the GDB remote protocol. */

#include "defs.h"
#include "gdb_string.h"
#include <ctype.h>
#include <fcntl.h>
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"
#include "target.h"
/*#include "terminal.h" */
#include "gdbcmd.h"
#include "objfiles.h"
#include "gdb-stabs.h"
#include "gdbthread.h"
#include "remote.h"
#include "regcache.h"
#include "value.h"
#include "gdb_assert.h"

#include <ctype.h>
#include <sys/time.h>
#ifdef USG
#include <sys/types.h>
#endif

#include "event-loop.h"
#include "event-top.h"
#include "inf-loop.h"

#include <signal.h>
#include "serial.h"

#include "gdbcore.h" /* for exec_bfd */
#include "remote-ubicom32-isd.h"
#include "remote-ubicom32-dbginterface.h"
#include "ubicom32-tdep.h"

volatile int ubicom32_cntrl_c_abort;
unsigned ubicom32_current_thread= 1;
unsigned int currentThread=0;
unsigned int debuggerThreadNo=0;
unsigned int dontDebugMask=0;	// Threads that we will not debug
extern unsigned int flashMID, flashType, flashSectorSize, flashSize;

enum ubicom32_state {
  UBICOM32_RUNNING,
  UBICOM32_STOPPED,
  UBICOM32_DETACHED,
  UBICOM32_STEPPING,
  UBICOM32_STEPPING_PROBLEMS,
  UBICOM32_DCAPT_ERROR,
  UBICOM32_PARITY_ERROR,
  UBICOM32_STEP_INTERPRET_JMP,
  UBICOM32_STEP_INTERPRET_CALL,
  UBICOM32_STEP_INTERPRET_CALLI,
  UBICOM32_STEP_INTERPRET_REST
};

enum ubicom32_state remote_ubicom32_state = UBICOM32_DETACHED;

struct step_interpret {
  unsigned int current_pc;
  unsigned int target1_pc;
  unsigned int target2_pc;
  unsigned int address_register;
  unsigned int stepping_thread;
};

struct step_interpret stepinterpret;
extern unsigned int single_step_buffer_address;
//extern int ubicom32_write_bytes (unsigned int address, char *buffer, int length);

/* GDB structure.  */
struct target_ops ubicom32_ops;
struct target_ops ubicom32_core_ops;

#define UBICOM32_FLASH_ADDR 0x20000000
#define UBICOM32_FLASH_END	0x2ffffffc
#define UBICOM32_LOAD_BLOCK_SIZE 1024*2

#define UBICOM32_PRAM_ADDR 0x40000000
#define UBICOM32_PRAM_END  0x4003fffc

#define UBICOM32_FLASH_SECTION 1
#define UBICOM32_PRAM_SECTION 2

#define REGISTER_RAW_SIZE(x)  (4)


/* This global variable is set to 3023 for mercury or it is set to 5133 for mars */
unsigned int debugging_processor;

/* ubicom32_loadable_section.  */
static int
ubicom32_loadable_section(bfd *loadfile_bfd, asection *s)
{
  int addr, length;

  if (!(s->flags & SEC_LOAD))
    return 0;

  length = bfd_get_section_size (s);
  if (length == 0)
    return 0;

  if ((s->lma >= UBICOM32_FLASH_ADDR) && (s->lma + length <= UBICOM32_FLASH_END))
    return UBICOM32_FLASH_SECTION;
  if ((s->lma >= UBICOM32_PRAM_ADDR) && (s->lma + length <= UBICOM32_PRAM_END))
    return UBICOM32_PRAM_SECTION;
  return 0;
}

/* ubicom32_lookup_symbol.  */
static unsigned int
ubicom32_lookup_symbol (char *name)
{
  struct symbol *sym;
  struct minimal_symbol *msym;

  sym = lookup_symbol (name, NULL, VAR_DOMAIN, NULL, NULL);
  if (sym)
    return SYMBOL_VALUE (sym);

  msym = lookup_minimal_symbol (name, NULL, NULL);
  if (msym)
    return SYMBOL_VALUE (msym);

  return 0;
}


static void
swizzle_for_write_register (int regnum, const void *val)
{
  int i, j;
  char * value = (char *)malloc(REGISTER_RAW_SIZE (regnum));
  char *ptr = (char *)val;

  for(i= (REGISTER_RAW_SIZE (regnum)) -1, j=0;i >= 0; i--)
    {
      value[j++] = ptr[i];
    }

  /* copy the data back into the buffer */
  for(i= (REGISTER_RAW_SIZE (regnum)) -1, j=0;i >= 0; i--)
    {
      ptr[i] = value[i];
    }

  free(value);
}


static void
swizzled_supply_register (struct regcache *regcache, int regnum, const void *val)
{
  int i, j;
  char * value = (char *)malloc(REGISTER_RAW_SIZE (regnum));
  char *ptr = (char *)val;

  for(i= (REGISTER_RAW_SIZE (regnum)) -1, j=0;i >= 0; i--)
    {
      value[j++] = ptr[i];
    }

  regcache_raw_supply(regcache, regnum, (const void *)value);
  free(value);
}

void
toggleMtEn(void)
{
  int i;
  int ret;
  CORE_ADDR pc;

  for(i=0; i< 8; i++)
    {
      int threadMask = 1 << i;

      if(i == debuggerThreadNo)
	continue;

      if(mainRegisters.globals.mt_en & threadMask)
	{
	  /* this is an enabled thread. Read its pc */
	  if(mainRegisters.perThreadRead[i])
	    pc = mainRegisters.tRegs[i].threadPc;
	  else
	    {
	      /* call the lowlevel routines to pull up the pc */
	      ret = readRegisters(i, 34,(void *) &pc, 1);
	    }

	  /* if the pc is in the flash then toggle the enable bit */
	  if(0x20000000< pc && pc < 0x20000000 + flashSize)
	    {
	      /* We have to toggle the mt_en bit to off */
	      mainRegisters.globals.mt_en &= ~threadMask;

	      ret = writeRegisters(i, 48,
				   &mainRegisters.globals.mt_en, 1);

	      /* turn mt_en for this thread back to on */
	      mainRegisters.globals.mt_en |= threadMask;

	      ret = writeRegisters(i, 48,
				   &mainRegisters.globals.mt_en, 1);
	    }
	}
    }
}

static void
set_thread (int th)
{
  unsigned int thread_num;

  thread_num = th;

  if(th > 0)
    {
      ubicom32_current_thread = thread_num;
      currentThread = ubicom32_current_thread -1;
    }
}

static int
ubicom32_thread_alive (ptid_t ptid)
{
  return (1);
}

#define MAGIC_NULL_PID 42000

/* ubicom32_open
   Usage: target remote <ip_addr>:<ip_port>
   Connects to the dongle and then connects to target through the dongle.  */
static void
ubicom32_open (char *args, int from_tty)
{
  int ret, i;
  struct gdbarch_info info;
  extern void gdbarch_info_init (struct gdbarch_info *info);
  char *env_dongle;

  ubicom32_current_thread = 1;
  if (remote_debug)
    {
      if (args)
	printf_filtered ("ubicom32_open (%s)\n", args);
      else
	printf_filtered ("ubicom32_open ()\n");
    }

  target_preopen (from_tty);

  //printf("Current arch is %s\n", TARGET_ARCHITECTURE->printable_name);

  /* Default device.  */
  if (!args)
    {
      env_dongle = getenv("UBICOM_DONGLE");
      if (env_dongle)
	args = env_dongle;
      else
	args = "localhost:5010";
    }

  /* Connect to dongle.  */
  if (ubicom32isp_connect (args) != 0)
    error ("Unable to find dongle.");

  /*
   * Turn off the 2 wire interface if possible.
   */
  ret = turnOff2wire();


  /* Attach to target.  */
  ret = ubicom32isp_attach ();
  if(ret)
    {
      (void)ubicom32isp_close();
      remote_ubicom32_state = UBICOM32_DETACHED;
      switch(ret)
	{
	case -1:
	  error("Error in reply packets from dongle\n");
	  break;
	case -2:
	  error("Unable to establish ISD connection\n");
	  break;
	case -3:
	  error("Can't attach to debug kernel\n");
	  break;
	case -4:
	  {
	    extern void ubicom32v3_open(char *, int);
	    ubicom32v3_open(args, from_tty);
	    return;
	  }
	  break;
	default:
	  error("Unable to connect to target\n");
	  break;
	}
    }

  if (current_gdbarch == NULL)
    {
      gdbarch_info_init (&info);
      info.bfd_arch_info = bfd_scan_arch ("ubicom32");

      if (info.bfd_arch_info == NULL)
	internal_error (__FILE__, __LINE__,
			"set_architecture: bfd_scan_arch failed");
      (void)gdbarch_update_p (info);
    }
  else
    {
      struct gdbarch_tdep *tdep = gdbarch_tdep(current_gdbarch);
      const char *arch_string = tdep->name;
      if (strcmp(arch_string, "Ubicom32 Version 2"))
	{
	  /* Architecture mismatch. We need to go find the "ubicom32dsp" arch. */
	  gdbarch_info_init (&info);
	  info.bfd_arch_info = bfd_scan_arch ("ubicom32");

	  if (info.bfd_arch_info == NULL)
	    internal_error (__FILE__, __LINE__,
			    "set_architecture: bfd_scan_arch failed");
	  (void)gdbarch_update_p (info);
	}
    }

  push_target (&ubicom32_ops);		/* Switch to using remote target now */

  remote_ubicom32_state = UBICOM32_STOPPED;
  inferior_ptid = pid_to_ptid (ubicom32_current_thread);
  start_remote(from_tty);
  //inferior_ptid = pid_to_ptid (1);

  /* add the threads to the system */
  for(i=1; i<= 8; i++)
    add_thread(pid_to_ptid(i));

  set_thread(currentThread +1);
}

struct ubicom32_core_memory {
  unsigned int data_size;
  unsigned char *data_mem;
  unsigned int pram_size;
  unsigned char *pram_mem;
  unsigned int sdram_size;
  unsigned char *sdram_mem;
  unsigned int flash_size;
  unsigned char *flash_mem;
  unsigned char *hrts;
} ubicom32_core;


/* ubicom32_close.  */
static void
ubicom32_core_close (int quitting)
{
  if (remote_debug)
    printf_filtered ("ubicom32_core_close (%d)\n", quitting);

  /* Disconnect.  */
  if(ubicom32_core.data_mem)
    free(ubicom32_core.data_mem);
  ubicom32_core.data_size = 0;
  ubicom32_core.data_mem = NULL;

  if(ubicom32_core.pram_mem)
    free(ubicom32_core.pram_mem);
  ubicom32_core.pram_size = 0;
  ubicom32_core.pram_mem = NULL;

  if(ubicom32_core.sdram_mem)
    free(ubicom32_core.sdram_mem);
  ubicom32_core.sdram_size = 0;
  ubicom32_core.sdram_mem = NULL;

  if(ubicom32_core.flash_mem)
    free(ubicom32_core.flash_mem);
  ubicom32_core.flash_size = 0;
  ubicom32_core.flash_mem = NULL;

  if(ubicom32_core.hrts)
    free(ubicom32_core.hrts);
  ubicom32_core.hrts = NULL;

  remote_ubicom32_state = UBICOM32_DETACHED;
}


/* ubicom32_fetch_register.  */
static void
ubicom32_fetch_register (struct regcache *regcache, int regno)
{
  int i, ret;
  int thread_num = PIDGET (inferior_ptid);
  int perThreadRegs = sizeof(ubicom32MainPerThreadRegs_t)/4;

  int *rptr = (int *)&mainRegisters.tRegs[thread_num-1];
  set_thread (thread_num);

  /* grab main processor registers */
  ret = gdbMainGetallRegs((thread_num-1), &mainRegisters);
  if(ret)
    {
      printf_unfiltered("error: Main processor fetch registers for thread %d failed\n", thread_num-1);
    }

  if(ret)
    return;

  /* we are supplying main processor registers */
  /* first supply the thread specific registers */
  for(i=0; i< perThreadRegs; i++)
    swizzled_supply_register(regcache, i, rptr++);

  /* Now supply the global registers */
  perThreadRegs += (sizeof(ubicom32MainGlobalRegs_t)/4);
  rptr = (int *)&mainRegisters.globals;
  for(;i< perThreadRegs; i++)
    swizzled_supply_register(regcache, i, rptr++);
}

struct ubicom32_core_header {
  unsigned int magic;
  unsigned int regOffset;
  unsigned int hrtOffset;
  unsigned int dataOffset;
  unsigned int pramOffset;
  unsigned int sdramOffset;
  unsigned int flashOffset;
};


/* ubicom32_core_open
   Usage: ubicom32-elf-gdb elf-filename core-fliename. */
static void
ubicom32_core_open (char *args, int from_tty)
{
  int ret, i;
  FILE *fp;
  struct ubicom32_core_header coreHeader;
  char *file;
  bfd *loadfile_bfd;
  int registerSize;
  asection *s;
  CORE_ADDR dyn_begin;
  unsigned int dyn_length;

  struct gdbarch_info info;
  extern void gdbarch_info_init (struct gdbarch_info *info);

  ubicom32_current_thread = 1;
  if (remote_debug)
    {
      if (args)
	printf_filtered ("ubicom32_core_open (%s)\n", args);
      else
	printf_filtered ("ubicom32_core_open ()\n");
    }

  target_preopen (from_tty);

  remote_ubicom32_state = UBICOM32_DETACHED;

  /* Open and read in the core file. */
  fp = fopen(args, "rb");

  if(fp == NULL)
    {
      printf_unfiltered("Unable to open core file %s\n", args);
      return;
    }

  /* go retreive the core header from the file */
  (void) fread(&coreHeader, 1, sizeof(coreHeader), fp);

  if(coreHeader.magic != 0x1234aa55)
    {
      /*
       * If it is a 5k corefile let the 5k functions deal with it and we are done.
       */

      if(coreHeader.magic == 0x123455aa || coreHeader.magic == 0x123455ab)
	{
	  extern void ubicom32v3_core_open(char *args, int from_tty);

	  fclose(fp);
	  /*
	   * 5k core file the the 5k code deal with it.
	   */
	  //ubicom32v3_core_open(args, from_tty);
	  return;
	}
      printf_unfiltered("%s is not a Ubicom core file. Incorrect magic number.\n", args);
      fclose(fp);
      return;
    }

  printf_unfiltered("Reading machine state from %s\n", args);

  /* read in the debuggerThreadNo */
  (void) fread(&debuggerThreadNo, 1, 4, fp);

  /* read in the flashSecorSize */
  (void) fread(&flashSectorSize, 1, 4, fp);

  /* read in the flashSize */
  (void) fread(&flashSize, 1, 4, fp);

  /* go retrieve the flash contents from the bfd file */
  file = get_exec_file(1);

  loadfile_bfd = bfd_openr (file, gnutarget);

  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  /* malloc space for flash in the core representation */
  ubicom32_core.flash_size = flashSize;
  ubicom32_core.flash_mem = malloc(ubicom32_core.flash_size);
  if(ubicom32_core.flash_mem == NULL)
    {
      printf_unfiltered("Could not allocate space to represent flash\n");
      ubicom32_core_close(1);
      return;
    }

  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32_loadable_section (loadfile_bfd, s) != UBICOM32_FLASH_SECTION)
	continue;
      else
	{
	  int length = bfd_get_section_size (s);
	  int addr = s->lma;
	  addr -= 0x20000000;
	  bfd_get_section_contents (loadfile_bfd, s, &ubicom32_core.flash_mem[addr], 0, length);
	}
    }

  /* read in size of the register area */
  (void) fread(&registerSize, 1, 4, fp);

  /* read in register data from the core */
  (void) fread(&mainRegisters, 1, sizeof(mainRegisters), fp);

  /* malloc space for hrts and read in the data */
  ubicom32_core.hrts = malloc(512);
  if(ubicom32_core.hrts == NULL)
    {
      printf_unfiltered("Could not allocate space to represent hrts\n");
      ubicom32_core_close(1);
      return;
    }

  /* read in hrt data */
  (void) fread(ubicom32_core.hrts, 1, 512, fp);

  /*read data memory size */
  (void) fread(&ubicom32_core.data_size, 1, 4, fp);

  /* malloc space for it */
  ubicom32_core.data_mem = malloc(ubicom32_core.data_size);
  if(ubicom32_core.data_mem == NULL)
    {
      printf_unfiltered("Could not allocate space to represent data memory\n");
      ubicom32_core_close(1);
      return;
    }

  /*read data memory */
  (void) fread(ubicom32_core.data_mem, 1, ubicom32_core.data_size, fp);

  /*read pram memory size */
  (void) fread(&ubicom32_core.pram_size, 1, 4, fp);

  /* malloc space for it */
  ubicom32_core.pram_mem = malloc(ubicom32_core.pram_size);
  if(ubicom32_core.pram_mem == NULL)
    {
      printf_unfiltered("Could not allocate space to represent pram memory\n");
      ubicom32_core_close(1);
      return;
    }

  /*read pram memory */
  (void) fread(ubicom32_core.pram_mem, 1, ubicom32_core.pram_size, fp);

  /*read sdram memory size */
  (void) fread(&ubicom32_core.sdram_size, 1, 4, fp);

  /* malloc space for it */
  ubicom32_core.sdram_mem = malloc(ubicom32_core.sdram_size);
  if(ubicom32_core.sdram_mem == NULL)
    {
      printf_unfiltered("Could not allocate space to represent sdram memory\n");
      ubicom32_core_close(1);
      return;
    }

  /*read sdram memory */
  (void) fread(ubicom32_core.sdram_mem, 1, ubicom32_core.sdram_size, fp);

  /* reload the dynamic_non_volatile_block */
  /* read the dynamic block begin address */
  (void) fread(&dyn_begin, 1, 4, fp);

  /* read in the dynamic block length */
  (void) fread(&dyn_length, 1, 4, fp);

  /*read int the block */
  dyn_begin -= 0x20000000;

  (void)fread(&ubicom32_core.flash_mem[dyn_begin], 1, dyn_length, fp);

  fclose(fp);

  if (current_gdbarch == NULL)
    {
      gdbarch_info_init (&info);
      info.bfd_arch_info = bfd_scan_arch ("ubicom32");

      if (info.bfd_arch_info == NULL)
	internal_error (__FILE__, __LINE__,
			"set_architecture: bfd_scan_arch failed");
      (void)gdbarch_update_p (info);
    }
  else
    {
      struct gdbarch_tdep *tdep = gdbarch_tdep(current_gdbarch);
      char *arch_string = (char *)tdep->name;
      if (strcmp(arch_string, "Ubicom32 Version 2"))
	{
	  /* Architecture mismatch. We need to go find the "ubicom32dsp" arch. */
	  gdbarch_info_init (&info);
	  info.bfd_arch_info = bfd_scan_arch ("ubicom32");

	  if (info.bfd_arch_info == NULL)
	    internal_error (__FILE__, __LINE__,
			    "set_architecture: bfd_scan_arch failed");
	  (void)gdbarch_update_p (info);
	}
    }

  push_target (&ubicom32_core_ops);		/* Switch to using remote target now */

  remote_ubicom32_state = UBICOM32_STOPPED;
  inferior_ptid = pid_to_ptid (ubicom32_current_thread);
  start_remote(from_tty);

  /* add the threads to the system */
  currentThread = 0;
  for(i=1; i<= 8; i++)
    add_thread(pid_to_ptid(i));

  set_thread(currentThread +1);

#if 0
  set_thread(1);
  inferior_ptid = pid_to_ptid (1);
  ubicom32_fetch_register(0);
  select_frame(get_current_frame());
  normal_stop();
#endif
}

/* ubicom32_attach */
static void
ubicom32_attach (char *args, int from_tty)
{
  struct gdbarch_info info;
  extern void gdbarch_info_init (struct gdbarch_info *info);
  int ret = ubicom32isp_attach ();

  if(ret)
    {
      switch(ret)
	{
	case -1:
	  error("Unable to communicate to dongle. Error in reply packets from dongel\n");
	  break;
	case -2:
	  error("Unable to establish ISD connection\n");
	  break;
	case -3:
	  error("Unable to attach to debug kernel\n");
	  break;
	default:
	  error("Unable to connect to target\n");
	  break;
	}
    }
  remote_ubicom32_state = UBICOM32_STOPPED;
  set_thread(currentThread +1);

  if (current_gdbarch == NULL)
    {
      info.bfd_arch_info = bfd_scan_arch ("ubicom32");

      if (info.bfd_arch_info == NULL)
	internal_error (__FILE__, __LINE__,
			"set_architecture: bfd_scan_arch failed");
      (void)gdbarch_update_p (info);
    }
  else
    {
      struct gdbarch_tdep *tdep = gdbarch_tdep(current_gdbarch);
      char *arch_string = (char *)tdep->name;
      if (strcmp(arch_string, "Ubicom32 Version 2"))
	{
	  /* Architecture mismatch. We need to go find the "ubicom32dsp" arch. */
	  gdbarch_info_init (&info);
	  info.bfd_arch_info = bfd_scan_arch ("ubicom32");

	  if (info.bfd_arch_info == NULL)
	    internal_error (__FILE__, __LINE__,
			    "set_architecture: bfd_scan_arch failed");
	  (void)gdbarch_update_p (info);
	}
    }
}

/* ubicom32_core_detach.
   Detach from core.  */
static void
ubicom32_core_detach (char *args, int from_tty)
{
  (void) ubicom32_core_close(1);
  pop_target();
}

/* ubicom32_detach.
   Detach from the remote board.  */
static void
ubicom32_detach (char *args, int from_tty)
{
  int ret;
  FBlock_t *fBlock = firstFlashBlock;

  if (remote_debug)
    printf_filtered ("ubicom32_detach\n");

  /* walk down the flash blocks and ship them out */
  while(fBlock)
    {
      if(fBlock->numRemoveBPs|| fBlock->flashState == 0)
	{
	  /* Time to ship out this block */
	  ret = ubicom32WriteFlashBlock(fBlock);
	}

      fBlock = fBlock->next;
    }

  fBlock = firstFlashBlock;

  /* walk down the flash block and release everything */
  while(fBlock)
    {
      FBlock_t *next = fBlock->next;
      /* free the breakpointList */
      free(fBlock->breakpointList);

      /* free the breakpointRemoveList */
      free(fBlock->breakpointRemoveList);

      /* free the code block */
      free(fBlock->code);

      /* free the fBlock itself */
      free(fBlock);

      fBlock = next;
    }

  firstFlashBlock = lastFlashBlock = NULL;

  toggleMtEn();

  (void) ubicom32isp_detach();
  (void) ubicom32isp_close();
  remote_ubicom32_state = UBICOM32_DETACHED;
  pop_target();

}

/* ubicom32_close.  */
static void
ubicom32_close (int quitting)
{
  if(remote_ubicom32_state == UBICOM32_DETACHED)
    return;

  if (remote_debug)
    printf_filtered ("ubicom32_close (%d)\n", quitting);

  /* Disconnect.  */
  (void)ubicom32isp_close();
  remote_ubicom32_state = UBICOM32_DETACHED;
}

/* ubicom32_cntrl_c.  */
static void
ubicom32_cntrl_c (int signo)
{
  ubicom32_cntrl_c_abort = 1;
}

/* ubicom32_files_info.  */
static void
ubicom32_files_info (struct target_ops *target)
{
  if (exec_bfd)
    {
      char *file = bfd_get_filename (exec_bfd);
      printf_unfiltered ("Debugging %s\n", file);
    }
  else
    printf_unfiltered ("No file loaded\n");
}

/* ubicom32_stop.
   Notify the target of an asynchronous request to stop.  */
static void
ubicom32_stop (void)
{
  if (remote_debug)
    printf_filtered ("ubicom32_stop\n");

  ubicom32_cntrl_c_abort = 1;
}

static char *
ubicom32_thread_pid_to_str (ptid_t ptid)
{
  static char buf[30];

  sprintf (buf, "Thread %d", (PIDGET (ptid))-1);
  return buf;
}

static void
ubicom32_store_register(struct regcache *regcache, int regnum)
{
  char *regs = malloc(REGISTER_RAW_SIZE(regnum));
  int thread_num = PIDGET (inferior_ptid);
  int ret;

  set_thread(thread_num);

  if(regnum >= 0)
    {
      /* pull the data from regcache */
      regcache_raw_collect (regcache, regnum, regs);

      swizzle_for_write_register (regnum, regs);
      ret = writeRegisters((thread_num-1), regnum, (int *) regs, 1);
      if(ret)
	{
	  printf_unfiltered("error: Write to register %d in thread %d failed\n",
			    regnum, thread_num-1);
	}
    }
  else
    {
      printf_unfiltered("error: Write of all registers not supported\n");
    }
}

/* Prepare to store registers.  Since we may send them all
   we have to read out the ones we don't want to change
   first.  */

static void
ubicom32_prepare_to_store (struct regcache *regcache)
{
  int i;
  gdb_byte buf[MAX_REGISTER_SIZE];

  /* Make sure all the necessary registers are cached.  */
  for (i = 0; i < gdbarch_num_regs(current_gdbarch); i++)
    regcache_raw_read (regcache, i, buf);
}

#if 0
/* ubicom32_core_wait */
static ptid_t
ubicom32_core_wait (ptid_t ptid, struct target_waitstatus *status)
{
  return pid_to_ptid (ubicom32_current_thread);
}
#endif

/* ubicom32_wait */
static ptid_t
ubicom32_wait (ptid_t ptid, struct target_waitstatus *status)
{
  unsigned int mpTpacketRegs[10];
  bpReason_t reason;
  struct regcache *current_regcache = get_current_regcache ();

  int mask;

  static RETSIGTYPE (*prev_sigint) ();
  int ret;

  if (remote_debug)
    printf_filtered ("ubicom32_wait\n");

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  if(remote_ubicom32_state == UBICOM32_STEPPING_PROBLEMS)
    {
      /* single stepping was attempted on a suspended thread */
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_STOP;
      remote_ubicom32_state = UBICOM32_STOPPED;
      return pid_to_ptid (ubicom32_current_thread);
    }
  else if(remote_ubicom32_state == UBICOM32_STOPPED)
    {
      /* Things are alread stopped */
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_TRAP;
      return pid_to_ptid (ubicom32_current_thread);
    }
  else if(remote_ubicom32_state == UBICOM32_STEP_INTERPRET_CALL)
    {
      /* set the PC to target_pc */
      mask = 1 << currentThread;
      writeRegisters(stepinterpret.stepping_thread, 34,
		     &stepinterpret.target1_pc, 1);

      stepinterpret.current_pc += 4;
      writeRegisters(stepinterpret.stepping_thread, (16 + stepinterpret.address_register),
		     &stepinterpret.current_pc, 1);

      ret = createTpacketMP((unsigned char *)mpTpacketRegs, ubicom32_current_thread-1);

      if(ret)
	{
	  printf_unfiltered("error: Could not retrieve registers after stopping the processor\n");
	  signal (SIGINT, prev_sigint);
	  return null_ptid;
	}

      swizzled_supply_register(current_regcache, 0, &mainRegisters.tRegs[currentThread].dr[0]);
      swizzled_supply_register(current_regcache, 21, &mainRegisters.tRegs[currentThread].ar[5]);
      swizzled_supply_register(current_regcache, 22, &mainRegisters.tRegs[currentThread].ar[6]);
      swizzled_supply_register(current_regcache, 23, &mainRegisters.tRegs[currentThread].ar[7]);
      swizzled_supply_register(current_regcache, 30, &mainRegisters.tRegs[currentThread].rosr);
      swizzled_supply_register(current_regcache, 34, &mainRegisters.tRegs[currentThread].threadPc);
      swizzled_supply_register(current_regcache, 45, &mainRegisters.globals.mt_active);
      swizzled_supply_register(current_regcache, 47, &mainRegisters.globals.mt_dbg_active);
      swizzled_supply_register(current_regcache, 48, &mainRegisters.globals.mt_en);

      /* We have to toggle the mt_en bit to off */
      mainRegisters.globals.mt_en &= ~mask;

      ret = writeRegisters(currentThread, 48,
			   &mainRegisters.globals.mt_en, 1);

      /* turn mt_en for this thread back to on */
      mainRegisters.globals.mt_en |= mask;

      ret = writeRegisters(currentThread, 48,
			   &mainRegisters.globals.mt_en, 1);

      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_TRAP;
      return pid_to_ptid (ubicom32_current_thread);
    }

  ubicom32_cntrl_c_abort = 0;
  prev_sigint = signal (SIGINT, ubicom32_cntrl_c);
  while (1)
    {
      if (ubicom32_cntrl_c_abort)
	{
	  ret = stopProcessor();
	  if(ret)
	    {
	      printf_unfiltered("error: Could not stop the processor\n");
	      signal (SIGINT, prev_sigint);
	      return null_ptid;
	    }
	  reason.reason = BREAKPOINT_HIT;
	  set_thread(currentThread +1);
	  break;
	}
      else
	{
	  /* call waitForBP and see if we get anything */
	  ret = waitForBP(&reason);

	  if(ret< 0)
	    {
	      printf_unfiltered ("error: unable to read status\n");
	      signal (SIGINT, prev_sigint);
	      return null_ptid;
	    }
	  if(ret == 0)
	    {
	      /* wait for 10 ms before trying again */
	      usleep(10000);
	      continue;
	    }

	  set_thread(ret);

	  break;
	}
    }

  /* go retrieve the registers for a TPacket */
  if(ubicom32_current_thread < 33)
    {
      int redoTpacket = 1;
      ret = createTpacketMP((unsigned char *)mpTpacketRegs, ubicom32_current_thread-1);

      if(ret)
	{
	  printf_unfiltered("error: Could not retrieve registers after stopping the processor\n");
	  signal (SIGINT, prev_sigint);
	  return null_ptid;
	}

      switch(remote_ubicom32_state)
	{
	case UBICOM32_STEP_INTERPRET_JMP:
	  {
	    if(mainRegisters.tRegs[currentThread].threadPc ==
	       single_step_buffer_address + 4)
	      {
		/* the real PC is target1_pc */
		writeRegisters(stepinterpret.stepping_thread, 34, &stepinterpret.target1_pc, 1);
	      }
	    else
	      {
		/* the real PC is target2_pc */
		writeRegisters(stepinterpret.stepping_thread, 34, &stepinterpret.target2_pc, 1);
	      }

	    break;
	  }
	case UBICOM32_STEP_INTERPRET_CALLI:
	  {
	    /* Need to fix the address register */
	    stepinterpret.current_pc += 4;
	    writeRegisters(stepinterpret.stepping_thread, (16 + stepinterpret.address_register),
			   &stepinterpret.current_pc, 1);

	    break;
	  }
	case UBICOM32_STEP_INTERPRET_REST:
	  {
	    /* Need to fix the PC */
	    stepinterpret.current_pc += 4;
	    writeRegisters(stepinterpret.stepping_thread, 34,
			   &stepinterpret.current_pc, 1);
	    break;
	  }
	default:
	  redoTpacket = 0;
	}


      if(redoTpacket)
	{
	  mask = 1 <<(ubicom32_current_thread - 1);
	  ret = createTpacketMP((unsigned char *)mpTpacketRegs, ubicom32_current_thread-1);

	  if(ret)
	    {
	      printf_unfiltered("error: Could not retrieve registers after stopping the processor\n");
	      signal (SIGINT, prev_sigint);
	      return null_ptid;
	    }

	  /* We have to toggle the mt_en bit to off */
	  mainRegisters.globals.mt_en &= ~mask;

	  ret = writeRegisters(ubicom32_current_thread-1, 48,
			       &mainRegisters.globals.mt_en, 1);

	  /* turn mt_en for this thread back to on */
	  mainRegisters.globals.mt_en |= mask;

	  ret = writeRegisters(ubicom32_current_thread-1, 48,
			       &mainRegisters.globals.mt_en, 1);
	}

      swizzled_supply_register(current_regcache, 0, &mainRegisters.tRegs[currentThread].dr[0]);
      swizzled_supply_register(current_regcache, 21, &mainRegisters.tRegs[currentThread].ar[5]);
      swizzled_supply_register(current_regcache, 22, &mainRegisters.tRegs[currentThread].ar[6]);
      swizzled_supply_register(current_regcache, 23, &mainRegisters.tRegs[currentThread].ar[7]);
      swizzled_supply_register(current_regcache, 30, &mainRegisters.tRegs[currentThread].rosr);
      swizzled_supply_register(current_regcache, 34, &mainRegisters.tRegs[currentThread].threadPc);
      swizzled_supply_register(current_regcache, 45, &mainRegisters.globals.mt_active);
      swizzled_supply_register(current_regcache, 47, &mainRegisters.globals.mt_dbg_active);
      swizzled_supply_register(current_regcache, 48, &mainRegisters.globals.mt_en);

      switch(reason.reason)
	{
	case DCAPT_ERROR:
	  {
	    unsigned tnum = reason.status & 0x1f;

	    printf_unfiltered("DCAPT error in thread %d at address 0x%08x\n", tnum, reason.addr);

	    if (reason.status & (1 << 5)) {
	      printf_unfiltered("DCAPT cause: write watchpoint triggered\n");
	    }
	    else if (reason.status & (1 << 6)) {
	      printf_unfiltered("DCAPT cause: source 1 out of range\n");
	    }
	    else if (reason.status & (1 << 7)) {
	      printf_unfiltered("DCAPT cause: destination out of range\n");
	    }
	    else if (reason.status & (1 << 8)) {
	      printf_unfiltered("DCAPT cause: source 1 alignment error\n");
	    }
	    else if (reason.status & (1 << 9)) {
	      printf_unfiltered("DCAPT cause: destination alignment error\n");
	    }
	    else {
	      printf_unfiltered("DCAPT cause: unknown error\n");
	    }
	    status->kind = TARGET_WAITKIND_STOPPED;
	    status->value.sig = TARGET_SIGNAL_STOP;
	    remote_ubicom32_state = UBICOM32_DCAPT_ERROR;
	  }
	  break;

	case PARITY_ERROR:
	  printf_unfiltered("PARITY error at address 0x%08x\n", reason.addr);
	  status->kind = TARGET_WAITKIND_STOPPED;
	  status->value.sig = TARGET_SIGNAL_STOP;
	  remote_ubicom32_state = UBICOM32_PARITY_ERROR;
	  break;

	default:
	  status->kind = TARGET_WAITKIND_STOPPED;
	  status->value.sig = TARGET_SIGNAL_TRAP;
	  remote_ubicom32_state = UBICOM32_STOPPED;
	}
    }
  signal (SIGINT, prev_sigint);

  return pid_to_ptid (ubicom32_current_thread);
}

/* Resume execution of the target process.  STEP says whether to single-step
   or to run free; SIGGNAL is the signal value (e.g. SIGINT) to be given
   to the target, or zero for no signal.  */
static void
ubicom32_resume (ptid_t ptid, int step, enum target_signal siggnal)
{
  int thread_num = ptid_get_pid (inferior_ptid);
  FBlock_t *fBlock = firstFlashBlock;
  int mask, i;
  CORE_ADDR pc;

  set_thread(thread_num);
  thread_num--;
  mask = 1<< currentThread;

  if (remote_debug)
    printf_filtered ("ubicom32_resume\n");

  pc = read_pc();

 //inferior_ptid = ptid;

  if (step)
    {
      int ret;
      thread_num = PIDGET (inferior_ptid);

      set_thread(thread_num);

      /* we are single stepping the main processor */
      if(mainRegisters.globals.mt_dbg_active&mask)
	{
	  printf_unfiltered("error: Single Stepping an Uncontrolled thread.\n");
	  remote_ubicom32_state = UBICOM32_STEPPING_PROBLEMS;
	}
      else if(!(mainRegisters.globals.mt_en & mask))
	{
	  // Tryingn to single step a Disabled thread
	  printf_unfiltered("error: Single Stepping a Disabled thread.\n");
	  remote_ubicom32_state = UBICOM32_STEPPING_PROBLEMS;
	}
      else if (!(mainRegisters.globals.mt_active & mask))
	{
	  printf_unfiltered("error: Single Stepping a Suspended thread.\n");
	  remote_ubicom32_state = UBICOM32_STEPPING_PROBLEMS;
	}
      if(remote_ubicom32_state == UBICOM32_STEPPING_PROBLEMS)
	return;

      if(0x20000000 <= pc && pc < 0x20000000 + flashSize)
	{
	  /* This is a flash address */
	  unsigned int blockNo = pc & ~(flashSectorSize-1);

	  while(fBlock)
	    {
	      if(fBlock->blockNumber == blockNo)
		break;

	      fBlock = fBlock->next;
	    }

	  if(fBlock)
	    {
	      unsigned int i;
	      /* got a hit. Now search the removed BP list */
	      for(i=0; i< fBlock->numRemoveBPs; i++)
		if(fBlock->breakpointRemoveList[i] == (unsigned int)pc)
		  break;

	      if(i < fBlock->numRemoveBPs)
		{
		  if(single_step_buffer_address)
		    {
		      unsigned char *cptr  =
			(char *) &fBlock->code[(fBlock->breakpointRemoveList[i] & (flashSectorSize -1))/sizeof(unsigned int)];

		      unsigned int insn = cptr[0] <<24 | cptr[1] << 16 | cptr[2] << 8 | cptr[3];

		      unsigned char base_opcode = (cptr[0] >> 3) & 0x1f;
		      stepinterpret.current_pc = (unsigned int )pc;
		      stepinterpret.stepping_thread = currentThread;

		      /* analyze the instruction */
		      switch(base_opcode)
			{
			case 0x1a:
			  {
			    /* jump conditionals */
			    int offset = (insn & 0x1fffff)<<2;
			    ;
			    if (insn & 0x00400000)
			      {
				/* set up a negative number */
				offset |= 0xff800000;
			      }
			    stepinterpret.target1_pc = stepinterpret.current_pc+4;
			    stepinterpret.target2_pc = stepinterpret.current_pc+offset;
			    remote_ubicom32_state = UBICOM32_STEP_INTERPRET_JMP;
			    break;
			  }
			case 0x1b:
			  {
			    /* CALL */
			    int offset = ((int)(cptr[0] & 0x7))<<23 | ((insn & 0x1fffff)<<2);
			    if(offset & 0x02000000)
			      {
				offset |= 0xfc000000;
			      }
			    stepinterpret.target1_pc = stepinterpret.current_pc+offset;
			    stepinterpret.address_register = (cptr[1] >> 5) & 0x7;
			    remote_ubicom32_state = UBICOM32_STEP_INTERPRET_CALL;
			    break;
			  }
			case 0x1e:
			  {
			    /* CALLI */
			    stepinterpret.target1_pc = 0;
			    stepinterpret.address_register = (cptr[1] >> 5) & 0x7;
			    remote_ubicom32_state = UBICOM32_STEP_INTERPRET_CALLI;
			    break;
			  }
			default:
			  {
			    /* everything else */
			    stepinterpret.target1_pc = 0;
			    stepinterpret.address_register = 0;
			    remote_ubicom32_state = UBICOM32_STEP_INTERPRET_REST;
			    break;
			  }
			}

		      if(remote_ubicom32_state != UBICOM32_STEP_INTERPRET_CALL)
			{
			  /* load the single step buffer with this instruction */
			  (void) ubicom32_write_bytes(single_step_buffer_address, cptr, 4);

			  /* we have to change this threads pc to point to the single step buffer */
			  ret = writeRegisters(currentThread, 34,
					       &single_step_buffer_address, 1);

			  /* We have to toggle the mt_en bit to off */
			  mainRegisters.globals.mt_en &= ~mask;

			  ret = writeRegisters(thread_num, 48,
					       &mainRegisters.globals.mt_en, 1);

			  /* turn mt_en for this thread back to on */
			  mainRegisters.globals.mt_en |= mask;

			  ret = writeRegisters(thread_num, 48,
					       &mainRegisters.globals.mt_en, 1);

			  /* force the single step */
			  ret = singleStep(currentThread);
			}
		      return;
		    }

		  /* We have to update the flash so step can happen */
		  /* Time to ship out this block */
		  ret = ubicom32WriteFlashBlock(fBlock);

		  fBlock = firstFlashBlock;

		      /* We have to toggle the mt_en bit to off */
		  mainRegisters.globals.mt_en &= ~mask;

		  ret = writeRegisters(thread_num, 48,
				       &mainRegisters.globals.mt_en, 1);

		  /* turn mt_en for this thread back to on */
		  mainRegisters.globals.mt_en |= mask;

		  ret = writeRegisters(thread_num, 48,
				       &mainRegisters.globals.mt_en, 1);
		}
	    }
	}

      ret = singleStep(currentThread);

      if(remote_ubicom32_state == UBICOM32_STOPPED)
	remote_ubicom32_state = UBICOM32_STEPPING;
    }
  else
    {
      /* let everything run */
      int ret;
      unsigned int blockNo = pc & ~(flashSectorSize-1);

      /* scan the flash blocks and see if any has to be shipped down */
      /* walk down the flash blocks and ship them out */
      while(fBlock)
	{
	  /* write out the block */
	  if(fBlock->numRemoveBPs|| fBlock->flashState == 0)
	    {
	      /* Time to ship out this block */
	      ret = ubicom32WriteFlashBlock(fBlock);
	    }

	  fBlock = fBlock->next;
	}

      toggleMtEn();

      ret= restartProcessor();
      remote_ubicom32_state = UBICOM32_RUNNING;
    }
}


static int
ubicom32_read_bytes (CORE_ADDR address, char *buffer, int length)
{
  // split the transfer into lead, aligned middle and end
  unsigned truncBytes = (address & 0x3);
  unsigned truncAddress = address & ~0x3;
  unsigned int leadBytes, leadAddr, midBytes, midAddr, endBytes, endAddr, leadIndex;
  unsigned toReadWords;
  unsigned char  *recvBuffer;
  unsigned char *freeBuffer;
  unsigned char *cptr;
  int i, j, ret;
  FBlock_t *fBlock = firstFlashBlock;

  leadIndex = 0;
  if(truncBytes)
    {
      leadBytes = 4-truncBytes;
      if(leadBytes > length)
	leadBytes = length;
      leadIndex = 3-truncBytes;
      length -= leadBytes;
      address = truncAddress + 4;
    }
  else
    leadBytes =0;

  leadAddr = truncAddress;

  endAddr = address + length;
  endBytes = endAddr & 0x3;
  endAddr &= ~0x3;

  midBytes = length - endBytes;
  midAddr = address;

  toReadWords = midBytes/4;

  if(leadBytes)
    toReadWords++;

  if(endBytes)
    toReadWords++;

  /* Allocate the recv buffer */
  recvBuffer = (unsigned char *)malloc(toReadWords*4);
  freeBuffer = recvBuffer;
  cptr = recvBuffer;

  // Read the words
  if(leadAddr < 0x20000000 || leadAddr >= 0x60000000)
    {
      // a data section read
      ret = readDataMemory(leadAddr, toReadWords, (int *)recvBuffer);
    }
  else
    {
      // a code section read
      ret = readPgmMemory(leadAddr, toReadWords, (int *)recvBuffer);
    }

  /* walk down the fblocks to see if this data is sitting in the removeBPList */
  while(fBlock && fBlock->numRemoveBPs)
    {
      unsigned int blockStart = leadAddr;
      unsigned int blockEnd = leadAddr + toReadWords *4;
      int i;

      for(i=0; i< fBlock->numRemoveBPs; i++)
	{
	  if(blockStart <= fBlock->breakpointRemoveList[i] &&
	     fBlock->breakpointRemoveList[i] < blockEnd)
	    {
	      /* This location is in the block and we need to replace it with data from the code block */
	      unsigned char *opcode =
		(unsigned char *)&fBlock->code[(fBlock->breakpointRemoveList[i] & (flashSectorSize -1))/sizeof(unsigned int)];
	      unsigned char *fixLocation =
		&recvBuffer[fBlock->breakpointRemoveList[i] - leadAddr];

	      fixLocation[0] = opcode[3];
	      fixLocation[1] = opcode[2];
	      fixLocation[2] = opcode[1];
	      fixLocation[3] = opcode[0];
#if 0

	      memcpy((void *) fixLocation,
		     (void *) &opcode,
		     sizeof(unsigned int));
#endif
	    }
	}

      fBlock = fBlock->next;
    }

  // Now Swizzle the data out
  if(leadBytes)
    {
      for(i= 0;i< leadBytes; i++)
	{
	  *buffer++ = recvBuffer[leadIndex--];
	}
      recvBuffer += 4;
    }

  if(midBytes)
    {
      for(i=0; i< midBytes; i+= 4)
	{
	  for(j=0; j< 4; j++)
	    {
	      *buffer++ = recvBuffer[3-j];
	    }
	  recvBuffer += 4;
	}
    }

  if(endBytes)
    {
      for(i=0; i< endBytes; i++)
	{
	  *buffer ++ = recvBuffer[3-i];
	}
    }
  recvBuffer += 4;
  free (freeBuffer);

  return (leadBytes + midBytes + endBytes);
}

#if 0
static int
ubicom32_verify_bytes (CORE_ADDR address, char *buffer, int length)
{
  // split the transfer into lead, aligned middle and end
  unsigned truncBytes = (address & 0x3);
  unsigned truncAddress = address & ~0x3;
  unsigned int leadBytes, leadAddr, midBytes, midAddr, endBytes, endAddr, leadIndex;
  unsigned toVerifyWords;
  unsigned char cptr[4];
  int i, j, ret;

  leadBytes =0;
  leadIndex = 0;
  if(truncBytes)
    {
      leadBytes = 4-truncBytes;
      if(leadBytes > length)
	leadBytes = length;
      leadIndex = 3-truncBytes;
      length -= leadBytes;
      address = truncAddress + 4;
    }

  leadAddr = truncAddress;

  endAddr = address + length;
  endBytes = endAddr & 0x3;
  endAddr &= ~0x3;

  midBytes = length - endBytes;
  midAddr = address;

  toVerifyWords = midBytes/4;

  /* Allocate the send buffer */
  if(leadBytes)
    {
      // Misaligned start. Deal with it by first backing up the address to nearest
      // Go read 4 bytes from the backed up address
      switch (leadAddr >> 28)
	{
	  case 0x2:
	    // a pram section read
	    ret = readPgmMemory(leadAddr, 1, (int *)cptr);
	    break;
	  default:
	    printf_unfiltered("unknown address: 0x%08x\n", leadAddr);
	}

      for(i=0; i<leadBytes; i++)
	{
	  if (cptr[leadIndex--] != (unsigned char) *buffer++)
	    return -1;
	}
    }

  // Deal with the midsection if any
  if(midBytes)
    {
      if (crcPgmMemory(midAddr, toVerifyWords, (int *)buffer) != 0)
	return -1;
      buffer += midBytes;
    }

  if(endBytes)
    {
      // trailing cruft to deal with
      // Go read 4 bytes from the backed up end address
      switch (endAddr >> 28)
	{
	  case 0x2:
	    ret = readPgmMemory(endAddr, 1, (int *)cptr);
	    break;
	  default:
	    printf_unfiltered("unknown address: 0x%08x\n", endAddr);
	}

      for(i=0; i< endBytes; i++)
	{
	  if (cptr[3-i] != (unsigned char) *buffer++)
	    return -1;
	}
    }

  return (leadBytes + midBytes + endBytes);
}
#endif

addrBounds_t bounds[]={
   {0x800, 0xc00},		/* HRT tables */
   {0x1000, 0x1400},		/* IO ports */
   {0x100000, 0x110000},	/* Data Memory */
   {0x400000, 0x500000},	/* Coprocessor memory */
   {0x20000000, 0x20400000},	/* Off Chip Flash */
   {0x40000000, 0x40080000},	/* On board program memory */
   {0x60000000, 0x61000000},	/* SDRAM */
};

unsigned int numBoundsEntries = sizeof(bounds)/sizeof(addrBounds_t);

/* ARGSUSED */
static int
ubicom32_xfer_memory (CORE_ADDR mem_addr, gdb_byte *buffer, int mem_len,
		    int should_write, struct mem_attrib *attrib,
		    struct target_ops *target)
{
  CORE_ADDR targ_addr;
  int targ_len;
  int res;

  targ_len = mem_len;
  targ_addr = mem_addr;
  if (targ_len <= 0)
    return 0;

  // Check if we got a valid address
  if(targ_addr >= bounds[numBoundsEntries-1].upper)
    {
      // address is too high
      return 0;
    }

  for(res =0; res< numBoundsEntries; res++)
    {
      // test the very lowest address
      if(targ_addr < bounds[res].lower)
	{
	  // invalid address. The start is in the hole
	  return (targ_addr - bounds[res].lower);
	}

      // test if this is a valid address
      if(targ_addr < bounds[res].upper)
	{
	  // Valid range. Do an upper bound check and adjust lenght if needed
	  if(targ_len+targ_addr > bounds[res].upper)
	    targ_len = bounds[res].upper - targ_addr;

	  break;
	}
    }

  if (should_write)
    res = ubicom32_write_bytes (targ_addr, buffer, targ_len);
  else
    res = ubicom32_read_bytes (targ_addr, buffer, targ_len);

  return res;
}

/* ARGSUSED */
static int
ubicom32_core_xfer_memory (CORE_ADDR mem_addr, gdb_byte *buffer, int mem_len,
		    int should_write, struct mem_attrib *attrib,
		    struct target_ops *target)
{
  CORE_ADDR targ_addr;
  int targ_len;
  int res;
  char *src = NULL;
  CORE_ADDR index;
  int switch_res;;

  targ_addr = mem_addr;
  targ_len = mem_len;

  if (targ_len <= 0)
    return 0;

  if(should_write)
    return 0;

  // Check if we got a valid address
  if(targ_addr >= bounds[numBoundsEntries-1].upper)
    {
      // address is too high
      return 0;
    }

  for(res =0; res< numBoundsEntries; res++)
    {
      // test the very lowest address
      if(targ_addr < bounds[res].lower)
	{
	  // invalid address. The start is in the hole
	  return (targ_addr - bounds[res].lower);
	}

      // test if this is a valid address
      if(targ_addr < bounds[res].upper)
	{
	  // Valid range. Do an upper bound check and adjust lenght if needed
	  if(targ_len+targ_addr > bounds[res].upper)
	    targ_len = bounds[res].upper - targ_addr;

	  break;
	}
    }

  src = NULL;
  index = targ_addr -= bounds[res].lower;
  switch_res = res;
  res = targ_len;

  switch(switch_res)
    {
    case 0:
      {
	/* hrts */
	src = ubicom32_core.hrts;
	break;
      }
    case 1:
      {
	/* read to the IO block. We can't do much about it */
	printf_unfiltered("Core has no information on IO block registers.\n");
	memset(buffer, 0, targ_len);
	src = NULL;
	break;
      }
    case 2:
      {
	/* Data Memory */
	src = ubicom32_core.data_mem;
	break;
      }
    case 4:
      {
	/* flash memory */
	src = ubicom32_core.flash_mem;
	break;
      }
    case 5:
      {
	/* Pram */
	src = ubicom32_core.pram_mem;
	break;
      }
    case 6:
      {
	/* Sdram */
	src = ubicom32_core.sdram_mem;
	break;
      }
    default:
      {
	printf_unfiltered("Incorrect address block 0x%lx\n", (targ_addr + bounds[res].lower));
	res = 0;
      }

    }
  if(src)
    {
      char *ptr = &src[index];
      memcpy(buffer, ptr, targ_len);
    }
  return res;
}


/* ubicom32_flash_info */
static void
ubicom32_flash_info (char *args, int from_tty)
{
  int ret;

  printf("Flash Details :- MID = 0x%x Type 0x%x SectorSize 0x%x Size = 0x%x\n",
	 flashMID, flashType, flashSectorSize, flashSize);

}

/* ubicom32_hw_mon */
static void
ubicom32_hw_mon (char *args, int from_tty)
{
  int ret, i;
  unsigned int resBuf[32];
  unsigned int numEntries;
  numEntries = 16;

  ret = hw_monitor(0, 1, 0, 0x80000000, 0x80000000, numEntries, resBuf, args);

  if(ret == -3)
    {
      error("No existing connection.\n Reissue command: hwmon hostname:PORT#\n");
    }
  else if(ret == -4)
    {
      error("Unable to establish connection to %s\n", args);
    }
  else if(ret == -6)
    {
      error("Chip appears to be dead. There are no executing threads\n");
    }

  if(ret)
    return;

  for(i=0; i< numEntries; i++)
    {
      if(debugging_processor == 5133)
	{
	  unsigned int pc = ((resBuf[i] &0x00ffffff)<<2);
	  unsigned int threadNo = (resBuf[i]>>26)&0xf;

	  switch ((resBuf[i] &0x03000000)>>24) {
	  case 1:
	    pc |= 0x3c000000;
	    break;
	  case 2:
	    pc |= 0x40000000;
	    break;
	  case 3:
	    pc |= 0x60000000;
	    break;
	  }

	  if(resBuf[i] & 0x80000000)
	    printf("Thread # %d at pc 0x%08x Valid\n", threadNo, pc);
	  else
	    printf("Thread # %d at pc 0x%08x Cancelled\n", threadNo, pc);
	}
      else
	{
	  unsigned int pc = ((resBuf[i] &0x03000000)<<6)|((resBuf[i] &0x00ffffff)<<2);
	  unsigned int threadNo = (resBuf[i]>>26)&0x7;
	  if(resBuf[i] & 0x80000000)
	    printf("Thread # %d at pc 0x%08x Valid\n", threadNo, pc);
	  else
	    printf("Thread # %d at pc 0x%08x Cancelled\n", threadNo, pc);
	}
    }
}

unsigned int sample_buffer[1000][2];
/* ubicom32_hw_mon_sample */
static void
ubicom32_hw_mon_sample (char *args, int from_tty)
{
  int ret, i;
  unsigned int numEntries;
  unsigned int threadNo;
  unsigned int pc;

  numEntries = 2;

  /* we are going to grab a 1000 samples every 10ms */
  for(i=0; i< 1000; i++) {
    debugOpen();

    ret = hw_monitor(0, 1, 0, 0x80000000, 0x80000000, numEntries, sample_buffer[i], args);

    if(ret == -3)
      {
	error("No existing connection.\n Reissue command: hwmon hostname:PORT#\n");
	return;
      }
    else if(ret == -4)
      {
	error("Unable to establish connection to %s\n", args);
	return;
      }
    else if(ret == -6)
      {
	sample_buffer[i][0] = 0xdeadbeef;
	sample_buffer[i][1] = 0xdeadbeef;
	//error("Chip appears to be dead. There are no executing threads\n");
	continue;
      }

#if 0
    threadNo = (sample_buffer[i][0]>>26)&0x7;
    pc = ((sample_buffer[i][0] &0x03000000)<<6)|((sample_buffer[i][0] &0x00ffffff)<<2);

    if(pc ==0 && ((sample_buffer[i][0] & 0x80000000) ==0))
      i--;
#endif
  }

  for(i=0; i< 1000; i++)
    {
      if(debugging_processor == 5133)
	{
	  pc = ((sample_buffer[i][0] & 0x00ffffff)<<2);
	  threadNo = (sample_buffer[i][0]>>26)&0xf;

	  switch ((sample_buffer[i][0] &0x03000000)>>24)
	    {
	    case 1:
	      pc |= 0x3c000000;
	      break;
	    case 2:
	      pc |= 0x40000000;
	      break;
	    case 3:
	      pc |= 0x60000000;
	      break;
	    }
	}
      else
	{
	  threadNo = (sample_buffer[i][0]>>26)&0x7;
	  pc = ((sample_buffer[i][0] &0x03000000)<<6)|((sample_buffer[i][0] &0x00ffffff)<<2);
	}

      if(sample_buffer[i][0] == 0xdeadbeef) {
	printf("Sample #%d Dead threads\n", i);
	continue;
      } else if(sample_buffer[i][0] & 0x80000000)
	printf("Thread # %d at pc 0x%08x Valid\n", threadNo, pc);
      else
	printf("Thread # %d at pc 0x%08x Cancelled\n", threadNo, pc);

      if(debugging_processor == 5133)
	{
	  pc = ((sample_buffer[i][1] &0x00ffffff)<<2);
	  threadNo = (sample_buffer[i][1]>>26)&0xf;

	  switch ((sample_buffer[i][1] &0x03000000)>>24)
	    {
	    case 1:
	      pc |= 0x3c000000;
	      break;
	    case 2:
	      pc |= 0x40000000;
	      break;
	    case 3:
	      pc |= 0x60000000;
	      break;
	    }
	}
      else
	{
	  threadNo = (sample_buffer[i][1]>>26)&0x7;
	  pc = ((sample_buffer[i][1] &0x03000000)<<6)|((sample_buffer[i][1] &0x00ffffff)<<2);
	}

      if(sample_buffer[i][1] & 0x80000000)
	  printf("Thread # %d at pc 0x%08x Valid\n", threadNo, pc);
      else
	printf("Thread # %d at pc 0x%08x Cancelled\n", threadNo, pc);

    }
}

/* ubicom32_reset.  */
static void
ubicom32_reset (char *args, int from_tty)
{
  int ret;
  if (remote_debug)
    printf_filtered ("ubicom32_reset\n");

  reinit_frame_cache();
  registers_changed();
  ret = ubicom32isp_reset();

  set_thread(1);
  inferior_ptid = pid_to_ptid (1);
  ubicom32_fetch_register(get_current_regcache(), 0);
  select_frame(get_current_frame());
  normal_stop();
}

/* common reset */
void
common_reset(char *args, int from_tty)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep(current_gdbarch);
  char *arch_string = (char *)tdep->name;
  if (!strcmp(arch_string, "Ubicom32 Version 2"))
    {
      ubicom32_reset(args, from_tty);
    }
  else if (!strcmp(arch_string, "Ubicom32 Version 3") || !strcmp(arch_string, "Ubicom32 Version 4") || !strcmp(arch_string, "Ubicom32 Version 5"))
    {
      if(debugging_processor == 5133)
	{
	  extern void ubicom32v3_reset(char *args, int from_tty);
	  ubicom32v3_reset(args, from_tty);
	  return;
	}
    }
  else if(!strcmp(arch_string, "Ubicom32 POSIX"))
    {
      extern void ubicom32_posix_reset(char *args, int from_tty);
      ubicom32_posix_reset(args, from_tty);
      return;
    }
}


/* ubicom32_core_kill.  */
static void
ubicom32_core_kill (void)
{
  if (remote_debug)
      printf_filtered ("ubicom32_core_kill\n");

  inferior_ptid = null_ptid;

  /* Detach from target.  */
  ubicom32_core_detach (NULL, 0);
}

/* ubicom32_kill.  */
static void
ubicom32_kill (void)
{
  if (remote_debug)
      printf_filtered ("ubicom32_kill\n");

  inferior_ptid = null_ptid;

  /* Detach from target.  */
  ubicom32_detach (NULL, 0);
}

/* ubicom32_show_load_progress_default.  */
static void
ubicom32_show_load_progress_default (const char *section,
			    unsigned int section_sent,
			    unsigned int section_size,
			    unsigned int total_sent,
			    unsigned int total_size)
{
  static int progress;
  int current;

  if (total_sent == 0)
    progress = 0;

  current = (int)((float)total_sent / (float)total_size * (float)80);
  while (progress < current)
    {
      putchar_unfiltered ('.');
      gdb_flush (gdb_stdout);
      progress ++;
    }

  if (total_sent == total_size)
    {
      putchar_unfiltered ('\n');
      gdb_flush (gdb_stdout);
    }
}

void
ubicom32_erase_progress_callback(int total_length, int current_length, int block_size, void *param)
{
  int current;
  static int progress = 0;

  if(current_length == 0)
    {
      progress = 0;
      return;
    }

  current = (int)((float)current_length/(float)total_length*(float)80);
  while(progress < current)
    {
      printf(".");
      fflush(stdout);
      progress++;
    }

  if(current_length == total_length)
    printf("\n");
}

/* ubicom32_load.  */
static void
ubicom32_load (char *file, int from_tty)
{
  bfd *loadfile_bfd;
  asection *s;
  int total_length = 0;
  int progress = 0;
  int addr, length, offs, size;
  unsigned char *data, *protect, *foundK;
  unsigned int last_address = 0;
  unsigned int first_address = 0xffffffff;
  int ret;
  unsigned protectStart, protectEnd;
  extern unsigned char dkmajor, dkminro, dkstate, dkincrement;
  extern unsigned int dkunique;

  int changedProtect = 0;
  int dontLoadProtect = 1;
  int hasProtect = 0;
  int hasDebugKernel = 0;
  int useNewDownloadMethod = 0;
  unsigned int reply;
  extern int whoareyou(unsigned int *reply);

  FBlock_t *fBlock = firstFlashBlock;

  /* walk down the flash block and release everything */
  while(fBlock)
    {
      FBlock_t *next = fBlock->next;
      /* free the breakpointList */
      free(fBlock->breakpointList);

      /* free the breakpointRemoveList */
      free(fBlock->breakpointRemoveList);

      /* free the code block */
      free(fBlock->code);

      /* free the fBlock itself */
      free(fBlock);

      fBlock = next;
    }

  firstFlashBlock = lastFlashBlock = NULL;

  /* Force the bootkernel to run */
  ret = initBackendToBootKernel(1);

  size = UBICOM32_LOAD_BLOCK_SIZE;
  data = malloc(size*2);
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for write\n");
      return;
    }

  if (remote_debug)
    printf_filtered ("ubicom32_load: %s (tty=%d)\n", file, from_tty);

  /* Open the file.  */
  loadfile_bfd = bfd_openr (file, gnutarget);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  protectStart = UBICOM32_FLASH_ADDR;
  protectEnd = protectStart + flashSectorSize;

  /* see if we can use the new download mechanism. */
  if((dkmajor >2) || ((dkmajor == 2) && (dkunique >= 5)))
    {
      /*
       * Scan through the sections looking for the .downloder section. We better find it
       * because without it we are not going to be able to download any code to the board.
       */
      for (s = loadfile_bfd->sections; s; s = s->next)
	{
	  unsigned char buf[4];
	  CORE_ADDR length;
	  int sec;
	  unsigned int section_last_address;
	  size_t len;

	  if(strcmp(s->name, ".downloader"))
	    {
	      continue;
	    }

	  useNewDownloadMethod = 1;
	  break;
	}

      if(useNewDownloadMethod ) {
	/* If we got here then download the .downloader section to the board. */
	size = UBICOM32_LOAD_BLOCK_SIZE;

	length = bfd_get_section_size (s);
	offs = 0;
	while(length)
	  {
	    int transfer, readtransfer;
	    int myaddr;
	    char *mydata;
	    char *verifybuf = &data[size];

	    if(size > length)
	      size = length;

	    /* read in the data size bytes at a time from the section */
	    bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	    addr = s->vma + offs;

	    myaddr = addr;
	    mydata = data;

	    transfer = ubicom32_write_bytes(myaddr, mydata, size);
	    /* read the data and verify */
	    readtransfer= ubicom32_read_bytes(addr, verifybuf, transfer);
	    if (memcmp (data, verifybuf, transfer) != 0)
	      {
		printf_unfiltered ("error: verify: data does not match!\n");
		printf_unfiltered ("Data in block starting at 0x%lx length %d\n",
				   addr, transfer);
		free (data);
		return ;
	      }

	    offs += transfer;
	    length -= transfer;
	  }

	/* now transfer control to the .downloader section */
	ret = jumpToPc(s->vma);
	if(ret)
	  {
	    /* Jump to PC failed */
	    printf_unfiltered("Failed to revector the debugger thread to 0x%lx.\n", s->vma);
	    free(data);
	    return;
	  }

	/* sleep for about .2 seconds */
	usleep(200000);

	/* Before we can go ahead issue a who are you */
	ret = whoareyou(&reply);
	if(ret)
	  {
	    printf_unfiltered("whoareyou query bombed.");
	    free(data);
	    ubicom32_reset(0,0);
	    return;
	  }

	ret = setupForDownload();
      }
    }

  /* scan through the sections looking for the .protect section */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      unsigned char buf[4];
      CORE_ADDR length;
      int sec;
      unsigned int section_last_address;
      size_t len;

      sec = ubicom32_loadable_section (loadfile_bfd, s);
      if (sec != UBICOM32_FLASH_SECTION)
	continue;

      if(strcmp(s->name, ".protect"))
	continue;

      /* we have found the .protect section */
      length = bfd_get_section_size (s);
      hasProtect = 1;

      /* may have to readjust the protectEnd */
      protectStart = s->lma;
      protectEnd = protectStart + length;
      protectEnd += flashSectorSize -1;
      protectEnd &= ~(flashSectorSize -1);

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;

      /* read in the protect section info a buffer */
      protect = malloc (length );
      if (!protect)
	{
	  printf_unfiltered ("error: unable to allocate memory for reading in protect section\n");
	  return;
	}

      /* read in the data from the section */
      bfd_get_section_contents (loadfile_bfd, s, protect, offs, length);

      /* Now we hunt down looking for a string that looks like KernelID */
      foundK = protect;
      len = length - (foundK - protect);
      while(1)
	{
	  /* find the first 'K' in memory */
	  foundK = (unsigned char *) memchr(foundK, 'K', len);

	  if(foundK == NULL)
	    break;

	  if(!strncmp(foundK, "KernelID:", strlen("KernelID:")))
	    {
	      hasDebugKernel = 1;
	      break;
	    }

	  foundK += (strlen(foundK) +1);

	  len = length - (foundK - protect);
	  if(len > 0 && len < length)
	    continue;
	  else
	    {
	      foundK = NULL;
	      break;
	    }
	}

      if(hasDebugKernel)
	{
	  unsigned char *verptr = (unsigned char *)strchr(foundK, ':');
	  verptr++;

	  changedProtect = compareVersion(verptr[0] - '0',
					  verptr[2] - '0',
					  verptr[4],
					  verptr[5] - '0',
					  verptr[7] - '0');

	}

      free(protect);
      break;
    }

  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      unsigned char buf[4];
      CORE_ADDR length;
      int sec;
      unsigned int section_last_address;

      sec = ubicom32_loadable_section (loadfile_bfd, s);
      if (sec != UBICOM32_FLASH_SECTION)
	continue;

      if(dontLoadProtect)
	{
	  if(!strcmp(s->name, ".protect"))
	     continue;
	}

      if(hasProtect)
	{
	  /* check for bootsector encroachment */
	  if(protectStart <= s->lma && s->lma < protectEnd)
	    {
	      /* this section is encroaching the protect sector */
	      printf_unfiltered("error: Boot sector is being encroached by section \"%s\".\nSection \"%s\" start address = 0x%08lx.\nPlease check flash selection in ipStorage-ubicom32 package configuration.\nDownload has been aborted.\n",
				s->name, s->name, s->lma);
	      return;
	    }
	}

      length = bfd_get_section_size (s);
      total_length += length;
      section_last_address = length + s->lma;
      section_last_address +=3;

      /* round up the last address */
      section_last_address &= ~0x3;

      if(section_last_address > last_address)
	last_address = section_last_address;

      if(s->lma < first_address)
	first_address = s->lma;
    }

  if(last_address > UBICOM32_FLASH_ADDR + flashSize)
    {
	    /* too big image. Abort the load */
       printf_unfiltered("Dowload image is bigger than the flash. Download aborted.\n");
       free(data);
       return;
    }
  last_address -= first_address;

  if(hasProtect)
    {
      if(hasDebugKernel == 0)
	{
	  printf_unfiltered("\n\
WARNING: \".protect\" section has no boot kernel image.\n\
\"loadprotect -force\" command may be issued from gdb prompt\n\
to download the \".protect\" section to flash.\n\
Downloading this \".protect\" section to flash\n\
\"WILL DISABLE FUTURE FLASH PROGRAMMING OF THE BOARD\".\n\
If this is not desired then please make sure \"GDB Loader Support\"\n\
box under \"ipHAL-ubicom32 GDB Support\" is turned on.\n\n");
	}
      if(changedProtect && (useNewDownloadMethod == 0 ))
	{
	  printf_unfiltered("\
Caution: \".protect\" section has changed.\n\
\"loadprotect\" command may be issued from gdb prompt\n\
to download the \".protect\" section to flash.\n\n");
	}
    }

  /* tell spt that we are going to start code download. spt will issue the chip erase.
     This will block till the chip erase returns. */
  data[0] = 0;
  sprintf(data,"Begin Erase of non \".protect\" sections.\n\
0x%x length 0x%x", first_address, last_address);
  printf("%s\n", data);

  ret = eraseVerifyFlash(first_address, last_address, ubicom32_erase_progress_callback, NULL);
  if(ret)
    return;

  printf("Begin download\n");
  /* Write flash.  */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32_loadable_section (loadfile_bfd, s) != UBICOM32_FLASH_SECTION)
	continue;

      if(dontLoadProtect)
	{
	  if(!strcmp(s->name, ".protect"))
	     continue;
	}

      length = bfd_get_section_size (s);

      if (remote_debug)
	printf_filtered("%s: 0 / %ld\n", s->name, length);
      ubicom32_show_load_progress_default (s->name, 0, length, progress, total_length);

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  ubicom32_write_bytes(addr, data, size);

	  offs += size;
	  length -= size;
	  progress += size;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %ld\n", s->name, length);
	  ubicom32_show_load_progress_default (s->name, 0, length, progress, total_length);

	}
    }

  /* Verify flash.  */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32_loadable_section (loadfile_bfd, s) != UBICOM32_FLASH_SECTION)
	continue;

      if(dontLoadProtect)
	{
	  if(!strcmp(s->name, ".protect"))
	     continue;
	}

      length = bfd_get_section_size (s);

      if (remote_debug)
	printf_filtered("%s: 0 / %ld\n", s->name, length);

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  if (common_verify_bytes(addr, data, size) < 0)
	    printf_unfiltered("verify error in section %s (0x%08lx - 0x%08lx)\n", s->name, addr, addr + size);

	  offs += size;
	  length -= size;
	  progress += size;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %ld\n", s->name, length);
	}
    }

  free(data);

  /* reset and restart */
  ubicom32_reset(0, 0);

}

static void
ubicom32_mail_status (char *file, int from_tty)
{
  int status;
  extern int readStatus(int *);
  int ret = readStatus(&status);
  printf("mail status = 0x%8x\n", status);
}

static void
ubicom32_read_mailbox (char *file, int from_tty)
{
  int mboxData=0xaa55aa55;
  extern int readMailbox(int *);
  int ret = readMailbox(&mboxData);
  printf("Ret = %d Mailbox sent = 0x%8x\n", ret,  mboxData);
}

#if 0
static void
ubicom32_mail_clear (char *file, int from_tty)
{
  extern void clean_mail(void);

  clean_mail();

}
#endif
/* ubicom32_load_protect.  */
static void
ubicom32_load_protect (char *arg, int from_tty)
{
  bfd *loadfile_bfd;
  asection *s;
  int total_length = 0;
  int progress = 0;
  int addr, length, offs, size;
  unsigned char *data, *verifybuf, *protect, *version, *foundK;
  unsigned int last_address = 0;
  unsigned int first_address = 0xffffffff;
  int ret;
  int dontLoadProtect = 1;

  int hasProtect = 0;
  int hasDebugKernel = 0;
  int abortLoad = 0;
  char *file;
  extern unsigned char dkmajor, dkminro, dkstate, dkincrement;
  extern unsigned int dkunique;

  size = UBICOM32_LOAD_BLOCK_SIZE;
  data = malloc (size *2 );
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for write\n");
      return;
    }
  verifybuf = &data[size];

  file = get_exec_file(1);

  if (remote_debug)
    printf_filtered ("ubicom32_load: %s (tty=%d)\n", file, from_tty);

  /* Open the file.  */
  loadfile_bfd = bfd_openr (file, gnutarget);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  /* scan through the sections looking for the .dynamic_non_volatile section */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      unsigned char buf[4];
      CORE_ADDR length;
      int sec, len;
      unsigned psStart, psEnd;
      unsigned int section_last_address;
      int bkendtobk = 1;

      sec = ubicom32_loadable_section (loadfile_bfd, s);
      if (sec != UBICOM32_FLASH_SECTION )
	continue;

      if(strcmp(s->name, ".dynamic_non_volatile"))
	continue;

      /* load last_address with the lma address of this section  */
      last_address = s->lma;
    }

  /* scan through the sections looking for the .protect section */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      unsigned char buf[4];
      CORE_ADDR length;
      int sec, len;
      unsigned psStart, psEnd;
      unsigned int section_last_address;
      int bkendtobk = 1;

      sec = ubicom32_loadable_section (loadfile_bfd, s);
      if (sec != UBICOM32_FLASH_SECTION )
	continue;

      if(strcmp(s->name, ".protect"))
	continue;

      /* we have found the .protect section */
      length = bfd_get_section_size (s);
      hasProtect = 1;

      total_length += length;
      section_last_address = length + s->lma;
      section_last_address +=3;

      /* round up the last address */
      section_last_address &= ~0x3;

      if(section_last_address > last_address)
	last_address = section_last_address;

      if(s->lma < first_address)
	first_address = s->lma;

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;

      /* read in the protect section info a buffer */
      protect = malloc (length );
      if (!protect)
	{
	  printf_unfiltered ("error: unable to allocate memory for reading in protect section\n");
	  return;
	}

      /* read in the data from the section */
      bfd_get_section_contents (loadfile_bfd, s, protect, offs, length);

      /* Now we hunt down looking for a string that looks like KernelID */
      foundK = protect;
      len = length - (foundK - protect);
      while(1)
	{
	  /* find the first 'K' in memory */
	  foundK = (unsigned char *) memchr(foundK, 'K', len);

	  if(foundK == NULL)
	    break;

	  if(!strncmp(foundK, "KernelID:", strlen("KernelID:")))
	    {
	      hasDebugKernel = 1;
	      break;
	    }

	  foundK += (strlen(foundK) +1);

	  len = length - (foundK - protect);
	  if(len > 0 && len < length)
	    continue;
	  else
	    {
	      foundK = NULL;
	      break;
	    }
	}

      if(hasDebugKernel == 0)
	{
	  if(arg == NULL)
	    abortLoad = 1;
	  else if(strcmp(arg, "-force")) {
	    abortLoad = 1;
	  }
	}
      else
	{
	  unsigned char *verptr = (unsigned char *)strchr(foundK, ':');
	  verptr++;
	  version = strdup(verptr);
	}

      free(protect);

      if(abortLoad)
	{
	  printf_unfiltered("\n\
WARNING: \".protect\" section has no boot kernel image.\n\
\"loadprotect -force\" command may be issued from gdb prompt\n\
to download the \".protect\" section to flash.\n\
Downloading this \".protect\" section to flash\n\
\"WILL DISABLE FUTURE FLASH PROGRAMMING OF THE BOARD\".\n\
If this is not desired then please make sure \"GDB Loader Support\"\n\
box under \"ipHAL-ubicom32 GDB Support\" is turned on.\n\
\".protect section\" download aborted.\n\n");
	  return;
	}

      bkendtobk = 1;
      if(arg && (strcmp(arg, "-nobk") == 0))
	 bkendtobk = 0;

      if(bkendtobk) {
	/* Force the bootkernel to run */
	ret = initBackendToBootKernel(1);
      }

      while(length)
	{
	  int transfer, readtransfer;
	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  /* read the data from .protect and verify */
	  readtransfer= ubicom32_read_bytes(addr, verifybuf, size);
	  if (memcmp (data, verifybuf, size) != 0)
	    {
	      dontLoadProtect = 0;
	      break;
	    }
	  offs += size;
	  length -= size;

	}

      break;
    }
  last_address -= first_address;

  /* We will download only if the .protect section is different than what is already there */
  if(dontLoadProtect)
    {
      /* Check if user wants to forcibly load the debug kernel */
      if(arg == NULL || (strcmp(arg, "-force")))
	{
	   /* reset and restart */
	   ubicom32_reset(0, 0);
	   return;
	}
    }

  /* see if we can use the new download mechanism. */
  if((dkmajor >2) || ((dkmajor == 2) && (dkunique >= 5)))
    {
      unsigned char useNewDownloadMethod = 0;
      /*
       * Scan through the sections looking for the .downloder section. We better find it
       * because without it we are not going to be able to download any code to the board.
       */
      for (s = loadfile_bfd->sections; s; s = s->next)
	{
	  unsigned char buf[4];
	  CORE_ADDR length;
	  int sec;
	  unsigned int section_last_address;
	  size_t len;

	  if(strcmp(s->name, ".loadprotect"))
	    {
	      continue;
	    }

	  useNewDownloadMethod = 1;
	  break;
	}

      if(useNewDownloadMethod) {
	/* If we got here then download the .loadprotect section to the board. */

	size = UBICOM32_LOAD_BLOCK_SIZE;

	length = bfd_get_section_size (s);
	offs = 0;
	while(length)
	  {
	    int transfer, readtransfer;
	    int myaddr;
	    char *mydata;
	    char *verifybuf = &data[size];

	    if(size > length)
	      size = length;

	    /* read in the data size bytes at a time from the section */
	    bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	    addr = s->vma + offs;

	    myaddr = addr;
	    mydata = data;

	    transfer = ubicom32_write_bytes(myaddr, mydata, size);
	    /* read the data and verify */
	    readtransfer= ubicom32_read_bytes(addr, verifybuf, transfer);
	    if (memcmp (data, verifybuf, transfer) != 0)
	      {
		printf_unfiltered ("error: verify: data does not match!\n");
		printf_unfiltered ("Data in block starting at 0x%lx length %d\n",
				   addr, transfer);
		free (data);
		return ;
	      }

	    offs += transfer;
	    length -= transfer;
	  }

	/* now transfer control to the .loadprotect section */
	ret = jumpToPc(s->vma);
	if(ret)
	  {
	    /* Jump to PC failed */
	    printf_unfiltered("Failed to revector the debugger thread to 0x%lx.\n", s->vma);
	    free(data);
	    return;
	  }

	/* sleep for about .2 seconds */
	usleep(1000000);

	free(data);
	ubicom32_reset(0,0);
	return;
      }
    }

  /* Issue the chip erase. This will block till the chip erase returns. */
  data[0] = 0;
  sprintf(data,"beginErase 0x%x length 0x%x", first_address, last_address);
  printf("%s\n", data);

  ret = eraseVerifyFlash(first_address, last_address, ubicom32_erase_progress_callback, NULL);
  if(ret)
    return;

  printf("Begin download\n");
  /* Write internal flash.  */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32_loadable_section (loadfile_bfd, s) != UBICOM32_FLASH_SECTION)
	continue;

      if(hasProtect)
	{
	  if(strcmp(s->name, ".protect"))
	     continue;
	}

      length = bfd_get_section_size (s);

      if (remote_debug)
	printf_filtered("%s: 0 / %ld\n", s->name, length);
      ubicom32_show_load_progress_default (s->name, 0, length, progress, total_length);

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  int transfer;
	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  transfer = ubicom32_write_bytes(addr, data, size);
	  offs += transfer;
	  length -= transfer;
	  progress += transfer;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %ld\n", s->name, length);
	  ubicom32_show_load_progress_default (s->name, 0, length, progress, total_length);

	}

      /* now do the verify */
      length = bfd_get_section_size (s);

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  int readtransfer;
	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  /* read the data and verify */
	  readtransfer= ubicom32_read_bytes(addr, verifybuf, size);
	  if (memcmp (data, verifybuf, readtransfer) != 0)
	    {
	      printf_unfiltered ("error: verify: data does not match!\n");
	      printf_unfiltered ("Data in block starting at 0x%lx length %d\n",
				 addr, readtransfer);
	      free (data);
	      return ;
	    }

	  offs += readtransfer;
	  length -= readtransfer;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %ld\n", s->name, length);
	}


      break;
    }

  free(data);

  /* reset and restart */
  ubicom32_reset(0, 0);

}

/* ubicom32_hw_mon_sample */
static void
ubicom32_core_dump (char *args, int from_tty)
{
  char default_name[]="Ubicom.core";
  int default_sdram_size = 16*1024*1024;
  int sdsize;
  struct ubicom32_core_header coreHeader;
  char *filename;
  CORE_ADDR begin_data, end_data;
  CORE_ADDR begin_sdram, end_sdram;
  CORE_ADDR begin_pram, end_pram;
  unsigned int mem_size;
  FILE *fp;
  int i;
  char *buffer;
  unsigned int progress, total_size;
  int ret_val;

  filename = default_name;

  if(debugging_processor == 5133)
    {
      extern void ubicom32v3_core_dump(char *args, int from_tty);
      ubicom32v3_core_dump(args, from_tty);
      return;
    }

  if(args)
    {
      /* look for space in the buffer */
      char *space = args;

      if(isdigit(space[0]))
	{
	  /*
	   * we do no allow filenames to start with a number. So this is a case of
	   * coredump n.
	   */
	  int ret = sscanf(space, "%d", &sdsize);
	  if(ret == 1)
	    {
	      if(sdsize < 1)
		{
		  /* clamp it to 1 */
		  sdsize = 1;
		}
	      else if(sdsize > 16)
		{
		  /* clamp it to 16 */
		  sdsize = 16;
		}

	      default_sdram_size = sdsize *1024*1024;
	    }
	  else
	    {
	      printf_unfiltered("scanf bombed ret = %d\n", ret);
	    }
	}
      else
	{
	  space  = strchr(args, ' ');
	  if(space)
	    {
	      int ret;
	      *space++ = 0;

	      /* scan the rest of the string for a number */
	      ret = sscanf(space, "%d", &sdsize);
	      if(ret == 1)
		{
		  if(sdsize < 1)
		    {
		      /* clamp it to 1 */
		      sdsize = 1;
		    }
		  else if(sdsize > 8)
		    {
		      /* clamp it to 8 */
		      sdsize = 8;
		    }
		  default_sdram_size = sdsize *1024*1024;
		}
	      else
		{
		  printf_unfiltered("scanf bombed ret = %d\n", ret);
		}
	    }
	}
    }

  fp = fopen(filename, "wb+");

  if(fp == NULL)
    {
      printf_filtered("ubicom32_core_dump: Cannot open %s\n", filename);
      return;
    }

  /*
   * Turn off the 2 wire interface if possible.
   */
  ret_val = turnOff2wire();

  /* We managed to open the file */
  /* Update the register info for all the threads */
  for(i=0; i< 8; i++)
    gdbMainGetallRegs(i, &mainRegisters);

  /* malloc a 1 Meg buffer to move the data through */
  buffer = malloc(1024*1024);

  if(buffer == NULL)
    {
      printf_unfiltered("ubicom32_core_dump: Failed to malloc transfer buffer\n");
      return;
    }

  /* look up the various symbols */
  begin_data = ubicom32_lookup_symbol("__data_begin");
  end_data = ubicom32_lookup_symbol("__data_limit");
  begin_sdram = ubicom32_lookup_symbol("__sdram_begin");
  end_sdram = ubicom32_lookup_symbol("__sdram_limit");
  begin_pram = ubicom32_lookup_symbol("__pram_begin");
  end_pram = ubicom32_lookup_symbol("__pram_limit");

  if(begin_data == 0)
    printf("begin_data not found\n");

  if(end_data == 0)
    printf("end_pram not found\n");

  if(begin_pram == 0)
    printf("begin_pram not found\n");

  if(end_pram == 0)
    printf("end_pram not found\n");

  if(begin_sdram == 0)
    printf("begin_sdram not found\n");

  if(end_sdram == 0)
    printf("end_sdram not found\n");

  /* load Ubicom Core Magic into the header */
  coreHeader.magic = 0x1234aa55;

  /* Set file position to end of header */
  (void) fseek(fp, sizeof(coreHeader), SEEK_SET);

  /* write out debugger thread number */
  (void) fwrite(&debuggerThreadNo, 1, 4, fp);

  /* write out sector size */
  (void) fwrite(&flashSectorSize, 1, 4, fp);

  /* wirte out flash size */
  (void) fwrite(&flashSize, 1, 4, fp);

  /* load the register offset into the coreHeader */
  coreHeader.regOffset = ftell(fp);

  /* write out register area size */
  mem_size = sizeof(mainRegisters);
  (void) fwrite(&mem_size, 1, 4, fp);

  /* write out the register data */
  (void) fwrite(&mainRegisters, 1, sizeof(mainRegisters), fp);

  /* load hrt offset into coreHeader */
  coreHeader.hrtOffset = ftell(fp);

  /* read and write the hrt block */
  (void) ubicom32_read_bytes(0x800, buffer, 512);
  (void) fwrite(buffer, 1, 512, fp);

  /* load data offset into coreHeader */
  coreHeader.dataOffset = ftell(fp);

  printf_unfiltered("Dumping DATA memory.\n");
  /* write out the data memory size */
  total_size = mem_size = end_data - begin_data;
  (void) fwrite(&mem_size, 1, 4, fp);
  progress = 0;
  ubicom32_show_load_progress_default(NULL, 0, 0, progress, total_size);

  /* now read and write out the data memory */
  while(mem_size)
    {
      int length = 1024*1024;

      if(length > mem_size)
	length = mem_size;

      /* read in the data */
      //(void) ubicom32_read_bytes(begin_data, buffer, length);
      (void) readDataMemoryRaw(begin_data, length/4, (int *)buffer);

      /* write out the data. */
      (void) fwrite(buffer, 1, length, fp);

      begin_data += length;
      mem_size -= length;
      progress += length;
      ubicom32_show_load_progress_default(NULL, 0, 0, progress, total_size);
    }

  /* load pram offset into coreHeader */
  coreHeader.pramOffset = ftell(fp);

  printf_unfiltered("Dumping PRAM.\n");
  /* write out the pram memory size */
  total_size = mem_size = end_pram - begin_pram;
  (void) fwrite(&mem_size, 1, 4, fp);
  progress = 0;
  ubicom32_show_load_progress_default(NULL, 0, 0, progress, total_size);

  /* now read and write out the data memory */
  while(mem_size)
    {
      int length = 1024*1024;

      if(length > mem_size)
	length = mem_size;

      /* read in the data */
      //(void) ubicom32_read_bytes(begin_pram, buffer, length);
      (void) readPgmMemoryRaw(begin_pram, length/4, (int *)buffer);

      /* write out the data. */
      (void) fwrite(buffer, 1, length, fp);

      begin_pram += length;
      mem_size -= length;
      progress += length;
      ubicom32_show_load_progress_default(NULL, 0, 0, progress, total_size);
    }

  printf_unfiltered("Dumping SDRAM. This will take time\n");
  /* load sdram offset into coreHeader */
  coreHeader.sdramOffset = ftell(fp);

  /* read and write out the sdram */
  total_size = mem_size = default_sdram_size;
  if((end_sdram - begin_sdram) < mem_size)
    mem_size = (end_sdram - begin_sdram);

  total_size = mem_size;

  (void) fwrite(&mem_size, 1, 4, fp);
  progress = 0;
  ubicom32_show_load_progress_default(NULL, 0, 0, progress, total_size);

  /* now read and write out the data memory */
  while(mem_size)
    {
      int length = 1024*1024;

      if(length > mem_size)
	length = mem_size;

      /* read in the data */
      //(void) ubicom32_read_bytes(begin_sdram, buffer, length);
      (void) readDataMemoryRaw(begin_sdram, length/4, (int *)buffer);

      /* write out the data. */
      (void) fwrite(buffer, 1, length, fp);

      begin_sdram += length;
      mem_size -= length;
      progress += length;
      ubicom32_show_load_progress_default(NULL, 0, 0, progress, total_size);
    }

#if 0
  /* read and write out the dynamic_non_volatile_section  */
  begin_data = ubicom32_lookup_symbol("__dynamic_non_volatile_load_begin");
  end_data = ubicom32_lookup_symbol("__dynamic_non_volatile_reserve_end");
#endif

  /* read and write out the dynamic_non_volatile_section  */
  begin_data = ubicom32_lookup_symbol("__flash_begin");
  end_data = ubicom32_lookup_symbol("__flash_limit");

  /* load flash offset into coreHeader */
  coreHeader.flashOffset = ftell(fp);

  /* write out the dynamic_non_volatile block start address to core file */
  (void) fwrite(&begin_data, 1, 4, fp);

  printf_unfiltered("Dumping FLASH. This will take time\n");
  /* write out the data memory size */
  total_size = mem_size = end_data - begin_data;
  (void) fwrite(&mem_size, 1, 4, fp);
  progress = 0;
  ubicom32_show_load_progress_default(NULL, 0, 0, progress, total_size);

  /* now read and write out the data memory */
  while(mem_size)
    {
      int length = 1024*1024;

      if(length > mem_size)
	length = mem_size;

      /* read in the data */
      //(void) ubicom32_read_bytes(begin_data, buffer, length);
      (void) readPgmMemoryRaw(begin_data, length/4, (int *)buffer);

      /* write out the data. */
      (void) fwrite(buffer, 1, length, fp);

      begin_data += length;
      mem_size -= length;
      progress += length;
      ubicom32_show_load_progress_default(NULL, 0, 0, progress, total_size);
    }

  /* rewind the file and write out the header into the file */
  rewind(fp);

  (void) fwrite(&coreHeader, 1, sizeof(coreHeader), fp);

  /* close the file */
  fclose(fp);

  /* Turn on turbo 2 wire if possible. */
  ret_val = turnOn2wire();
  return;
}

/* ubicom32_verify.  */
static void
ubicom32_verify (char *arg, int from_tty)
{
  bfd *loadfile_bfd;
  asection *s;
  int total_length = 0;
  int progress = 0;
  int addr, length, offs, size;
  unsigned char *data, *verifybuf, *protect;
  unsigned int last_address = 0;
  unsigned int first_address = 0xffffffff;
  int ret;

  char *file;

  size = UBICOM32_LOAD_BLOCK_SIZE;
  data = malloc (size *2 );
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for write\n");
      return;
    }
  verifybuf = &data[size];

  file = get_exec_file(1);

  if (remote_debug)
    printf_filtered ("ubicom32_load: %s (tty=%d)\n", file, from_tty);

  /* Open the file.  */
  loadfile_bfd = bfd_openr (file, gnutarget);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32_loadable_section (loadfile_bfd, s) != UBICOM32_FLASH_SECTION)
	continue;

      length = bfd_get_section_size (s);

      if (remote_debug)
	printf_filtered("%s: 0 / %ld\n", s->name, length);

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  int readtransfer;
	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  /* read the data and verify */
	  readtransfer= ubicom32_read_bytes(addr, verifybuf, size);
	  if (memcmp (data, verifybuf, readtransfer) != 0)
	    {
	      int i;
	      char *badblock = verifybuf;
	      char *cmpblock = data;
	      int currentaddress = addr;
	      for(i = 0; i< readtransfer; i++, currentaddress++) {
		if(badblock[i] != cmpblock[i]){
		  printf("addr = 0x%08lx expect 0x%02hhx got 0x%02hhx\n", currentaddress, (unsigned)cmpblock[i], (unsigned)badblock[i]);
		  break;
		}
	      }
	    }

	  offs += readtransfer;
	  length -= readtransfer;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %ld\n", s->name, length);
	}
    }

  free(data);

}

/* ubicom32_fix_protect.  */
static void
ubicom32_fix_protect (char *arg, int from_tty)
{
  bfd *loadfile_bfd;
  asection *s;
  int total_length = 0;
  int progress = 0;
  int addr, length, offs, size;
  unsigned char *data, *verifybuf, *protect, *version, *foundK;
  unsigned int last_address = 0;
  unsigned int first_address = 0xffffffff;
  int ret;
  int dontLoadProtect = 1;

  int hasProtect = 0;
  int hasDebugKernel = 0;
  int abortLoad = 0;
  char *file;

  size = UBICOM32_LOAD_BLOCK_SIZE;
  data = malloc (size *2 );
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for write\n");
      return;
    }
  verifybuf = &data[size];

  file = get_exec_file(1);

  if (remote_debug)
    printf_filtered ("ubicom32_load: %s (tty=%d)\n", file, from_tty);

  /* Open the file.  */
  loadfile_bfd = bfd_openr (file, gnutarget);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  /* scan through the sections looking for the .protect section */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      unsigned char buf[4];
      CORE_ADDR length;
      int sec, len;
      unsigned psStart, psEnd;
      unsigned int section_last_address;

      sec = ubicom32_loadable_section (loadfile_bfd, s);

      if(strcmp(s->name, ".protect"))
	continue;

      /* we have found the .protect section */
      length = bfd_get_section_size (s);
      hasProtect = 1;

      total_length += length;
      section_last_address = length + s->lma;
      section_last_address +=3;

      /* round up the last address */
      section_last_address &= ~0x3;

      if(section_last_address > last_address)
	last_address = section_last_address;

      if(s->lma < first_address)
	first_address = s->lma;

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;

      /* read in the protect section info a buffer */
      protect = malloc (length );
      if (!protect)
	{
	  printf_unfiltered ("error: unable to allocate memory for reading in protect section\n");
	  return;
	}

      /* read in the data from the section */
      bfd_get_section_contents (loadfile_bfd, s, protect, offs, length);

      /* Now we hunt down looking for a string that looks like KernelID */
      foundK = protect;
      len = length - (foundK - protect);
      while(1)
	{
	  /* find the first 'K' in memory */
	  foundK = (unsigned char *) memchr(foundK, 'K', len);

	  if(foundK == NULL)
	    break;

	  if(!strncmp(foundK, "KernelID:", strlen("KernelID:")))
	    {
	      hasDebugKernel = 1;
	      break;
	    }

	  foundK += (strlen(foundK) +1);

	  len = length - (foundK - protect);
	  if(len > 0 && len < length)
	    continue;
	  else
	    {
	      foundK = NULL;
	      break;
	    }
	}

      if(hasDebugKernel == 0)
	{
	  if(arg == NULL)
	    abortLoad = 1;
	  else if(strcmp(arg, "-force"))
	    abortLoad = 1;
	}
      else
	{
	  unsigned char *verptr = (unsigned char *)strchr(foundK, ':');
	  verptr++;
	  version = strdup(verptr);
	}

      free(protect);

      if(abortLoad)
	{
	  printf_unfiltered("\n\
WARNING: \".protect\" section has no boot kernel image.\n\
\"loadprotect -force\" command may be issued from gdb prompt\n\
to download the \".protect\" section to flash.\n\
Downloading this \".protect\" section to flash\n\
\"WILL DISABLE FUTURE FLASH PROGRAMMING OF THE BOARD\".\n\
If this is not desired then please make sure \"GDB Loader Support\"\n\
box under \"ipHAL-ubicom32 GDB Support\" is turned on.\n\
\".protect section\" download aborted.\n\n");
	  return;
	}

      break;
    }

  printf("Begin download\n");
  /* Write internal flash.  */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32_loadable_section (loadfile_bfd, s) != UBICOM32_PRAM_SECTION)
	continue;

      length = bfd_get_section_size (s);
      printf_filtered("%s: 0 / %ld\n", s->name, length);

      if (remote_debug)
	printf_filtered("%s: 0 / %ld\n", s->name, length);
      ubicom32_show_load_progress_default (s->name, 0, length, progress, total_length);

      size = UBICOM32_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  int transfer, readtransfer;
	  int myaddr;
	  char *mydata;

	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  myaddr = addr;
	  mydata = data;

	  for(transfer =0; transfer < size; transfer+=4, myaddr+= 4, mydata+=4) {
		  readtransfer = ubicom32_write_bytes(myaddr, mydata, 4);
	  }

	  /* read the data and verify */
	  readtransfer= ubicom32_read_bytes(addr, verifybuf, transfer);
	  if (memcmp (data, verifybuf, transfer) != 0)
	    {
	      printf_unfiltered ("error: verify: data does not match!\n");
	      printf_unfiltered ("Data in block starting at 0x%lx length %d\n",
				 addr, transfer);
	      free (data);
	      return ;
	    }

	  offs += transfer;
	  length -= transfer;
	  progress += transfer;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %ld\n", s->name, length);
	  ubicom32_show_load_progress_default (s->name, 0, length, progress, total_length);

	}
    }

  free(data);

  printf("Now fix the gdb thread stack and let it run\n");
}

FBlock_t *firstFlashBlock = NULL; /* Pointer to the first flash block */
FBlock_t *lastFlashBlock = NULL; /* Pointer to the last flash block */
unsigned char dirtyFlashBlocks = 0; /* Global flag to know when there are dirty flash blocks  */

static void
ubicom32FlashStats(char *file, int from_tty)
{
  /* print info about flash blocks */
  FBlock_t *fBlock = firstFlashBlock;
  int i;

  while(fBlock)
    {
      printf_filtered("Flash Block # 0x%x flashState = %d numBP = %d\nnumRemoveBP = %d GDBState = %d numWrites= %d\n",
		      fBlock->blockNumber,
		      fBlock->flashState,
		      fBlock->numBreakpoints,
		      fBlock->numRemoveBPs,
		      fBlock->flashBlockGDBState,
		      fBlock->numWrites);

      if(fBlock->numBreakpoints)
	{
	  /* print out the breakpoint list */
	  i =0;
	  printf_filtered("Break points are\n0x%4x ", fBlock->breakpointList[i++]);

	  for(;i< fBlock->numBreakpoints; i++)
	    {
	      printf_filtered("%8x ", fBlock->breakpointList[i]);

	      if((i%8) == 0)
		printf_filtered("\n");
	    }
	}

      if(fBlock->numRemoveBPs)
	{
	  /* print out the breakpoint list */
	  i =0;
	  printf_filtered("Removed Break points are\n0x%4x ", fBlock->breakpointRemoveList[i++]);

	  for(;i< fBlock->numRemoveBPs; i++)
	    {
	      printf_filtered("%8x ", fBlock->breakpointRemoveList[i]);

	      if((i%8) == 0)
		printf_filtered("\n");
	    }
	}
      printf_filtered("\n\n");

      fBlock = fBlock->next;
    }
}

unsigned int
ubicom32WriteFlashBlock(FBlock_t * fBlock)
{
  unsigned int ret = 0;
  unsigned int iWrite = 1;
  unsigned int bp_size;
  CORE_ADDR addr = 0x20000000;
  unsigned int *bp = (unsigned int *) gdbarch_breakpoint_from_pc(current_gdbarch, &addr, &bp_size);
  unsigned int bpToSend;
  char *cptr = (char *)&bpToSend;
  int i;

  memcpy((void *)&bpToSend, (void *)bp, bp_size);
  cptr[3] =  (~dontDebugMask & 0xff);

  /* everything is set for breakpoint insersion */
  if(fBlock->flashState == 0)
    {
      /* This is break point insertion. */

      for(i=0; i< fBlock->numBreakpoints; i++)
	{
	  unsigned int insn =
	    fBlock->code[(fBlock->breakpointList[i] & (flashSectorSize -1))/sizeof(unsigned int)];

	  if((insn & bpToSend) != bpToSend)
	  {
	    /* can't do a write */
	    iWrite =0;
	  }

	  fBlock->code[(fBlock->breakpointList[i] & (flashSectorSize -1))/sizeof(unsigned int)] = bpToSend;
	}
    }

  if(iWrite)
    {
      /* We have to remove some BPs case to check */
      for(i=0; i< fBlock->numRemoveBPs; i++)
	{
	  unsigned int opcode =
	    fBlock->code[(fBlock->breakpointRemoveList[i] & (flashSectorSize -1))/sizeof(unsigned int)];

	  if((opcode!= 0))
	    {
	      /* too bad can't do iWrites */
	      iWrite = 0;
	      break;
	    }
	}
    }

  if(iWrite == 0)
    {
      int length;
      int size;
      unsigned int current_block;
      unsigned char *destptr;
      unsigned int okToDoubleBufferOldValue;
      extern unsigned int okToDoubleBuffer;

      /* erase the block */
      ret = eraseVerifyFlash(fBlock->blockNumber, flashSectorSize, NULL, NULL);
      fBlock->numWrites ++;

      /* write the code block to flash  */
      length = flashSectorSize;
      current_block = fBlock->blockNumber;
      destptr = (unsigned char *)fBlock->code;

      okToDoubleBufferOldValue = okToDoubleBuffer;
      okToDoubleBuffer = 0;

      while(length)
	{
	  size = 1024;
	  if(size > length)
	    size = length;

	  ret = ubicom32_write_bytes (current_block, destptr, size);

	  length -= size;
	  current_block += size;
	  destptr += size;
	}
      okToDoubleBuffer = okToDoubleBufferOldValue;
    }
  else
    {
      /* write out the Break points if any */
      for(i=0; i< fBlock->numBreakpoints; i++)
	{
	  ret = ubicom32_write_bytes(fBlock->breakpointList[i],
				 (char *)&fBlock->code[(fBlock->breakpointList[i] & (flashSectorSize -1))/sizeof(unsigned int)],
				 sizeof(unsigned int));
	}

      /* remove Breakpoints if any */
      for(i=0; i< fBlock->numRemoveBPs; i++)
	{
	  ret = ubicom32_write_bytes(fBlock->breakpointRemoveList[i],
				 (char *)&fBlock->code[(fBlock->breakpointRemoveList[i] & (flashSectorSize -1))/sizeof(unsigned int)],
				 sizeof(unsigned int));
	}
    }

  fBlock->numRemoveBPs = 0;
  if(fBlock->numBreakpoints)
    {
      /* change state to BP installed */
    fBlock->flashState = 1;
    }
  else
    {
      fBlock->flashState = 2;
    }

  return 0;
}

static int
ubicom32_insert_breakpoint (struct bp_target_info *bp_tgt)
{
  unsigned int *bp = (unsigned int *) gdbarch_breakpoint_from_pc(current_gdbarch, &bp_tgt->placed_address, &bp_tgt->placed_size);
  CORE_ADDR addr = bp_tgt->placed_address;
  unsigned int bpToSend;
  char *cptr = (char *)&bpToSend;
  int ret;
  unsigned int flashSectorIgnore = (flashSectorSize < 16*1024)? 16*1024: flashSectorSize;
  /* or in the complement of dontDebugMask */

  memcpy((void *)&bpToSend, (void *)bp, bp_tgt->placed_size);
  cptr[3] =  (~dontDebugMask & 0xff);

  /* make sure the address is in the PRAM space. XXXXXX May change in the future */
  if(addr >= 0x40000000 &&  0x40080000 > addr)
    {
      /* valid pram address */

      /* read out the old contents */
      ret = ubicom32_read_bytes (addr, bp_tgt->shadow_contents, bp_tgt->placed_size);

      if(ret!= bp_tgt->placed_size)
	return -1;
      /* write in the break point */
      ret = ubicom32_write_bytes (addr, (char *) &bpToSend, bp_tgt->placed_size);

      if(ret == bp_tgt->placed_size)
	return 0;
      else
	return -1;
    }
  else if(addr >= (0x20000000 + flashSectorIgnore) && (0x20000000 + flashSize) > addr)
    {
      unsigned int blockNo = addr & ~(flashSectorSize -1);
      FBlock_t *fBlock = firstFlashBlock;
      int i;
      int BPState = 0;		/* 1 means an old installed BP */

      /* This is a break point in the flash but outside the protected sector */

      /* walk down the Flash block list and find the current block */
      while(fBlock)
	{
	  if(fBlock->blockNumber == blockNo)
	    {
	      /* we have a hit */
	      break;
	    }
	  else
	    fBlock = fBlock->next;
	}

      if(fBlock == NULL)
	{
	  int length;
	  int size;
	  unsigned int current_block;
	  unsigned char *destptr;

	  /* We are touching this block for the first time. We need to set it up */
	  fBlock = (FBlock_t *) malloc(sizeof (FBlock_t));
	  if(fBlock == NULL)
	    exit(1);

	  fBlock->numBreakpoints = 0;
	  fBlock->numRemoveBPs =0;
	  /* now malloc the code space */
	  fBlock->code = (unsigned int * ) malloc(flashSectorSize);

	  if(fBlock->code == NULL)
	    exit(2);

	  /* now malloc the space for 16 breakpoints */
	  fBlock->breakpointList = (unsigned int *) malloc(sizeof (unsigned int )*16);
	  if(fBlock->breakpointList == NULL)
	    exit(3);

	  /* now malloc the space for 16 removed breakpoints */
	  fBlock->breakpointRemoveList = (unsigned int *) malloc(sizeof (unsigned int)*16);
	  if(fBlock->breakpointRemoveList == NULL)
	    exit(3);

	  /* now initialize a few things */
	  fBlock->next = NULL;
	  fBlock->bpListSpace = 16;
	  fBlock->bpListRemoveSpace = 16;
	  fBlock->blockNumber = blockNo;
	  fBlock->flashBlockGDBState = 0;
	  fBlock->flashState = 0;
	  fBlock->numWrites = 0;

	  /* read in the code block from the Hardware and cache it */
	  length = flashSectorSize;
	  current_block = blockNo;
	  destptr = (unsigned char *)fBlock->code;

	  while(length)
	    {
	      size = UBICOM32_LOAD_BLOCK_SIZE;
	      if(size > length)
		size = length;

	      ret = ubicom32_read_bytes (current_block, destptr, size);

	      length -= size;
	      current_block += size;
	      destptr += size;
	    }

	  if(lastFlashBlock)
	    {
	      lastFlashBlock->next = fBlock;
	    }
	  else
	    {
	      firstFlashBlock = fBlock;
	    }

	  lastFlashBlock = fBlock;
	}

      fBlock->flashBlockGDBState = 1;

      /* walk down the remove breakpoint list to see if this is an installed breakpoint */
      for(i=0; i< fBlock->numRemoveBPs; i++)
	if(fBlock->breakpointRemoveList[i] == addr)
	  break;

      if(i < fBlock->numRemoveBPs)
	{
	  /* we need to collapse the breakpointRemoveList queue by one */
	  memcpy((void *)&fBlock->breakpointRemoveList[i],
		(void *)&fBlock->breakpointRemoveList[i+1],
		(fBlock->numRemoveBPs-- - i)* sizeof(unsigned int));
	  BPState = 1;		/* indicate this is an old installed Breakpoint  */
	}

      /* We now have a valid Flash Block pointer.
       * Walk down the breakpointList to see if this address is already there.
       */
      for(i=0; i< fBlock->numBreakpoints; i++)
	if(fBlock->breakpointList[i] == (unsigned short)addr)
	  break;

      if(i == fBlock->numBreakpoints)
	{
	  if(BPState == 0)
	    {
	      /* This is a new breakpoint */
	      fBlock->flashState = 0;
	    }

	  if(fBlock->numBreakpoints == fBlock->bpListSpace)
	    {
	      /* we are out of space. Time to realloc and bump up by 16 entries */
	      unsigned int *temp =
		(unsigned int *)realloc(fBlock->breakpointList,
					sizeof(unsigned int)*(fBlock->bpListSpace + 16));

	      if(temp == NULL)
		exit(3);

	      fBlock->breakpointList = temp;
	      fBlock->bpListSpace +=16;
	    }

	  /* send back the original code back to GDB */
	  memcpy((void *) bp_tgt->shadow_contents,
		 (void *) &fBlock->code[(addr & (flashSectorSize -1))/sizeof(unsigned int)],
		 sizeof(unsigned int));

	  /* copy the address into the breakpointList */
	  fBlock->breakpointList[fBlock->numBreakpoints ++] = (unsigned int)addr;

	  /* if is an old Breakpoint change is to the break instruction */
	  if(BPState)
	    {
	      fBlock->code[(addr & (flashSectorSize -1))/sizeof(unsigned int)] = bpToSend;
	    }
	}
      return 0;
    }
  else
    return EINVAL;
}

static int
ubicom32_remove_breakpoint (struct bp_target_info *bp_tgt)
{
  unsigned int *bp = (unsigned int *) gdbarch_breakpoint_from_pc(current_gdbarch, &bp_tgt->placed_address, &bp_tgt->placed_size);
  CORE_ADDR addr = bp_tgt->placed_address;
  int ret, i;
  char *prev_contents = (char *)malloc(bp_tgt->placed_size);
  char *cptr = (char *) bp;
  int *iptr = (int *)prev_contents;
  unsigned int flashSectorIgnore = (flashSectorSize < 16*1024)? 16*1024: flashSectorSize;

  /* make sure the address is in the PRAM space. XXXXXX May change in the future */
  if(addr >=0x40000000 &&  0x40080000 > addr)
    {

      /* read out the old contents */
      ret = ubicom32_read_bytes (addr, prev_contents, bp_tgt->placed_size);

#if 0
      /* blow off the lower 11 bits */
      *iptr &= ~(0x7ff);
#endif

      /* check if we do have a bp instruction at this address */
      for(ret =0; ret< bp_tgt->placed_size-1; ret++)
	if(cptr[ret] != prev_contents[ret])
	  return 0;

      /* write in the old contents */
      ret = ubicom32_write_bytes (addr, (char *) bp_tgt->shadow_contents, bp_tgt->placed_size);

      if(ret == bp_tgt->placed_size)
	return 0;
      else
	return -1;
    }
  else if(addr >= (0x20000000 + flashSectorIgnore) && (0x20000000 + flashSize) > addr)
    {
      /* flash break point case. Walk down the flash block list to find the block */
      unsigned int blockNo = (unsigned int)( addr & ~(flashSectorSize-1) );
      FBlock_t *fBlock = firstFlashBlock;
      ret = 0;

      /* walk down the Flash block list and find the current block */
      while(fBlock)
	{
	  if(fBlock->blockNumber == blockNo)
	    {
	      /* we have a hit */
	      break;
	    }
	  else
	    fBlock = fBlock->next;
	}

      if(fBlock == NULL)
	{
	  exit(5);
	}

      /* We now have a valid Flash Block pointer.
       * Walk down the breakpointList to see if this address is already there.
       */
      for(i=0; i< fBlock->numBreakpoints; i++)
	if(fBlock->breakpointList[i] == (unsigned int)addr)
	  break;

      if(i == fBlock->numBreakpoints)
	{
	  /* This is a error */
	  exit(6);
	}

      /* we have a valid entry to remove */
      memcpy((void *) &fBlock->code[(addr & (flashSectorSize -1 ))/sizeof(unsigned int)],
	     (void *) bp_tgt->shadow_contents,
	     sizeof(unsigned int));

      /* we need to collapse the breakpointList queue by one */
      memcpy((void *)&fBlock->breakpointList[i],
	     (void *)&fBlock->breakpointList[i+1],
	    (fBlock->numBreakpoints-- - i)* sizeof(unsigned int));

      if(fBlock->flashState == 1)
	{
	  /*
	   * We are dealing with a BP that is installed in the flash.
	   * We need to install it in the remove list.
	   *
	   * Walk down the breakpointRemoveList to see if this address is already there.
	   */
	  for(i=0; i< fBlock->numRemoveBPs; i++)
	    if(fBlock->breakpointRemoveList[i] == (unsigned int)addr)
	      break;

	  if(i == fBlock->numRemoveBPs)
	    {
	      if(fBlock->numRemoveBPs == fBlock->bpListRemoveSpace)
		{
		  /* we are out of space. Time to realloc and bump up by 16 entries */
		  unsigned int *temp =
		    (unsigned int *)realloc(fBlock->breakpointRemoveList,
					    (fBlock->bpListRemoveSpace + 16) * sizeof (unsigned int));

		  if(temp == NULL)
		    exit(3);

		  fBlock->breakpointRemoveList = temp;
		  fBlock->bpListRemoveSpace +=16;
		}

	      /* copy the address into the breakpointRemoveList */
	      fBlock->breakpointRemoveList[fBlock->numRemoveBPs ++] = (unsigned int)addr;
	    }
	}
      fBlock->flashBlockGDBState = 2;

      if(fBlock->numBreakpoints == 0 && fBlock->numRemoveBPs == 0)
	{
	  /* This block has no breakpoints in it */
	  fBlock->flashState = 2;
	}
      return 0;
    }
  else
    return EINVAL;
}

/*
 * Collect a descriptive string about the given thread.
 * The target may say anything it wants to about the thread
 * (typically info about its blocked / runnable state, name, etc.).
 * This string will appear in the info threads display.
 *
 * Optional: targets are not required to implement this function.
 */

static char *
ubicom32_threads_extra_info (struct thread_info *tp)
{
  unsigned int thread_num;
  extern char * ubicom32_remote_threads_extra_info (struct thread_info *tp);
  static char dbgbuffer[] = "Dbg Thread";
  thread_num = PIDGET(tp->ptid);

  ubicom32_current_thread = thread_num;
  set_thread(thread_num);

  if(ubicom32_current_thread - 1 == debuggerThreadNo)
    return dbgbuffer;
  else
    return (ubicom32_remote_threads_extra_info(tp));
}

/* ubicom32_protect.  */
static void
ubicom32_protect (char *args, int from_tty)
{
  int ret;
  if (remote_debug)
    printf_filtered ("ubicom32_protect\n");

  ret = protectBootSector();
}

/* ubicom32_unprotect.  */
static void
ubicom32_unprotect (char *args, int from_tty)
{
  int ret;
  if (remote_debug)
    printf_filtered ("ubicom32_unprotect\n");

  ret = unprotectBootSector();
}

/* init_ubicom32_ops.  */
static void
init_ubicom32_ops (void)
{
  ubicom32_ops.to_shortname = "ubicom32";
  ubicom32_ops.to_longname = "Remote ubicom32 debug Via Ubicom Ethernet Dongle";
  ubicom32_ops.to_doc = "Remote ubicom32 debug Via Ubicom Ethernet Dongle.\n\
Connect to target ubicom32 board as follows:\n\
target ubicom32 dongle-ip-address:5010\n\
";
  ubicom32_ops.to_open = ubicom32_open;
  ubicom32_ops.to_close = ubicom32_close;
  ubicom32_ops.to_attach = ubicom32_attach;
  ubicom32_ops.to_post_attach = NULL;
  ubicom32_ops.to_detach = ubicom32_detach;
  ubicom32_ops.to_resume = ubicom32_resume;
  ubicom32_ops.to_wait = ubicom32_wait;
  ubicom32_ops.to_fetch_registers = ubicom32_fetch_register;
  ubicom32_ops.to_store_registers = ubicom32_store_register;
  ubicom32_ops.to_prepare_to_store = ubicom32_prepare_to_store;
  ubicom32_ops.deprecated_xfer_memory = ubicom32_xfer_memory;
  ubicom32_ops.to_files_info = ubicom32_files_info;
  ubicom32_ops.to_kill = ubicom32_kill;
  ubicom32_ops.to_load = ubicom32_load;
  ubicom32_ops.to_insert_breakpoint = ubicom32_insert_breakpoint;
  ubicom32_ops.to_remove_breakpoint = ubicom32_remove_breakpoint;

  ubicom32_ops.to_terminal_init = NULL;
  ubicom32_ops.to_terminal_inferior = NULL;
  ubicom32_ops.to_terminal_ours_for_output = NULL;
  ubicom32_ops.to_terminal_ours = NULL;
  ubicom32_ops.to_terminal_info = NULL;
  ubicom32_ops.to_lookup_symbol = NULL;
  ubicom32_ops.to_create_inferior = NULL;
  ubicom32_ops.to_post_startup_inferior = NULL;
  ubicom32_ops.to_acknowledge_created_inferior = NULL;
  ubicom32_ops.to_insert_fork_catchpoint = NULL;
  ubicom32_ops.to_remove_fork_catchpoint = NULL;
  ubicom32_ops.to_insert_vfork_catchpoint = NULL;
  ubicom32_ops.to_remove_vfork_catchpoint = NULL;
  ubicom32_ops.to_insert_exec_catchpoint = NULL;
  ubicom32_ops.to_remove_exec_catchpoint = NULL;
  ubicom32_ops.to_reported_exec_events_per_exec_call = NULL;
  ubicom32_ops.to_has_exited = NULL;
  ubicom32_ops.to_mourn_inferior = NULL;
  ubicom32_ops.to_can_run = 0;
  ubicom32_ops.to_notice_signals = 0;
  ubicom32_ops.to_thread_alive = ubicom32_thread_alive;
  ubicom32_ops.to_pid_to_str = ubicom32_thread_pid_to_str;
  ubicom32_ops.to_stop = ubicom32_stop;
  ubicom32_ops.to_pid_to_exec_file = NULL;
  ubicom32_ops.to_stratum = process_stratum;
  ubicom32_ops.to_has_all_memory = 1;
  ubicom32_ops.to_has_memory = 1;
  ubicom32_ops.to_has_stack = 1;
  ubicom32_ops.to_has_registers = 1;
  ubicom32_ops.to_has_execution = 1;
  ubicom32_ops.to_sections = NULL;
  ubicom32_ops.to_sections_end = NULL;
  ubicom32_ops.to_magic = OPS_MAGIC;
  ubicom32_ops.to_extra_thread_info = ubicom32_threads_extra_info;
}

/* init_ubicom32_ops.  */
static void
init_ubicom32_core_ops (void)
{
  ubicom32_core_ops.to_shortname = "ubicom32core";
  ubicom32_core_ops.to_longname = "Remote ubicom32 debug Via Ubicom core File.";
  ubicom32_core_ops.to_doc = "Remote ubicom32 debug Via Ubicom core File.\n\
Invoke as ubicom32-elf-gdb elffile corefile.\n\
";
  ubicom32_core_ops.to_open = ubicom32_core_open;
  ubicom32_core_ops.to_close = ubicom32_core_close;
  ubicom32_core_ops.to_attach = NULL;
  ubicom32_core_ops.to_post_attach = NULL;
  ubicom32_core_ops.to_detach = ubicom32_core_detach;
  ubicom32_core_ops.to_resume = NULL;
  ubicom32_core_ops.to_wait = ubicom32_wait;
  ubicom32_core_ops.to_fetch_registers = ubicom32_fetch_register;
  ubicom32_core_ops.to_store_registers = NULL;
  ubicom32_core_ops.to_prepare_to_store = ubicom32_prepare_to_store;
  ubicom32_core_ops.deprecated_xfer_memory = ubicom32_core_xfer_memory;
  ubicom32_core_ops.to_files_info = ubicom32_files_info;
  ubicom32_core_ops.to_kill = ubicom32_core_kill;
  ubicom32_core_ops.to_load = NULL;
  ubicom32_core_ops.to_insert_breakpoint = NULL;
  ubicom32_core_ops.to_remove_breakpoint = NULL;

  ubicom32_core_ops.to_terminal_init = NULL;
  ubicom32_core_ops.to_terminal_inferior = NULL;
  ubicom32_core_ops.to_terminal_ours_for_output = NULL;
  ubicom32_core_ops.to_terminal_ours = NULL;
  ubicom32_core_ops.to_terminal_info = NULL;
  ubicom32_core_ops.to_lookup_symbol = NULL;
  ubicom32_core_ops.to_create_inferior = NULL;
  ubicom32_core_ops.to_post_startup_inferior = NULL;
  ubicom32_core_ops.to_acknowledge_created_inferior = NULL;
  ubicom32_core_ops.to_insert_fork_catchpoint = NULL;
  ubicom32_core_ops.to_remove_fork_catchpoint = NULL;
  ubicom32_core_ops.to_insert_vfork_catchpoint = NULL;
  ubicom32_core_ops.to_remove_vfork_catchpoint = NULL;
  ubicom32_core_ops.to_insert_exec_catchpoint = NULL;
  ubicom32_core_ops.to_remove_exec_catchpoint = NULL;
  ubicom32_core_ops.to_reported_exec_events_per_exec_call = NULL;
  ubicom32_core_ops.to_has_exited = NULL;
  ubicom32_core_ops.to_mourn_inferior = NULL;
  ubicom32_core_ops.to_can_run = 0;
  ubicom32_core_ops.to_notice_signals = 0;
  ubicom32_core_ops.to_thread_alive = ubicom32_thread_alive;
  ubicom32_core_ops.to_pid_to_str = ubicom32_thread_pid_to_str;
  ubicom32_core_ops.to_stop = NULL;
  ubicom32_core_ops.to_pid_to_exec_file = NULL;
  ubicom32_core_ops.to_stratum = core_stratum;
  ubicom32_core_ops.to_has_all_memory = 1;
  ubicom32_core_ops.to_has_memory = 1;
  ubicom32_core_ops.to_has_stack = 1;
  ubicom32_core_ops.to_has_registers = 1;
  ubicom32_core_ops.to_has_execution = 1;
  ubicom32_core_ops.to_sections = NULL;
  ubicom32_core_ops.to_sections_end = NULL;
  ubicom32_core_ops.to_magic = OPS_MAGIC;
  ubicom32_core_ops.to_extra_thread_info = ubicom32_threads_extra_info;
}


void
_initialize_ubicom32 (void)
{
  init_ubicom32_ops ();
  init_ubicom32_core_ops ();
  add_target (&ubicom32_ops);
  //add_target (&ubicom32_core_ops);
  add_com ("reset", class_obscure, common_reset, "reset target.");
  /* Backward compatability.  */
  add_com ("ubicom32reset", class_obscure, common_reset,
	   "Backward compatability - use 'reset'.");
  add_com ("flashinfo", class_obscure, ubicom32_flash_info,
	   "Backward compatability - use 'reset'.");
  add_com ("hwmon", class_obscure, ubicom32_hw_mon,
	   "Backward compatability - use 'reset'.");
  add_com ("hwsample", class_obscure, ubicom32_hw_mon_sample,
	   "Used to sample the pc 1000 times every 10ms.");
  add_com ("verify", class_obscure, ubicom32_verify,
	   "Used to verify contents of flash.");
  add_com ("coredump", class_obscure, ubicom32_core_dump,
	   "Used to create a core dump of the machine state.\n\
Usage:\n\
coredump\n\
\tUBICOM32 processor: This will dump core to a file called Ubicom.core \n\twith 16MB of sdram and all of the flash.\n\
\tDump will take 2 minutes to complete.\n\n\
\tUBICOM32V3 processor: This will dump core to a file called Ubicom.core \n\twith 64MB of DDR memory and 4MB of FLASH memory.\n\
\tDump will take 10 minutes to complete.\n\n\
");
  add_com ("ubicom32protect", class_obscure, ubicom32_protect, "Protect the Flash Boot sector.");

  add_com ("ubicom32unprotect", class_obscure, ubicom32_unprotect, "Unprotect the Flash Boot sector.");

  add_com ("loadprotect", class_obscure, ubicom32_load_protect, "Load the \".protect\". section into Flash Boot sector.");
  add_com ("fixprotect", class_obscure, ubicom32_fix_protect, "Load the \".protect\". section into Flash Boot sector via pram.");
  add_com ("mstat", class_obscure, ubicom32_mail_status, "Printout mail box status.");
  add_com ("rdmail", class_obscure, ubicom32_read_mailbox, "Read data from mail box.");
  add_com ("fstat", class_obscure, ubicom32FlashStats, "Print out stats about breakpoints in Flash.");
  //  add_com ("restore", class_obscure, ubicom32RestoreBlock, "Restore a flash block.");

#if 0
  add_com ("mclear", class_obscure, ubicom32_mail_clear, "Load the \".protect\". section into Flash Boot sector.");
#endif
}
