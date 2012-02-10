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
#include "gdbcmd.h"
#include "objfiles.h"
#include "gdb-stabs.h"
#include "gdbthread.h"
#include "remote.h"
#include "regcache.h"
#include "value.h"
#include "gdb_assert.h"
#include "readline/readline.h"

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
#include "remote-ubicom32v3-dbginterface.h"
#include "ubicom32_dongle.h"
#include "elf-bfd.h"
#include "ubicom32-tdep.h"
#include "frame.h"

volatile int ubicom32v3_cntrl_c_abort;
int ubicom32v3_current_thread= -1;
int ubicom32v3currentThread=-1;
int ubicom32DebuggerAssistantThread = -1;
unsigned int dcache_status = 0;
static void ubicom32v3_read_dcache_status(void);

enum ubicom32v3_state remote_ubicom32v3_state = UBICOM32V3_DETACHED;

struct step_interpret {
  unsigned int current_pc;
  unsigned int target1_pc;
  unsigned int target2_pc;
  unsigned int address_register;
  unsigned int stepping_thread;
};

struct step_interpret stepinterpret;
extern unsigned int single_step_buffer_address;
static int ubicom32v3_write_bytes (CORE_ADDR address, char *buffer, int length);
extern CORE_ADDR ubicom32v5_translate_address (struct target_ops *ops, CORE_ADDR addr);

/* GDB structure.  */
struct target_ops ubicom32v3_ops;
struct target_ops ubicom32v3_core_ops;

#define UBICOM32V3_LOAD_BLOCK_SIZE 2048

#define UBICOM32_FLASH_SECTION 1
#define UBICOM32_DDR_SECTION 2
#define UBICOM32_OCM_SECTION 3
#define UBICOM32_OTHER_SECTION 4
#define UBICOM32_MAPPED_SECTION 5

#define REGISTER_RAW_SIZE(x)  (4)

static addrBounds_t ubicom32v3bounds[]={
  {0x800, 0xa00, UBICOM32_OTHER_SECTION},		/* HRT tables */
  {0x01000000, 0x01001000, UBICOM32_OTHER_SECTION},	/* On chip peripherals */
  {0x02000000, 0x02010000, UBICOM32_OTHER_SECTION},	/* IO */
  {0x3ffc0000, 0x40000000, UBICOM32_OCM_SECTION},	/* On chip Memory*/
  {0x40000000, 0x50000000, UBICOM32_DDR_SECTION},	/* DDR*/
  {0x60000000, 0x61000000, UBICOM32_FLASH_SECTION},	/* Flash */
};

static unsigned int ubicom32v3numBoundsEntries = sizeof(ubicom32v3bounds)/sizeof(addrBounds_t);

static addrBounds_t ubicom32v5bounds[]={
  {0x00000000, 0xb0000000, UBICOM32_MAPPED_SECTION},	/* Mapped space */
  {0xb0000000, 0xb1000000, UBICOM32_FLASH_SECTION},	/* Flash */
  {0xb8000800, 0xb8000b00, UBICOM32_OTHER_SECTION},	/* HRT + NRT tables */
  {0xb9000000, 0xb9001000, UBICOM32_OTHER_SECTION},	/* On chip peripherals */
  {0xba000000, 0xba060000, UBICOM32_OTHER_SECTION},	/* IO */
  {0xbffc0000, 0xc0000000, UBICOM32_OCM_SECTION},	/* On chip Memory*/
  {0xc0000000, 0xe0000000, UBICOM32_DDR_SECTION},	/* DDR*/
};

static unsigned int ubicom32v5numBoundsEntries = sizeof(ubicom32v5bounds)/sizeof(addrBounds_t);

void ubicom32v3_supply_registers(struct ubicom32_cpu *cpu, int threadNo, void *regcache);
void ubicom32v4_supply_registers(struct ubicom32_cpu *cpu, int threadNo, void *regcache);
void ubicom32v5_supply_registers(struct ubicom32_cpu *cpu, int threadNo, void *regcache);
void ubicom32v3_tpacket_supply_registers(struct ubicom32_cpu *cpu, int threadNo);
void ubicom32v4_tpacket_supply_registers(struct ubicom32_cpu *cpu, int threadNo);
void ubicom32v5_tpacket_supply_registers(struct ubicom32_cpu *cpu, int threadNo);

static int ignore_trap = 0;

/* ubicom32v3_loadable_section.  */
static int
ubicom32v3_loadable_section(bfd *loadfile_bfd, asection *s)
{
  int length, i;
  CORE_ADDR start, end;
  addrBounds_t *bounds = current_cpu->address_map;
  int numBoundEntries  = current_cpu->num_address_map_entries;

  if (!(s->flags & SEC_LOAD))
    {
      if (strncmp(s->name, ".image", strlen(".image")))
	return 0;
    }

  length = bfd_get_section_size (s);
  if (length == 0)
    return 0;

  start = s->lma;
  end = s->lma + length;

  /* Find the range in the bounds. */
  for (i = 0; i < numBoundEntries; i++) 
    if ((start >= bounds[i].lower) && (end <= bounds[i].upper))
      {
	return bounds[i].type;
      }
  return 0;
}

/* ubicom32v3_lookup_symbol.  */
static unsigned int
ubicom32v3_lookup_symbol (char *name)
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

static void
ubicom32v3_set_thread (int th)
{
  if(th > 0)
    {
      ubicom32v3_current_thread = th;
      ubicom32v3currentThread = th -1;
    }
}

static int
ubicom32v3_thread_alive (ptid_t ptid)
{
  return (1);
}

#define MAGIC_NULL_PID 42000

static char *ubi32cpus[] = {"ubicom32***Unknown Version***", "ubicom32v3", "ubicom32v4", "ubicom32v5"};

/* ubicom32v3_open
   Usage: target remote <ip_addr>:<ip_port>
   Connects to the dongle and then connects to target through the dongle.  */
void
ubicom32v3_open (char *args, int from_tty)
{
  int ret, i;
  struct gdbarch_info info;
  extern void gdbarch_info_init (struct gdbarch_info *info);
  char *env_dongle;
  extern int chipID;
  char *file;
  int elf_flags = -1;
  int ubi32cpus_index = 0;

  if (exec_bfd)
    {
      elf_flags = elf_elfheader (exec_bfd)->e_flags & 0xffff;
    }

  if (elf_flags != -1)
    {
      switch(elf_flags)
	{
	case 1:
	  ubi32cpus_index = 1;
	  break;
	case 2:
	  ubi32cpus_index = 2;
	  break;
	case 4:
	  ubi32cpus_index = 3;
	  break;
	}
    }

  if (remote_debug)
    {
      if (args)
	printf_filtered ("ubicom32v3_open (%s)\n", args);
      else
	printf_filtered ("ubicom32v3_open ()\n");
    }

  target_preopen (from_tty);

  // printf("Current arch is %s\n", TARGET_ARCHITECTURE->printable_name);

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
  ret = ubicom32v3isp_connect (args);

  if(ret == -1)
    error ("Unable to find dongle.");
  else if(ret == -2)
    error ("Dongle has no Ubicom32v3 support. Get a new dongle with Ubicom32v3 support and try again.");


  /*
   * control c turnOffconsole in ubicom32v3_open()
   */
  ret = turnOffconsole();

  switch (chipID)
    {
    case 0x20000:
    case 0x20001:
    case 0x20002:
      current_cpu = &ubicom32_cpus[0];
      current_cpu->supply_register_fn = ubicom32v3_supply_registers;
      current_cpu->supply_tpacket_regs_fn = ubicom32v3_tpacket_supply_registers;
      current_cpu->address_map = ubicom32v3bounds;
      current_cpu->num_address_map_entries = ubicom32v3numBoundsEntries;
      ubicom32PatchDebugCode();
      if (elf_flags != -1)
	{
	  if (elf_flags != bfd_mach_ubicom32dsp)
	    {
	      char *file = bfd_get_filename (exec_bfd);
	      char *slash_location = strrchr(file, '/');

	      if (slash_location == NULL)
		slash_location = file;
	      else
		slash_location++;

	      printf_unfiltered ("\nWarning Mismatch:\n\tFile \"%s\" has been built for %s.\n\tTarget cpu is ubicom32v3.\n\n", slash_location, ubi32cpus[ubi32cpus_index]);
	    }
	}

      break;
    case 0x30000:
    case 0x30001:
      current_cpu = &ubicom32_cpus[1];
      current_cpu->supply_register_fn = ubicom32v4_supply_registers;
      current_cpu->supply_tpacket_regs_fn = ubicom32v4_tpacket_supply_registers;
      current_cpu->address_map = ubicom32v3bounds;
      current_cpu->num_address_map_entries = ubicom32v3numBoundsEntries;
      ubicom32PatchDebugCode();

      if (elf_flags != -1)
	{
	  if (elf_flags != bfd_mach_ubicom32ver4)
	    {
	      char *file = bfd_get_filename (exec_bfd);
	      char *slash_location = strrchr(file, '/');

	      if (slash_location == NULL)
		slash_location = file;
	      else
		slash_location++;

	      printf_unfiltered ("\nWarning Mismatch:\n\tFile \"%s\" has been built for %s.\n\tTarget cpu is ubicom32v4.\n\n", slash_location, ubi32cpus[ubi32cpus_index]);
	    }
	}

      break;
    case 0x40000:
    case 0x40001:
      current_cpu = &ubicom32_cpus[2];
      current_cpu->supply_register_fn = ubicom32v5_supply_registers;
      current_cpu->supply_tpacket_regs_fn = ubicom32v5_tpacket_supply_registers;
      current_cpu->address_map = ubicom32v5bounds;
      current_cpu->num_address_map_entries = ubicom32v5numBoundsEntries;
      ubicom32PatchDebugCode();

      /* Set up translation from virtual to physical addresses.  */
      ubicom32v3_ops.to_translate_address = ubicom32v5_translate_address;

      if (elf_flags != -1)
	{
	  if (elf_flags != bfd_mach_ubicom32ver5)
	    {
	      char *file = bfd_get_filename (exec_bfd);
	      char *slash_location = strrchr(file, '/');

	      if (slash_location == NULL)
		slash_location = file;
	      else
		slash_location++;

	      printf_unfiltered ("\nWarning Mismatch:\n\tFile \"%s\" has been built for %s.\n\tTarget cpu is ubicom32v5.\n\n", slash_location, ubi32cpus[ubi32cpus_index]);
	    }
	}

      break;
    default:
      error("No support for Processor with chip ID 0x%x in this version of the debugger.", chipID);
    }

  /* Attach to target.  */
  ret = ubicom32v3isp_attach ();
  if(ret)
    {
      (void)ubicom32v3isp_close();
      remote_ubicom32v3_state = UBICOM32V3_DETACHED;
      switch(ret)
	{
	case -1:
	  error("Error in reply packets from dongle\n");
	  break;
	case -2:
	  error("Found Ubicom32 processor and not Ubicom32v3 processor\n");
	  break;
	case -3:
	  error("Unable to establish ISD connection\n");
	  break;
	default:
	  error("Unable to connect to target\n");
	  break;
	}
    }

  if (current_gdbarch == NULL)
    {
      char *ptr = NULL;
      gdbarch_info_init (&info);
      switch (chipID)
	{
	case 0x20000:
	case 0x20001:
	case 0x20002:
	  ptr = "ubicom32dsp";
	  break;
	case 0x30000:
	case 0x30001:
	  ptr = "ubicom32ver4";
	  break;
	case 0x40000:
	case 0x40001:
	  ptr = "ubicom32ver5";
	  break;
	}
      info.bfd_arch_info = bfd_scan_arch (ptr);

      if (info.bfd_arch_info == NULL)
	internal_error (__FILE__, __LINE__,
			"set_architecture: bfd_scan_arch failed");
      (void)gdbarch_update_p (info);
    }
  else
    {
      struct gdbarch_tdep *tdep = gdbarch_tdep(current_gdbarch);
      char *arch_string = (char *)tdep->name;
      switch (chipID)
	{
	case 0x20000:
	case 0x20001:
	case 0x20002:
	  if (strcmp(arch_string, "Ubicom32 Version 3"))
	    {
	      /* Architecture mismatch. We need to go find the "ubicom32dsp" arch. */
	      gdbarch_info_init (&info);
	      info.bfd_arch_info = bfd_scan_arch ("ubicom32dsp");

	      if (info.bfd_arch_info == NULL)
		internal_error (__FILE__, __LINE__,
				"set_architecture: bfd_scan_arch failed");
	      (void)gdbarch_update_p (info);
	    }
	  break;
	case 0x30000:
	case 0x30001:
	  if (strcmp(arch_string, "Ubicom32 Version 4"))
	    {
	      /* Architecture mismatch. We need to go find the "ubicom32ver4" arch. */
	      gdbarch_info_init (&info);
	      info.bfd_arch_info = bfd_scan_arch ("ubicom32ver4");

	      if (info.bfd_arch_info == NULL)
		internal_error (__FILE__, __LINE__,
				"set_architecture: bfd_scan_arch failed");
	      (void)gdbarch_update_p (info);
	    }
	  break;
	case 0x40000:
	case 0x40001:
	  if (strcmp(arch_string, "Ubicom32 Version 5"))
	    {
	      /* Architecture mismatch. We need to go find the "ubicom32ver4" arch. */
	      gdbarch_info_init (&info);
	      info.bfd_arch_info = bfd_scan_arch ("ubicom32ver5");

	      if (info.bfd_arch_info == NULL)
		internal_error (__FILE__, __LINE__,
				"set_architecture: bfd_scan_arch failed");
	      (void)gdbarch_update_p (info);
	    }
	  break;
	}
    }
  push_target (&ubicom32v3_ops);		/* Switch to using remote target now */

  remote_ubicom32v3_state = UBICOM32V3_STOPPED;
  inferior_ptid = pid_to_ptid (ubicom32v3_current_thread);
  start_remote(from_tty);
  //inferior_ptid = pid_to_ptid (1);

  /* add the threads to the system */
  for(i=1; i<= current_cpu->num_threads; i++)
    add_thread(pid_to_ptid(i));

  ubicom32v3_read_dcache_status();
}

void
ubicom32v3_supply_registers(struct ubicom32_cpu *cpu, int threadNo, void *rcache)
{
  ubicom32v3Regs_t *mainRegs = (ubicom32v3Regs_t *)cpu->registers;
  int *rptr = &mainRegs->tRegs[threadNo].dr[0];
  int i;
  struct regcache *regcache = (struct regcache *)rcache;
  /* first supply the thread specific registers */
  for(i=0; i< cpu->num_per_thread_regs; i++)
    swizzled_supply_register(regcache, i, rptr++);

  /* Now supply the global registers */
  rptr = (int *)&mainRegs->globals;
  for(;i< (cpu->num_per_thread_regs + cpu->num_global_regs); i++)
    swizzled_supply_register(regcache, i, rptr++);
}

void
ubicom32v4_supply_registers(struct ubicom32_cpu *cpu, int threadNo, void *rcache)
{
  ubicom32v4Regs_t *mainRegs = (ubicom32v4Regs_t *)cpu->registers;
  int *rptr = &mainRegs->tRegs[threadNo].dr[0];
  int i;
  struct regcache *regcache = (struct regcache *)rcache;

  /* first supply the thread specific registers */
  for(i=0; i< cpu->num_per_thread_regs; i++)
    swizzled_supply_register(regcache, i, rptr++);

  /* Now supply the global registers */
  rptr = (int *)&mainRegs->globals;
  for(;i< (cpu->num_per_thread_regs + cpu->num_global_regs); i++)
    swizzled_supply_register(regcache, i, rptr++);
}

void
ubicom32v5_supply_registers(struct ubicom32_cpu *cpu, int threadNo, void *rcache)
{
  ubicom32v5Regs_t *mainRegs = (ubicom32v5Regs_t *)cpu->registers;
  int *rptr = &mainRegs->tRegs[threadNo].dr[0];
  int i;
  struct regcache *regcache = (struct regcache *)rcache;

  /* first supply the thread specific registers */
  for(i=0; i< cpu->num_per_thread_regs; i++)
    swizzled_supply_register(regcache, i, rptr++);

  /* Now supply the global registers */
  rptr = (int *)&mainRegs->globals;
  for(;i< (cpu->num_per_thread_regs + cpu->num_global_regs); i++)
    swizzled_supply_register(regcache, i, rptr++);
}

/* ubicom32v3_fetch_register.  */
static void
ubicom32v3_fetch_register (struct regcache *regcache, int regno)
{
  int i, ret;
  int thread_num = PIDGET (inferior_ptid);

  ubicom32v3_set_thread (thread_num);

  /* grab registers */
  ret = current_cpu->fetch_registers_fn(current_cpu, (thread_num-1));
  if(ret)
    {
      printf_unfiltered("error: Fetch registers for thread %d failed\n", thread_num-1);
      return;
    }

  /* we have the registers. Now supply them */
  current_cpu->supply_register_fn(current_cpu, thread_num-1, regcache);
}

struct ubicom32v3_core_header {
  unsigned int magic;
  unsigned int regOffset;
  unsigned int hrtOffset;
  unsigned int ocmOffset;
  unsigned int ddrOffset;
  unsigned int flashOffset;
};

struct ubicom32v3_cache_dump_header {
  unsigned int dcache_tag_entries;
  unsigned int dcache_data_bytes;
  unsigned int icache_tag_entries;
  unsigned int icache_data_bytes;
};

/* ubicom32v3_attach */
static void
ubicom32v3_attach (char *args, int from_tty)
{
  struct gdbarch_info info;
  extern void gdbarch_info_init (struct gdbarch_info *info);
  int ret = ubicom32v3isp_attach ();

  if(ret)
    {
      switch(ret)
	{
	case -1:
	  error("Error in reply packets from dongle\n");
	  break;
	case -2:
	  error("Found Ubicom32 processor and not Ubicom32v3 processor\n");
	  break;
	case -3:
	  error("Unable to establish ISD connection\n");
	  break;
	default:
	  error("Unable to connect to target\n");
	  break;
	}
      (void)ubicom32v3isp_close();
      remote_ubicom32v3_state = UBICOM32V3_DETACHED;
    }

  remote_ubicom32v3_state = UBICOM32V3_STOPPED;

  if (current_gdbarch == NULL)
    {
      info.bfd_arch_info = bfd_scan_arch ("ubicom32dsp");

      if (info.bfd_arch_info == NULL)
	internal_error (__FILE__, __LINE__,
			"set_architecture: bfd_scan_arch failed");
      (void)gdbarch_update_p (info);
    }
  else
    {
      struct gdbarch_tdep *tdep = gdbarch_tdep(current_gdbarch);
      char *arch_string = (char *)tdep->name;
      extern int chipID;
      switch (chipID)
	{
	case 0x20000:
	case 0x20001:
	case 0x20002:
	  if (strcmp(arch_string, "Ubicom32 Version 3"))
	    {
	      /* Architecture mismatch. We need to go find the "ubicom32dsp" arch. */
	      gdbarch_info_init (&info);
	      info.bfd_arch_info = bfd_scan_arch ("ubicom32dsp");

	      if (info.bfd_arch_info == NULL)
		internal_error (__FILE__, __LINE__,
				"set_architecture: bfd_scan_arch failed");
	      (void)gdbarch_update_p (info);
	    }
	  break;
	}
    }

  ubicom32v3_read_dcache_status();
}

/* ubicom32v3_detach.
   Detach from the remote board.  */
static void
ubicom32v3_detach (char *args, int from_tty)
{
  int ret;
  // toggleMtEn();

  (void) ubicom32v3isp_detach();
  (void) ubicom32v3isp_close();
  remote_ubicom32v3_state = UBICOM32V3_DETACHED;
  pop_target();

}

/* ubicom32v3_close.  */
static void
ubicom32v3_close (int quitting)
{
  if(remote_ubicom32v3_state == UBICOM32V3_DETACHED)
    return;

  if (remote_debug)
    printf_filtered ("ubicom32v3_close (%d)\n", quitting);

  /* Disconnect.  */
  (void)ubicom32v3isp_close();
  remote_ubicom32v3_state = UBICOM32V3_DETACHED;
}

/* ubicom32v3_cntrl_c.  */
static void
ubicom32v3_cntrl_c (int signo)
{
  ubicom32v3_cntrl_c_abort = 1;
}

/* ubicom32v3_files_info.  */
static void
ubicom32v3_files_info (struct target_ops *target)
{
  if (exec_bfd)
    {
      char *file = bfd_get_filename (exec_bfd);
      printf_unfiltered ("Debugging %s\n", file);
    }
  else
    printf_unfiltered ("No file loaded\n");
}

/* ubicom32v3_stop.
   Notify the target of an asynchronous request to stop.  */
static void
ubicom32v3_stop (void)
{
  if (remote_debug)
    printf_filtered ("ubicom32v3_stop\n");

  ubicom32v3_cntrl_c_abort = 1;
}

static char *
ubicom32v3_thread_pid_to_str (ptid_t ptid)
{
  static char buf[30];

  sprintf (buf, "Thread %d", (PIDGET (ptid))-1);
  return buf;
}

static void
ubicom32v3_store_register(struct regcache *regcache, int regnum)
{
  char *regs = malloc(REGISTER_RAW_SIZE(regnum));
  int thread_num = PIDGET (inferior_ptid);
  int ret;

  ubicom32v3_set_thread(thread_num);

  if(regnum >= 0)
    {
      /* pull the data from regcache */
      regcache_raw_collect (regcache, regnum, regs);

      swizzle_for_write_register (regnum, regs);
      ret = current_cpu->write_register_fn((thread_num-1), regnum, (int *) regs, 1);
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
ubicom32v3_prepare_to_store (struct regcache *regcache)
{
  int i;
  gdb_byte buf[MAX_REGISTER_SIZE];

  /* Make sure all the necessary registers are cached.  */
  for (i = 0; i < gdbarch_num_regs(current_gdbarch); i++)
    regcache_raw_read (regcache, i, buf);
}

void
ubicom32v3_tpacket_supply_registers(struct ubicom32_cpu *cpu, int threadNo)
{
  ubicom32v3Regs_t *mainRegs = (ubicom32v3Regs_t *)cpu->registers;
  struct regcache *current_regcache = get_current_regcache();
  swizzled_supply_register(current_regcache, 0, &mainRegs->tRegs[ubicom32v3currentThread].dr[0]);
  swizzled_supply_register(current_regcache, 21, &mainRegs->tRegs[ubicom32v3currentThread].ar[5]);
  swizzled_supply_register(current_regcache, 22, &mainRegs->tRegs[ubicom32v3currentThread].ar[6]);
  swizzled_supply_register(current_regcache, 23, &mainRegs->tRegs[ubicom32v3currentThread].ar[7]);
  swizzled_supply_register(current_regcache, 30, &mainRegs->tRegs[ubicom32v3currentThread].rosr);
  swizzled_supply_register(current_regcache, 34, &mainRegs->tRegs[ubicom32v3currentThread].threadPc);
  swizzled_supply_register(current_regcache, 47, &mainRegs->globals.mt_active);
  swizzled_supply_register(current_regcache, 50, &mainRegs->globals.mt_dbg_active);
  swizzled_supply_register(current_regcache, 52, &mainRegs->globals.mt_en);
}

void
ubicom32v4_tpacket_supply_registers(struct ubicom32_cpu *cpu, int threadNo)
{
  ubicom32v4Regs_t *mainRegs = (ubicom32v4Regs_t *)cpu->registers;
  struct regcache *current_regcache = get_current_regcache();
  swizzled_supply_register(current_regcache, 0, &mainRegs->tRegs[ubicom32v3currentThread].dr[0]);
  swizzled_supply_register(current_regcache, 21, &mainRegs->tRegs[ubicom32v3currentThread].ar[5]);
  swizzled_supply_register(current_regcache, 22, &mainRegs->tRegs[ubicom32v3currentThread].ar[6]);
  swizzled_supply_register(current_regcache, 23, &mainRegs->tRegs[ubicom32v3currentThread].ar[7]);
  swizzled_supply_register(current_regcache, 30, &mainRegs->tRegs[ubicom32v3currentThread].rosr);
  swizzled_supply_register(current_regcache, 34, &mainRegs->tRegs[ubicom32v3currentThread].threadPc);
  swizzled_supply_register(current_regcache, 47, &mainRegs->globals.mt_active);
  swizzled_supply_register(current_regcache, 50, &mainRegs->globals.mt_dbg_active);
  swizzled_supply_register(current_regcache, 52, &mainRegs->globals.mt_en);
}

void
ubicom32v5_tpacket_supply_registers(struct ubicom32_cpu *cpu, int threadNo)
{
  ubicom32v5Regs_t *mainRegs = (ubicom32v5Regs_t *)cpu->registers;
  struct regcache *current_regcache = get_current_regcache();
  swizzled_supply_register(current_regcache, 0, &mainRegs->tRegs[ubicom32v3currentThread].dr[0]);
  swizzled_supply_register(current_regcache, 21, &mainRegs->tRegs[ubicom32v3currentThread].ar[5]);
  swizzled_supply_register(current_regcache, 22, &mainRegs->tRegs[ubicom32v3currentThread].ar[6]);
  swizzled_supply_register(current_regcache, 23, &mainRegs->tRegs[ubicom32v3currentThread].ar[7]);
  swizzled_supply_register(current_regcache, 30, &mainRegs->tRegs[ubicom32v3currentThread].rosr);
  swizzled_supply_register(current_regcache, 35, &mainRegs->tRegs[ubicom32v3currentThread].threadPc);
  swizzled_supply_register(current_regcache, 52, &mainRegs->globals.mt_active);
  swizzled_supply_register(current_regcache, 56, &mainRegs->globals.mt_dbg_active);
  swizzled_supply_register(current_regcache, 57, &mainRegs->globals.mt_en);
}

/* ubicom32v3_wait */
static ptid_t
ubicom32v3_wait (ptid_t ptid, struct target_waitstatus *status)
{
  bpReason_t reason;
  int mask;

  static RETSIGTYPE (*prev_sigint) ();
  int ret;

  if (remote_debug)
    printf_filtered ("ubicom32v3_wait\n");

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  if(remote_ubicom32v3_state == UBICOM32V3_STEPPING_PROBLEMS)
    {
      /* single stepping was attempted on a suspended thread */
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_STOP;
      remote_ubicom32v3_state = UBICOM32V3_STOPPED;
      ubicom32v3_read_dcache_status();

      return pid_to_ptid (ubicom32v3_current_thread);
    }
  else if((remote_ubicom32v3_state == UBICOM32V3_STOPPED) || (remote_ubicom32v3_state == UBICOM32V3_CORE_STOPPED))
    {
      /* Things are already stopped */
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_TRAP;
      if (remote_ubicom32v3_state != UBICOM32V3_CORE_STOPPED) 
	{
	  ubicom32v3_read_dcache_status();

	  /*
	   * Stop the watchdog.
	   */
	  ret = ubicom32v3StopWatchdog();
	  ret = ubicom32v3DisableLeakageEnable();
	}
      return pid_to_ptid (ubicom32v3_current_thread);
    }

  ubicom32v3_cntrl_c_abort = 0;
  prev_sigint = signal (SIGINT, ubicom32v3_cntrl_c);
  while (1)
    {
      if (ubicom32v3_cntrl_c_abort)
	{
	  /*
	   * control c turnOffconsole in ubicom32v3_wait()
	   */
	  int ret2 = turnOffconsole();

	  ret = ubicom32v3stopProcessor();
	  if(ret)
	    {
	      printf_unfiltered("error: Could not stop the processor\n");
	      signal (SIGINT, prev_sigint);
	      return null_ptid;
	    }

	  reason.reason = BREAKPOINT_HIT;
	  break;
	}
      else
	{
	  if (ignore_trap)
	    {
	      /*
	       * waitForBPnoTrap turnOffconsole in ubicom32v3_wait()
	       */
	      int ret2 = turnOffconsole();

	      /* call waitForBPNoTrap and see if we get anything */
	      ret = ubicom32v3waitForBPNoTrap(&reason);
	    }
	  else
	    {
	      /*
	       * waitForBP turnOffconsole in ubicom32v3_wait()
	       */
	      int ret2 = turnOffconsole();

	      /* call waitForBP and see if we get anything */
	      ret = ubicom32v3waitForBP(&reason);
	    }

	  if(ret< 0)
	    {
	      printf_unfiltered ("error: unable to read status\n");
	      signal (SIGINT, prev_sigint);
	      return null_ptid;
	    }
	  if(ret == 0)
	    {

	      /*
	       * not any BP hit turnOnconsole in ubicom32v3_wait()
	       */
	      int ret2 = turnOnconsole();


	      /* wait for 10 ms before trying again */
	      usleep(10000);
	      continue;
	    }

	  break;
	}
    }

  /* go retrieve the registers for a TPacket */
  ret = ubicom32v3createTpacketMP(ubicom32v3_current_thread-1);
  if(ret)
    {
      printf_unfiltered("error: Could not retrieve registers after stopping the processor\n");
      signal (SIGINT, prev_sigint);
      return null_ptid;
    }

  /* Supply the tpacket registers. */
  current_cpu->supply_tpacket_regs_fn(current_cpu, ubicom32v3_current_thread-1);

  /* XXXXXXXXXxx Rewrite */
  switch(reason.reason)
    {
    case TRAP:
      {
	printf_unfiltered("TRAP in thread %d status 0x%x\n", ubicom32v3currentThread, reason.status);

	if (reason.status & (1 << 0)) {
	  printf_unfiltered("TRAP cause: Instruction address decode error.\n");
	}
	else if (reason.status & (1 << 1)) {
	  printf_unfiltered("TRAP cause: Instruction synchronous error.\n");
	}
	else if (reason.status & (1 << 2)) {
	  printf_unfiltered("TRAP cause: Illegal instruction.\n");
	}
	else if (reason.status & (1 << 3)) {
	  printf_unfiltered("TRAP cause: Source 1 address decode error.\n");
	}
	else if (reason.status & (1 << 4)) {
	  printf_unfiltered("TRAP cause: Destination address decode error.\n");
	}
	else if (reason.status & (1 << 5)) {
	  printf_unfiltered("TRAP cause: Source 1 operand alignment error.\n");
	}
	else if (reason.status & (1 << 6)) {
	  printf_unfiltered("TRAP cause: Destination oprand alignment error.\n");
	}
	else if (reason.status & (1 << 7)) {
	  printf_unfiltered("TRAP cause: Source 1 synchronous error.\n");
	}
	else if (reason.status & (1 << 8)) {
	  printf_unfiltered("TRAP cause: Destination synchrouous error.\n");
	}
	else if (reason.status & (1 << 9)) {
	  printf_unfiltered("TRAP cause: Data Capture.\n");
	}
	else if (reason.status & (1 << 10)) {
	  printf_unfiltered("TRAP cause: Instruction fetch memory protection error.\n");
	}
	else if (reason.status & (1 << 11)) {
	  printf_unfiltered("TRAP cause: Source 1 memory protection error.\n");
	}
	else if (reason.status & (1 << 12)) {
	  printf_unfiltered("TRAP cause: Destination memory protection error.\n");
	}
	else if (reason.status & (1 << 13)) {
	  printf_unfiltered("TRAP cause: Privilege violation.\n");
	}
	status->kind = TARGET_WAITKIND_STOPPED;
	status->value.sig = TARGET_SIGNAL_STOP;
	remote_ubicom32v3_state = UBICOM32V3_TRAP_ERROR;

	break;
      }
    case HALT:
      {
	if (reason.status & (1 << 2)) {
	  printf_unfiltered("HALT cause: Watchdog timer reset.\n");
	}
	if (reason.status & (1 << 4)) {
	  printf_unfiltered("HALT cause: Software reset.\n");
	}
	if (reason.status & (1 << 5)) {
	  printf_unfiltered("HALT cause: Instruction port async error.\n");
	}
	if (reason.status & (1 << 6)) {
	  printf_unfiltered("HALT cause: Data port asnyc error.\n");
	}
	if (reason.status & (1 << 7)) {
	  printf_unfiltered("HALT cause: Instruction address decode error.\n");
	}
	if (reason.status & (1 << 8)) {
	  printf_unfiltered("HALT cause: Instruction synchronous error.\n");
	}
	if (reason.status & (1 << 9)) {
	  printf_unfiltered("HALT cause: Illegal instruction.\n");
	}
	if (reason.status & (1 << 10)) {
	  printf_unfiltered("HALT cause: Soruce 1 address decode error.\n");
	}
	if (reason.status & (1 << 11)) {
	  printf_unfiltered("HALT cause: Destination address decode error.\n");
	}
	if (reason.status & (1 << 12)) {
	  printf_unfiltered("HALT cause: Source 1 operand alignment error.\n");
	}
	if (reason.status & (1 << 13)) {
	  printf_unfiltered("HALT cause: Destination oprand alignment error.\n");
	}
	if (reason.status & (1 << 14)) {
	  printf_unfiltered("HALT cause: Source 1 synchronous error.\n");
	}
	if (reason.status & (1 << 15)) {
	  printf_unfiltered("HALT cause: Destination synchrouous error.\n");
	}
	if (reason.status & (1 << 16)) {
	  printf_unfiltered("HALT cause: Write address watchpoint.\n");
	}
	if (reason.status & (1 << 17)) {
	  printf_unfiltered("HALT cause: Source 1 memory protection error.\n");
	}
	if (reason.status & (1 << 18)) {
	  printf_unfiltered("TRAP cause: Destination memory protection error.\n");
	}
	status->kind = TARGET_WAITKIND_STOPPED;
	status->value.sig = TARGET_SIGNAL_STOP;
	remote_ubicom32v3_state = UBICOM32V3_TRAP_ERROR;

	break;
      }

    default:
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_TRAP;
      remote_ubicom32v3_state = UBICOM32V3_STOPPED;
    }
  signal (SIGINT, prev_sigint);

  ubicom32v3_read_dcache_status();

  return pid_to_ptid (ubicom32v3_current_thread);
}

/* Resume execution of the target process.  STEP says whether to single-step
   or to run free; SIGGNAL is the signal value (e.g. SIGINT) to be given
   to the target, or zero for no signal.  */
static void
ubicom32v3_resume (ptid_t ptid, int step, enum target_signal siggnal)
{
  int thread_num = ptid_get_pid (inferior_ptid);
  int mask, i;
  CORE_ADDR pc;
  extern unsigned int  mt_active, mt_en;

  ubicom32v3_set_thread(thread_num);
  thread_num--;
  mask = 1<< ubicom32v3currentThread;

  if (remote_debug)
    printf_filtered ("ubicom32v3_resume\n");

  pc = read_pc();

  dcache_status = 0;

 //inferior_ptid = ptid;

  if (step)
    {
      int ret;
      thread_num = PIDGET (inferior_ptid);

      ubicom32v3_set_thread(thread_num);

      /* we are single stepping the main processor */
      if(!(mt_en & mask))
	{
	  // Trying to single step a Disabled thread
	  printf_unfiltered("error: Single Stepping a Disabled thread.\n");
	  remote_ubicom32v3_state = UBICOM32V3_STEPPING_PROBLEMS;
	}
      else if (!(mt_active & mask))
	{
	  printf_unfiltered("error: Single Stepping a Suspended thread.\n");
	  remote_ubicom32v3_state = UBICOM32V3_STEPPING_PROBLEMS;
	}
      if(remote_ubicom32v3_state == UBICOM32V3_STEPPING_PROBLEMS)
	return;

      ret = ubicom32v3singleStep(ubicom32v3currentThread);

      if(remote_ubicom32v3_state == UBICOM32V3_STOPPED)
	remote_ubicom32v3_state = UBICOM32V3_STEPPING;
    }
  else
    {
      /* let everything run */
      int ret;

      /*
       * Restart watchdog.
       */
      (void) ubicom32v3RestartWatchdog();
      (void) ubicom32v3RestoreLeakageEnable();

      ret= ubicom32v3restartProcessor(1);

      /*
       * after resume command to run or continue call turnOnconsole in ubicom32v3_resume()
       */
      ret = turnOnconsole();

      remote_ubicom32v3_state = UBICOM32V3_RUNNING;

      /* Invalidate MMU & PGD status.  */
      mmuRead = 0;
      pgd_thread = 0;

    }
}

static int
ubicom32v3_write_bytes (CORE_ADDR address, char *buffer, int length)
{
  // split the transfer into lead, aligned middle and end
  unsigned truncBytes = (address & 0x3);
  unsigned truncAddress = address & ~0x3;
  unsigned int leadBytes, leadAddr, midBytes, midAddr, endBytes, endAddr, leadIndex;
  unsigned toWriteWords;
  unsigned char  *sendBuffer;
  unsigned char *freeBuffer;
  unsigned char *cptr;
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

  toWriteWords = midBytes/4;

  if(leadBytes)
    toWriteWords++;

  if(endBytes)
    toWriteWords++;

  /* Allocate the send buffer */
  sendBuffer = (unsigned char *)malloc(toWriteWords*4);
  freeBuffer = sendBuffer;
  cptr = sendBuffer;

  if(leadBytes)
    {
      // Misaligned start. Deal with it by first backing up the address to nearest
      // Go read 4 bytes from the backed up address
      ret = ubicom32v3readMemory(leadAddr, 1, (int *)cptr);

      for(i=0; i<leadBytes; i++)
	{
	  cptr[leadIndex--] = (unsigned char) *buffer++;
	}

      // Bump up cptr by 4
      cptr += 4;
    }

  // Deal with the midsection if any
  if(midBytes)
    {
      // Move the data into the transfer buffer
      for(i=0; i< midBytes; i+= 4)
	{
	  for(j=0; j< 4; j++)
	    {
	      cptr[3-j] = (*buffer++ &0xff);
	    }
	  cptr+= 4;
	}
    }

  if(endBytes)
    {
      // trailing cruft to deal with
      // Go read 4 bytes from the backed up end address
      ret = ubicom32v3readMemory(endAddr, 1, (int *)cptr);

      for(i=0; i< endBytes; i++)
	{
	  cptr[3-i] = (unsigned char) *buffer++;
	}
    }

  // Send the data to target
  ret = ubicom32v3writeMemory(leadAddr, toWriteWords, (int *)sendBuffer);
  //ret = ubicom32v3cacheflushinvalidate(leadAddr, toWriteWords*4);

  free (freeBuffer);
  return (leadBytes + midBytes + endBytes);
}

static int
ubicom32v3_read_bytes (CORE_ADDR address, char *buffer, int length)
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

  ret = ubicom32v3readMemory(leadAddr, toReadWords, (int *)recvBuffer);

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

static void
ubicom32v3_read_dcache_status(void)
{
  CORE_ADDR address = current_cpu->dcache_control_status_reg;
  char *dptr , temp[4];
  (void) ubicom32v3_read_bytes(address, (char *)temp, sizeof(dcache_status));

  /* need to swizzle to look correct on a little endian machine. */
  dptr = (char *)&dcache_status;
  *dptr++ = temp[3];
  *dptr++ = temp[2];
  *dptr++ = temp[1];
  *dptr++ = temp[0];
}


#if 0
static char *copyvbuf = NULL;
static int
ubicom32v3_slow_verify_bytes (CORE_ADDR address, char *buffer, int length)
{
  // split the transfer into lead, aligned middle and end
  unsigned truncBytes = (address & 0x3);
  unsigned truncAddress = address & ~0x3;
  unsigned int leadBytes, leadAddr, midBytes, midAddr, endBytes, endAddr, leadIndex;
  unsigned toVerifyWords;
  unsigned char cptr[4];
  int i, j, ret;

  if(copyvbuf == NULL)
    copyvbuf = malloc(UBICOM32V3_LOAD_BLOCK_SIZE);

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
      int ret, i;
      char *vbuf = copyvbuf;
      //ret = ubicom32v3_fast_read_bytes(midAddr,  (char *)vbuf, toVerifyWords*4 );
      ret = ubicom32v3UltraFastReadMemoryRaw(midAddr, toVerifyWords, (int *)vbuf);
      if (memcmp (vbuf, buffer, toVerifyWords * 4) != 0)
	{
	  for(i=0; i< midBytes; i+=4, vbuf+=4, buffer+=4)
	    {
	      if(vbuf[0] != buffer[0] ||
		 vbuf[1] != buffer[1] ||
		 vbuf[2] != buffer[2] ||
		 vbuf[3] != buffer[3])
		{
		  printf_unfiltered("Mismatch at 0x%x Expected 0x%02x %02x %02x %02x got 0x%02x %02x %02x %02x\n",
				    (midAddr+i),(unsigned char ) buffer[0], (unsigned char ) buffer[1], (unsigned char ) buffer[2], (unsigned char ) buffer[3],
				    (unsigned char ) vbuf[3], (unsigned char ) vbuf[2], (unsigned char ) vbuf[1], (unsigned char) vbuf[0]);
		  return -1;
		}
	    }
	}
      else
	{
	  buffer += toVerifyWords*4;
	}
    }

  if(endBytes)
    {
      // trailing cruft to deal with
      // Go read 4 bytes from the backed up end address
      ret = readPgmMemory(endAddr, 1, (int *)cptr);
      for(i=0; i< endBytes; i++)
	{
	  if (cptr[3-i] != (unsigned char) *buffer++)
	    return -1;
	}
    }

  return (leadBytes + midBytes + endBytes);
}
#endif

/* ARGSUSED */
static int
ubicom32v3_xfer_memory (CORE_ADDR targ_addr, gdb_byte *buffer, int mem_len,
		    int should_write, struct mem_attrib *attrib,
		    struct target_ops *target)
{
  int targ_len;
  int res;
  addrBounds_t *bounds = current_cpu->address_map;
  int numBoundsEntries = current_cpu->num_address_map_entries;
  int threadNo = inferior_ptid.pid;

  targ_len = mem_len;

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
	  if(should_write && (bounds[res].type == UBICOM32_FLASH_SECTION))
	    {
	      /* flash write attempt and that is a no no */
	      printf("Flash write is not allowed.\n");
	      return 0;
	    }

	  // Valid range. Do an upper bound check and adjust length if needed
	  if(targ_len+targ_addr > bounds[res].upper)
	    targ_len = bounds[res].upper - targ_addr;

	  break;
	}
    }

  // I dcache_status indicates error then see if the address is a DDR/FLASH address
  if(dcache_status & 0x1)
    {
      // dcache error. Disallow DDR/FLASH operations.
      if ((bounds[res].type == UBICOM32_DDR_SECTION) || (bounds[res].type == UBICOM32_FLASH_SECTION))
	{
	  // Flash DDR range. Do no allow the transaction.
	  printf_unfiltered("D-Cache in error state. DDR and Flash access is not allowed.\n");
	  return -1;
	}
    }

  if (should_write)
    {
      res = ubicom32v3_write_bytes (targ_addr, buffer, targ_len);
    }
  else
    res = ubicom32v3_read_bytes (targ_addr, buffer, targ_len);

  return res;
}

/* ubicom32v3_hw_mon */
static void
ubicom32v3_special_mon (char *args, int from_tty)
{
  unsigned int object_address, event_address, event_pattern, event_mask;
  int scanf_ret, ret;
  unsigned int resBuf[32];

  static RETSIGTYPE (*prev_sigint) ();

  /* scan the args for object, event, event_pat and event_mask */
  scanf_ret = sscanf(args, "0x%x 0x%x 0x%x 0x%x", &object_address, &event_address, &event_pattern, &event_mask);

  printf_unfiltered("object_addr =0x%x event_addr =0x%x event_pattern = 0x%x event_pattern = 0x%x\n",
		    object_address, event_address, event_pattern, event_mask);

  if(scanf_ret != 4)
    {
      printf("Usage: specialmon 0xobject_address 0xevent_address 0xevent_pattern 0xevent_mask\n");
      return;
    }

  /* set up the hardware monitor and get it cranking */
  ret = ubicom32v3hw_monitor_setup(object_address, 1, event_address, event_pattern, event_mask, 32);

  if(ret == -4)
    {
      error("No existing connection.\n Issue command: ubicom32v3hwmon hostname:PORT#\n");
    }

  /* time to monitor the status */
  ubicom32v3_cntrl_c_abort = 0;
  prev_sigint = signal (SIGINT, ubicom32v3_cntrl_c);
  while (1)
    {
      unsigned int status;
      if (ubicom32v3_cntrl_c_abort)
	{
	  printf_unfiltered("Aborting monitoring\n");
	  signal (SIGINT, prev_sigint);
	  return;
	}

      ret = ubicom32v3isp_hw_monitor_status(&status, resBuf, 32);
      if(ret) {
	error("Monitor status command bombed\n");
      }

      if(status == 1)
	break;
    }

  /* I you got here monitor has triggered */
  for(ret =0; ret< 32; ret++)
    {
      printf("0x%08x\n", resBuf[ret]);
    }
}

static void
ubicom32v3_cache_flush (char *args, int from_tty)
{
  int scanf_ret;
  unsigned int address;

  scanf_ret = 0;

  if(args)
    {
      scanf_ret = sscanf(args, "0x%x", &address);
    }

  if(scanf_ret == 0)
    {
      printf("Usage cacheflush 0xaddress\n");
      return;
    }

  scanf_ret = ubicom32v3cacheinvalidate(address, 4);
}

static void
ubicom32v3_bist_test (char *args, int from_tty)
{
  int ret;
  if (!args)
    args = "localhost:5010";

  /* Connect to dongle.  */
  ret = ubicom32v3isp_connect (args);

  if(ret == -1)
    error ("Unable to find dongle.");
  else if(ret == -2)
    error ("Dongle has no Ubicom32v3 support. Get a new dongle with Ubicom32v3 support and try again.");


  /* Attach to target.  */
  ret = ubicom32v3isp_attach ();

  ret = ubicom32v3isp_bist();
}

static void
ubicom32v3_previous_pc (char *args, int from_tty)
{
  int thread_num = ubicom32v3currentThread;
  extern int chipID;
  switch (chipID)
    {
    case 0x20000:
    case 0x20001:
    case 0x20002:
      ubicom32v3Registers.tRegs[thread_num].threadPc = ubicom32v3Registers.tRegs[thread_num].previous_pc;
      break;
    case 0x30000:
    case 0x30001:
      ubicom32v4Registers.tRegs[thread_num].threadPc = ubicom32v4Registers.tRegs[thread_num].previous_pc;
      break;
    }

  remote_ubicom32v3_state = UBICOM32V3_CORE_STOPPED;
  inferior_ptid = pid_to_ptid (ubicom32v3_current_thread);
  start_remote(from_tty);

  /*
  ubicom32v3_fetch_register(get_current_regcache(), 0);
  select_frame(get_current_frame());
  */
}

static void
ubicom32v3_ddr_test (char *args, int from_tty)
{
  /* malloc a Meg of data and fill it with random numbers. */
  int *sptr = (int *)malloc(0x100000);

  int i, ret;
  struct timeval begin, end;
  double time;
  char *cptr1, *cptr2;

  /* malloc a Meg of data for verification.. */
    int *dptr = (int *)malloc(0x100000);

  srandom (getpid());

  for(i=0; i< 0x100000/4; i++)
    {
      sptr[i] = random();
    }


  /* set up the fast downloader */
  ret = ubicom32v3isp_download_fast_transfer_code(current_cpu->ocm_start);
  if (ret)
    {
      printf_unfiltered("Failed downloading fast transfer code.\n");
      return;
    }

  /* load the begin time structure */
  (void) gettimeofday(&begin, NULL);

  /* write 10 Megs */
  for(i=0; i< 10; i++)
    {
      int ret = ubicom32v3UltrafastWriteMemory(current_cpu->ddr_start, 0x100000/4, (int *)sptr);
      if (ret) {
	printf_unfiltered("Fast Write bombed\n");
      }
    }
  /* load the end time structure */
  (void) gettimeofday(&end, NULL);

  /* calculate elapsed time */
  time = ((double)end.tv_sec + (double)end.tv_usec/1000000.0) - ((double)begin.tv_sec + (double)begin.tv_usec/1000000.0);

  printf_unfiltered ("Elapsed time in sec for 10M DDr write = %.2lf\n", time);

  for(i = 0; i< 0x100000/4; i++)
    {
      unsigned int send = sptr[i];
      cptr1 = (char *)&sptr[i];
      cptr2 = (char *) &send;

      /* swizzle the data */
      cptr1[0] = cptr2[3];
      cptr1[1] = cptr2[2];
      cptr1[2] = cptr2[1];
      cptr1[3] = cptr2[0];
    }

  ret = ubicom32v3UltraFastReadMemoryRaw(current_cpu->ddr_start, 0x100000/4, (int *)dptr);
  if (memcmp (sptr, dptr, 0x100000) != 0)
    {
      printf_unfiltered("Data mismatch\n");
    }
  else
    {
      printf_unfiltered("Success\n");
    }
}

static void
ubicom32v3_pll_test (char *args, int from_tty)
{
  int ret;
  if (!args)
    args = "localhost:5010";

  /* Connect to dongle.  */
  ret = ubicom32v3isp_connect (args);

  if(ret == -1)
    error ("Unable to find dongle.");
  else if(ret == -2)
    error ("Dongle has no Ubicom32v3 support. Get a new dongle with Ubicom32v3 support and try again.");


  /* Attach to target.  */
  ret = ubicom32v3isp_attach ();
  ret = ubicom32v3isp_reset();
  ret = ubicom32v3isp_pll_init();
}

/* ubicom32v3_show_load_progress_default.  */
static void
ubicom32v3_show_load_progress_default (const char *section,
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

/*
 * ubicom32v3_cache_dump()
 *   This routine will dump the cache and store it in a file called Ubicom.cache.
 *    XXXX We will need a way to to be able to find out which version of the chip we
 *    are dumping the cache for. I suppose CHIP_ID could be used to figure that out.
 *    For the time being we will dump 256 Tag lines and 8192 bytes for dcache.
 *    We will dump 512 Tag lines and 16384 bytes of icache.
 */

void
ubicom32v3_cache_dump()
{
  extern int chipID;
  struct ubicom32v3_cache_dump_header cache_header;
  FILE *fp;
  char filename[] = "Ubicom.cache";
  unsigned char *buffer;
  int ret;
  extern int ubicom32v3readcachetags(unsigned int startAddr, unsigned int numcachelines, unsigned int cache, unsigned char *buffer);
  extern int ubicom32v3readcachedata(unsigned int startAddr, unsigned int numdata, unsigned int cache, unsigned char *buffer);

  switch (chipID)
    {
    case 0x20000:
    case 0x20001:
    case 0x20002:
      {
	/* Set up the header of a Ubicom32v3 with 8k Dcache and 16k Icache. */
	cache_header.dcache_tag_entries = 256;
	cache_header.dcache_data_bytes  = 256*32;
	cache_header.icache_tag_entries = 512;
	cache_header.icache_data_bytes  = 512*32;

	/* Allocate the buffer through which we will stage the data. */
	buffer = malloc(512*32);
	if (buffer == NULL)
	  {
	    printf_unfiltered("Unable to allocate buffer for staging data. Cache not dumped.\n");
	    return;
	  }
      }
      break;
    case 0x30000:
    case 0x30001:
      {
	/* Set up the header of a IP&k with 16k Dcache and 16k Icache. */
	cache_header.dcache_tag_entries = 512;
	cache_header.dcache_data_bytes  = 512*32;
	cache_header.icache_tag_entries = 512;
	cache_header.icache_data_bytes  = 512*32;

	/* Allocate the buffer through which we will stage the data. */
	buffer = malloc(512*32);
	if (buffer == NULL)
	  {
	    printf_unfiltered("Unable to allocate buffer for staging data. Cache not dumped.\n");
	    return;
	  }
      }
    case 0x40000:
    case 0x40001:
      {
	/* Set up the header of a IP&k with 16k Dcache and 16k Icache. */
	cache_header.dcache_tag_entries = 2048;
	cache_header.dcache_data_bytes  = 2048*32;
	cache_header.icache_tag_entries = 2048;
	cache_header.icache_data_bytes  = 2048*32;

	/* Allocate the buffer through which we will stage the data. */
	buffer = malloc(2048*32);
	if (buffer == NULL)
	  {
	    printf_unfiltered("Unable to allocate buffer for staging data. Cache not dumped.\n");
	    return;
	  }
      }
      break;
    default:
      {
	printf_unfiltered ("Unknown Processor. No cache dumped.");
	return;
      }
    }

  /* Open a Ubicom.cache to save the cache data. */
  fp = fopen(filename, "wb+");

  if (fp == NULL)
    {
      printf_unfiltered("Unable to open %s. Cache not dumped.\n", filename);
      free(buffer);
      return;
    }

    /* Write the header out to the file. */
    fwrite(&cache_header, 1, sizeof(cache_header), fp);

    /* Dump dcache tags. */
    ret = ubicom32v3readcachetags(0, cache_header.dcache_tag_entries, 0, buffer);
    if (ret)
      {
	/* Extraction bombed. */
	printf_unfiltered("Dcache TAG dump bombed.\n");
	fclose(fp);
	free(buffer);
	return;
      }

    /* write out the TAG data to file. */
    fwrite(buffer, 1, cache_header.dcache_tag_entries*4, fp);

    /* Dump dcache data. */
    ret = ubicom32v3readcachedata(0, cache_header.dcache_data_bytes, 0, buffer);
    if (ret)
      {
	/* Extraction bombed. */
	printf_unfiltered("Dcache DATA dump bombed.\n");
	free(buffer);
	fclose(fp);
	return;
      }

    /* write out the data to file. */
    fwrite(buffer, 1, cache_header.dcache_data_bytes, fp);

    /* Dump icache tags. */
    ret = ubicom32v3readcachetags(0, cache_header.icache_tag_entries, 1, buffer);
    if (ret)
      {
	/* Extraction bombed. */
	printf_unfiltered("Icache TAG dump bombed.\n");
	free(buffer);
	fclose(fp);
	return;
      }

    /* write out the TAG data to file. */
    fwrite(buffer, 1, cache_header.icache_tag_entries*4, fp);

    /* Dump icache data. */
    ret = ubicom32v3readcachedata(0, cache_header.icache_data_bytes, 1, buffer);
    if (ret)
      {
	/* Extraction bombed. */
	printf_unfiltered("Icache DATA dump bombed.\n");
	free(buffer);
	fclose(fp);
	return;
      }

    /* write out the data to file. */
    fwrite(buffer, 1, cache_header.icache_data_bytes, fp);

    /* Close the file. */
    fclose(fp);
    free(buffer);
}

static void
make_output_phdrs (bfd *obfd, asection *osec, void *ignored)
{
  int p_flags = 0;
  int p_type;

  /* FIXME: these constants may only be applicable for ELF.  */
  if (strncmp (bfd_section_name (obfd, osec), "load", 4) == 0)
    p_type = PT_LOAD;
  else
    p_type = PT_NOTE;

  p_flags |= PF_R;	/* Segment is readable.  */
  if (!(bfd_get_section_flags (obfd, osec) & SEC_READONLY))
    p_flags |= PF_W;	/* Segment is writable.  */
  if (bfd_get_section_flags (obfd, osec) & SEC_CODE)
    p_flags |= PF_X;	/* Segment is executable.  */

  bfd_record_phdr (obfd, p_type, 1, p_flags, 0, 0, 0, 0, 1, &osec);
}

/* ubicom32v3_core_dump */
void
ubicom32v3_core_dump (char *args, int from_tty)
{
  extern int chipID;
  char default_name[]="Ubicom.core";
  int default_sdram_size = 2*1024*1024;
  int sdsize, ret;
  //struct ubicom32v3_core_header coreHeader;
  bfd *obfd;
  asection *note_sec = NULL;
  asection *hrt_sec = NULL;
  asection *ocm_sec = NULL;
  asection *ddr_sec = NULL;
  asection *mmu_sec = NULL;
  asection *flash_sec = NULL;
  flagword flags = SEC_ALLOC | SEC_HAS_CONTENTS | SEC_READONLY;

  char *filename;
  CORE_ADDR begin, end;
  unsigned int mem_size;
  FILE *fp;
  int i;
  char *buffer;
  unsigned int progress, total_size;
  int save_ubicom32v3_current_thread= ubicom32v3_current_thread;
  int save_ubicom32v3currentThread= ubicom32v3currentThread;
  CORE_ADDR begin_sdram, end_sdram;
  unsigned int sdram_limit_from_cpu;

  size_t buffer_size = 1024 * 1024;
  struct cleanup *old_chain;
  const struct bfd_arch_info *bfdarch = gdbarch_bfd_arch_info(current_gdbarch);
  char *note_data = NULL;
  int note_size = 0;
  int offset;

  /*
   * Turn off the 2 wire interface if possible.
   */
  ret = turnOff2wire();

  /*
   * Dump the cache before you do anything.
   */
  printf_unfiltered("Dumping Caches\n");
  ubicom32v3_cache_dump();

  filename = default_name;

  /* Open the output file.  */
  obfd = bfd_openw (filename, gnutarget);
  if (!obfd)
    error (_("Failed to open '%s' for output."), filename);

  /* Need a cleanup that will close the file (FIXME: delete it?).  */
  old_chain = make_cleanup_bfd_close (obfd);

  bfd_set_format (obfd, bfd_core);
  bfd_set_arch_mach (obfd, bfdarch->arch, bfdarch->mach);

  elf_elfheader(obfd)->e_flags = elf_elfheader(exec_bfd)->e_flags;

  /* We managed to open the file */
  /* Update the register info for all the threads */
  for(i=0; i< current_cpu->num_threads; i++)
    {
      ubicom32v3_set_thread(i+1);
      (void) current_cpu->fetch_registers_fn(current_cpu, i);
    }

  ubicom32v3_set_thread(save_ubicom32v3_current_thread);

  /* malloc a buffer that will hold per thread register + global regs + pid */
  buffer_size = (1 + current_cpu->num_per_thread_regs + current_cpu->num_global_regs) * 4;
  buffer = malloc(buffer_size);

  /* Create a note section consisting of all the thread registers */
  for(i=0; i< current_cpu->num_threads; i++)
    {
      /* The first 4 bytes is for the pid. */
      char *ptr = buffer + 4;
      current_cpu->create_register_note_data_fn(current_cpu, i, ptr);

      /* Load the thread number into the space for PID. */
      *(int *)buffer = (i +1);

      /* Write the note into the note buffer. */
      note_data = elfcore_write_note(obfd, note_data, &note_size, "CORE", NT_PRSTATUS, buffer, buffer_size);
    }

  /* Don't bother with a psinfo at this time. */

  /* Create the note section.  */
  if (note_data != NULL && note_size != 0)
    {
      note_sec = bfd_make_section_anyway_with_flags (obfd, "note0",
						     SEC_HAS_CONTENTS
						     | SEC_READONLY
						     | SEC_ALLOC);
      if (note_sec == NULL)
	error (_("Failed to create 'note' section for corefile: %s"),
	       bfd_errmsg (bfd_get_error ()));

      bfd_set_section_vma (obfd, note_sec, 0);
      bfd_set_section_alignment (obfd, note_sec, 0);
      bfd_set_section_size (obfd, note_sec, note_size);
    }

  /*
   * Create the various sections.
   */
  hrt_sec = bfd_make_section_anyway_with_flags (obfd, "load", flags);
  if (hrt_sec == NULL)
    {
      warning (_("Couldn't make core segment: %s"),
	       bfd_errmsg (bfd_get_error ()));
      return ;
    }
  
  bfd_set_section_size (obfd, hrt_sec, (current_cpu->hrt_end - current_cpu->hrt_start));
  bfd_set_section_vma (obfd, hrt_sec, current_cpu->hrt_start);
  bfd_section_lma (obfd, hrt_sec) = 0; /* ??? bfd_set_section_lma?  */

  ocm_sec = bfd_make_section_anyway_with_flags (obfd, "load", flags);
  if (ocm_sec == NULL)
    {
      warning (_("Couldn't make core segment: %s"),
	       bfd_errmsg (bfd_get_error ()));
      return ;
    }
  bfd_set_section_size (obfd, ocm_sec, (current_cpu->ocm_end - current_cpu->ocm_start));
  bfd_set_section_vma (obfd, ocm_sec, current_cpu->ocm_start);
  bfd_section_lma (obfd, ocm_sec) = 0; /* ??? bfd_set_section_lma?  */

  ddr_sec = bfd_make_section_anyway_with_flags (obfd, "load", flags);
  if (ddr_sec == NULL)
    {
      warning (_("Couldn't make core segment: %s"),
	       bfd_errmsg (bfd_get_error ()));
      return ;
    }

  mmu_sec = bfd_make_section_anyway_with_flags (obfd, "load", flags);
  if (mmu_sec == NULL)
    {
      warning (_("Couldn't make mmu segment: %s"),
	       bfd_errmsg (bfd_get_error ()));
      return ;
    }

  /* First see if there are explicit coredump limits */
  begin_sdram = ubicom32v3_lookup_symbol("__sdram_coredump_begin");
  end_sdram = ubicom32v3_lookup_symbol("__sdram_coredump_limit");
  sdram_limit_from_cpu = current_cpu->get_sdram_limit_fn();

  if (!begin_sdram || !end_sdram) {

	  begin_sdram = ubicom32v3_lookup_symbol("__sdram_begin");
	  end_sdram = ubicom32v3_lookup_symbol("__sdram_limit");
	  if(begin_sdram == 0)
		  printf("begin_sdram not found\n");

	  if(end_sdram == 0)
		  printf("end_sdram not found\n");
  }

  if (sdram_limit_from_cpu > end_sdram)
    end_sdram = sdram_limit_from_cpu;

  total_size = mem_size = (end_sdram - begin_sdram);
  begin = begin_sdram;

  bfd_set_section_size (obfd, ddr_sec, total_size);
  bfd_set_section_vma (obfd, ddr_sec, begin);
  bfd_section_lma (obfd, ddr_sec) = 0; /* ??? bfd_set_section_lma?  */

  if (chipID >= 0x40000)
    {
      bfd_set_section_size (obfd, mmu_sec, UBICOM32V5_MMU_SIZE);
      bfd_set_section_vma (obfd, mmu_sec, UBICOM32V5_MMU);
      bfd_section_lma (obfd, mmu_sec) = 0; 
    }
  
  flash_sec = bfd_make_section_anyway_with_flags (obfd, "load", flags);
  if (flash_sec == NULL)
    {
      warning (_("Couldn't make core segment: %s"),
	       bfd_errmsg (bfd_get_error ()));
      return ;
    }

  begin = current_cpu->flash_start;
  end = ubicom32v3_lookup_symbol("__filemedia_end_addr");
  total_size = mem_size = end - begin;

  bfd_set_section_size (obfd, flash_sec, total_size);
  bfd_set_section_vma (obfd, flash_sec, begin);
  bfd_section_lma (obfd, flash_sec) = 0; /* ??? bfd_set_section_lma?  */

  /* Record phdrs for section-to-segment mapping.  */
  bfd_map_over_sections (obfd, make_output_phdrs, NULL);

  /* malloc a 1 Meg buffer to move the data through */
  buffer_size = 1024 * 1024;
  buffer = malloc(buffer_size);

  if(buffer == NULL)
    {
      printf_unfiltered("ubicom32v3_core_dump: Failed to malloc transfer buffer\n");
      return;
    }

  /* write out the register data */
  if (note_data != NULL && note_size != 0)
    {
      if (!bfd_set_section_contents (obfd, note_sec, note_data, 0, note_size))
	warning (_("writing note section (%s)"), bfd_errmsg (bfd_get_error ()));
    }

  /* set up the fast downloader */
  ret = ubicom32v3isp_download_fast_transfer_code(current_cpu->ocm_start);

  /* read and write the hrt block */
  (void) ubicom32v3_read_bytes(current_cpu->hrt_start, buffer, (current_cpu->hrt_end - current_cpu->hrt_start));
  if (!bfd_set_section_contents (obfd, hrt_sec, buffer, 0, (current_cpu->hrt_end - current_cpu->hrt_start)))
    warning (_("writing note section (%s)"), bfd_errmsg (bfd_get_error ()));

  begin = current_cpu->ocm_start;
  end = current_cpu->ocm_end;

  /* write out the data memory size */
  mem_size = end - begin;

  printf_unfiltered("Dumping %dk OCM 0x%x-0x%x\n", mem_size / 1024, (int) begin, (int) end);
  /* now read and write out the ocm memory */
  offset = 0;
  while(mem_size)
    {
      int length = buffer_size;

      if(length > mem_size)
	length = mem_size;

      /* read in the data */
      ret = ubicom32v3UltraFastReadMemoryRaw(begin, length / 4, (int *)buffer);

      /* write out the data. */
      if (!bfd_set_section_contents (obfd, ocm_sec, buffer, offset, length))
	warning (_("writing note section (%s)"), bfd_errmsg (bfd_get_error ()));

      begin += length;
      mem_size -= length;
      offset += length;
    }

  /* First see if there are explicit coredump limits */
  begin_sdram = ubicom32v3_lookup_symbol("__sdram_coredump_begin");
  end_sdram = ubicom32v3_lookup_symbol("__sdram_coredump_limit");
  if (!begin_sdram || !end_sdram) {

	  begin_sdram = ubicom32v3_lookup_symbol("__sdram_begin");
	  end_sdram = ubicom32v3_lookup_symbol("__sdram_limit");
	  if(begin_sdram == 0)
		  printf("begin_sdram not found\n");

	  if(end_sdram == 0)
		  printf("end_sdram not found\n");
  }

  if (sdram_limit_from_cpu > end_sdram)
    end_sdram = sdram_limit_from_cpu;

  total_size = mem_size = (end_sdram - begin_sdram);
  begin = begin_sdram;

  printf("Dumping %dk DDR 0x%x-0x%x. This takes time.\n", mem_size / 1024, (int)begin_sdram, (int)end_sdram);
  /* now read and write out the DDR memory */
  progress = 0;
  offset = 0;
  ubicom32v3_show_load_progress_default(NULL, 0, 0, progress, total_size);

  while(mem_size)
    {
      int length = buffer_size;

      if(length > mem_size)
	length = mem_size;

      /* read in the data */
      ret = ubicom32v3UltraFastReadMemoryRaw(begin, length/4, (int *)buffer);

      /* write out the data. */
      if (!bfd_set_section_contents (obfd, ddr_sec, buffer, offset, length))
	warning (_("writing note section (%s)"), bfd_errmsg (bfd_get_error ()));

      begin += length;
      mem_size -= length;
      progress += length;
      offset += length;
      ubicom32v3_show_load_progress_default(NULL, 0, 0, progress, total_size);
    }

  /* read and write out the dynamic_non_volatile_section  */
  begin = current_cpu->flash_start;
  end = ubicom32v3_lookup_symbol("__filemedia_end_addr");
  total_size = mem_size = end - begin;

  printf_unfiltered("Dumping %dk FLASH 0x%x-0x%x\n", mem_size / 1024, (int)begin,(int)end);
  progress = 0;
  ubicom32v3_show_load_progress_default(NULL, 0, 0, progress, total_size);

  /* now read and write out the data memory */
  offset = 0;
  while(mem_size)
    {
      int length = buffer_size;

      if(length > mem_size)
	length = mem_size;

      /* read in the data */
      ret = ubicom32v3UltraFastReadMemoryRaw(begin, length/4, (int *)buffer);

      /* write out the data. */
      if (!bfd_set_section_contents (obfd, flash_sec, buffer, offset, length))
	warning (_("writing note section (%s)"), bfd_errmsg (bfd_get_error ()));

      offset += length;
      begin += length;
      mem_size -= length;
      progress += length;
      ubicom32v3_show_load_progress_default(NULL, 0, 0, progress, total_size);
    }
  
  if (chipID >= 0x40000)
    {
      /* Write MMU data.  */
      printf_unfiltered("Dumping MMU.\n");
      ret = ubicom32v3UltraFastReadMemoryRaw(UBICOM32V5_MMU, UBICOM32V5_MMU_SIZE/4, (int *)buffer);
      if (!bfd_set_section_contents (obfd, mmu_sec, buffer, 0, UBICOM32V5_MMU_SIZE))
        warning (_("writing mmu section (%s)"), bfd_errmsg (bfd_get_error ()));
    }

  do_cleanups (old_chain);

  /* Turn on turbo 2 wire if possible. */
  ret = turnOn2wire();

  return;
}

/* ubicom32v3_reset.  */
void
ubicom32v3_reset (char *args, int from_tty)
{
  int ret;
  bpReason_t reason;
  int i;

  dcache_status = 0;

  if (remote_debug)
    printf_filtered ("ubicom32v3_reset\n");

  reinit_frame_cache();
  registers_changed();
  ret = ubicom32v3isp_reset();

  /* restart the Cpu to get it to the ok_to_set_breakpoints label */
  ret= ubicom32v3restartProcessor(1);
  remote_ubicom32v3_state = UBICOM32V3_RUNNING;

  for (i=0; i< 500; i++)
    {
      ret = ubicom32v3waitForBP(&reason);

      if(ret< 0)
	{
	  printf_unfiltered ("error: unable to read status\n");
	  break;
	}

      if(ret == 0)
	{
	  /* wait for 10 ms before trying again */
	  usleep(10000);
	  continue;
	}
      else
	{
	  break;
	}
    }

  if((i == 500) || (ret < 0))
    {
      /* The processor is still running, we need to stop it. */
      ret = ubicom32v3stopProcessor();
    }

  ubicom32v3_set_thread(1);
  inferior_ptid = pid_to_ptid (1);
  ubicom32v3_fetch_register(get_current_regcache(), 0);
  select_frame(get_current_frame());
  normal_stop();
  ubicom32v3_read_dcache_status();
}

/* ubicom32v3_kill.  */
static void
ubicom32v3_kill (void)
{
  if (remote_debug)
      printf_filtered ("ubicom32v3_kill\n");

  inferior_ptid = null_ptid;

  /* Detach from target.  */
  ubicom32v3_detach (NULL, 0);
}

/* ubicom32v3_add_module_file.  */
static void
ubicom32v3_erase_flash (char *args, int from_tty)
{
  bfd *loadfile_bfd;
  asection *s, *downloadersection;
  char *arg;
  char **argv;
  struct cleanup *my_cleanups = make_cleanup (null_cleanup, NULL);
  char *file, *data;
  CORE_ADDR start_addr = 0;
  CORE_ADDR end_addr = 0;
  CORE_ADDR length = 0;
  int total_length = 0;
  int progress = 0;
  int has_downloader = 0;
  unsigned int reply;
  CORE_ADDR ocm_begin;
  int offs, argcnt, ret;
  extern int restart_watchdog;
  extern int leakage_enable;

  if (args == NULL)
    error (_("Usage: erase-flash start_addr length\n"));

  dont_repeat ();

  argv = buildargv (args);
  make_cleanup_freeargv (argv);

  if (argv == NULL)
    nomem (0);

  /*
   * Find how many arguments were passed to us.
   */
  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
    }

  if (argcnt != 2)
    error ("Usage: erase-flash start_address length\n");

  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
      /* Process the argument. */
      if (argcnt == 0)
	{
	  /* The first argument is the start address. */
	  start_addr = parse_and_eval_address (arg);
	}
      else if (argcnt == 1)
	{
	  /* The second argument is length. */
	  length = parse_and_eval_address (arg);
	  end_addr = start_addr + length;
	}
    }

  /*
   * range check the start and end addresses.
   */
  if ((start_addr < current_cpu->flash_start) || (start_addr > current_cpu->flash_end))
    error("Start address 0x%lx is out of range\n", start_addr);

  if ((end_addr < current_cpu->flash_end) || (end_addr > current_cpu->flash_end))
    error("Start address 0x%lx is out of range\n", end_addr);

  file = get_exec_file(1);

  if (remote_debug)
    printf_filtered ("ubicom32v3_load: %s (tty=%d)\n", file, from_tty);

  /* Open the file.  */
  loadfile_bfd = bfd_openr (file, gnutarget);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  /*
   * Scan through the sections looking for the .downloader section. We better find it
   * because without it we are not going to be able to download any code to the board.
   */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if(strcmp(s->name, ".downloader"))
	{
	  continue;
	}

      has_downloader = 1;
      break;
    }

  if(has_downloader == 0)
    {
      printf_unfiltered("%s has no \".downloader\" section. Unable to erase flash, no flash drivers.\n", file);
      return;
    }

  downloadersection = s;
  length = bfd_get_section_size (s);

  /* Get the downloader section data. */
  data = malloc(length+64);
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for .downloader section data.\n");
      return;
    }

  offs = 0;

  /* read in the .downloader section data from the the file. */
  bfd_get_section_contents (loadfile_bfd, s, data, offs, length);

  /* We are clobbering the system. Wipe out the restart watchdog flag. */
  restart_watchdog = 0;
  leakage_enable = 0;

  /* xxxxxxxxxxxxxxxxxxxxxxxxxx call the backed init */
  ocm_begin = ubicom32v3_lookup_symbol("__ocm_begin");
  if (ocm_begin == 0)
    {
      printf_unfiltered("No __ocm_begin symbol. Unable to initialize Boot Kernel\n");
      free(data);
      ubicom32v3_reset(0,0);
      return;
    }

  /*
   * Turn off the 2 wire interface if possible.
   */
  ret = turnOff2wire();

  ret = ubicom32v3isp_initBackendToBootKernel(length, (unsigned int*)data, ocm_begin);
  if(ret)
    {
      /* Turn on turbo 2 wire if possible. */
      ret = turnOn2wire();
      printf_unfiltered("Unable to initialize Boot Kernel\n");
      free(data);
      ubicom32v3_reset(0,0);
      return;
    }

  free(data);

  end_addr -= start_addr;
  printf("Begin Erase.\n 0x%lx length 0x%lx\n", start_addr, end_addr);

  ret = eraseVerifyFlash(start_addr, end_addr, ubicom32_erase_progress_callback, NULL);

  /* Turn on turbo 2 wire if possible. */
  ret = turnOn2wire();
  ubicom32v3_reset(0,0);

  return;
}

static bfd *usercore_obfd = NULL;
static char *usercore_note_data = NULL;
static int usercore_note_size = 0;
static char usercore_filename[100];
static struct cleanup *usercore_old_chain;
static asection *usercore_note_sec = NULL;

struct mem_info {
  CORE_ADDR start;
  CORE_ADDR length;
  asection *mem_sec;
};

static struct mem_info *m_info = NULL;
static int current_mem_info_index = 0;
static int max_mem_info = 0;

static void
create_user_coredump (char *args, int from_tty)
{
  char *arg;
  char **argv;
  int argcnt, i;
  CORE_ADDR arg_start_addr = 0;
  CORE_ADDR arg_end_addr = 0;
  CORE_ADDR core_name = 0;
  CORE_ADDR length = 0;
  const struct bfd_arch_info *bfdarch = gdbarch_bfd_arch_info(current_gdbarch);

  char filename[100];
  char buffer[80];		/* Program name */
  char buffer1[80];		/* Program args */
  char psinfo[124];		/* PS info structure. */
  int current_eflags = elf_elfheader(exec_bfd)->e_flags;

  /*
   * We want the core to be fdpic.
   */
  current_eflags |= 0x40000000;

  printf("Current e-flags = 0x%08x\n", current_eflags);

  if (args == NULL)
    error (_("Usage: create-user-coredump Name srart_args end_args\n"));

  dont_repeat ();

  argv = buildargv (args);
  make_cleanup_freeargv (argv);

  if (argv == NULL)
    nomem (0);

  /*
   * Find how many arguments were passed to us.
   */
  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
    }

  if (argcnt != 3)
    error ("Usage: create-user-coredump Name srart_args end_args\n");

  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
      /* Process the argument. */
      if (argcnt == 0)
	{
	  /* The first argument is the name of the file. This is 16 byte array in the core. */
	  core_name = parse_and_eval_address (arg);
	  
	  /* Read 16 bytes from the core info buffer. */
	  if(target_read_memory(core_name, (gdb_byte *)buffer, 16)) {
	    error("Target read bombed\n");
	  }

	  sprintf(usercore_filename, "%s%d", buffer, getpid());
	}
      else if (argcnt == 1)
	{
	  /* The second argument is start of the argument list. */
	  arg_start_addr = parse_and_eval_address (arg);
	}
      else if (argcnt == 2)
	{
	  /* The third argument is end of the argument list. */
	  arg_end_addr = parse_and_eval_address (arg);
	  length = arg_end_addr - arg_start_addr;

	  if (length > 80) {
	    length = 80;
	  }

	  /*
	   * Read the args into the buffer.
	   */
	  if(target_read_memory(arg_start_addr, (gdb_byte *)buffer1, length)) {
	    error("Target read bombed\n");
	  }

	  for (i = 0; i < length; i++)
	    if (buffer1[i] == 0)
	      buffer1[i] = ' ';
	  if (length == 80) {
	    buffer1[79] = 0;
	  } else {
	    buffer1[length] = 0;
	  }
	}
    }
  /* Open the output file.  */
  usercore_obfd = bfd_openw (usercore_filename, gnutarget);
  if (!usercore_obfd)
    error (_("Failed to open '%s' for output."), usercore_filename);

  /* Need a cleanup that will close the file (FIXME: delete it?).  */
  //usercore_old_chain = make_cleanup_bfd_close (usercore_obfd);

  bfd_set_format (usercore_obfd, bfd_core);
  bfd_set_arch_mach (usercore_obfd, bfdarch->arch, bfdarch->mach);

  elf_elfheader(usercore_obfd)->e_flags = current_eflags;

  /*
   * Create the psinfo buffer. Psinfo is 128 bytes long. The f_name is at offset 28
   * args is at offset 44.
   */
  memset(psinfo, 0, 124);
  memcpy(&psinfo[28], buffer, 16);
  memcpy(&psinfo[44], buffer1, 80);

  /* Write the note into the note buffer. */
  usercore_note_data = elfcore_write_note(usercore_obfd, usercore_note_data, &usercore_note_size, "CORE", NT_PRPSINFO, psinfo, 124);

  printf("Creating core file %s for %s args %s\n", usercore_filename, buffer, buffer1);
}

/*
 * Functiont to close and write out an open core file.
 *    Arguments: None
 */
static void
close_user_coredump (char *args, int from_tty)
{
  int i;
  char *buffer;

  /* Record phdrs for section-to-segment mapping.  */
  bfd_map_over_sections (usercore_obfd, make_output_phdrs, NULL);

  if (usercore_note_data != NULL && usercore_note_size != 0) {
    if (usercore_note_sec == NULL) {
      error("Note section does not exist.  Perhaps no memory was dumped?\n");
    }
  
    /*
     * Before we do anything get the note section established and put away 
     * into to core and zero its pointers.
     */
    if (!bfd_set_section_contents (usercore_obfd, usercore_note_sec, 
				   usercore_note_data, 0, usercore_note_size)) {
      warning (_("writing note section (%s)"), bfd_errmsg (bfd_get_error ()));
    }
    
    usercore_note_data = NULL;
    usercore_note_size = 0;
  }

  /* malloc a 1 Meg buffer to move the data through */
  buffer = malloc(0x100000);

  for (i=0; i< current_mem_info_index; i++) {
    CORE_ADDR start_ptr = m_info[i].start;
    int length = m_info[i].length;
    int offset = 0;
    int transfer_length = 0x100000;
    while (length) {
      if (transfer_length > length) {
	transfer_length = length;
      }

      /*
       * Read in the data.
       */
      if(target_read_memory(start_ptr, (gdb_byte *)buffer, transfer_length)) {
	error("Target read bombed\n");
      }

      /* write out the data. */
      if (!bfd_set_section_contents (usercore_obfd, m_info[i].mem_sec, buffer, offset, transfer_length))
	warning (_("writing memory section (%s)"), bfd_errmsg (bfd_get_error ()));
      
      offset += transfer_length;
      start_ptr += transfer_length;
      length -= transfer_length;
    }
  }
  free(buffer);

  usercore_old_chain = make_cleanup_bfd_close (usercore_obfd); 
  usercore_obfd = NULL;
  do_cleanups (usercore_old_chain);
}

/*
 * Functiont to add memory information to core file.
 *    Arguments: start_ptr, length
 */
static void
user_coredump_add_mem (char *args, int from_tty)
{
  char *arg;
  char **argv;
  int argcnt, i;
  CORE_ADDR start_ptr = 0;
  CORE_ADDR length = 0;
  struct mem_info *minfo;

  asection *mem_sec = NULL;
  char *buffer;
  int buffer_size = 0x100000;
  int transfer_length = 0x100000;
  int offset = 0;
  flagword flags = SEC_ALLOC | SEC_HAS_CONTENTS | SEC_READONLY;

  if (args == NULL)
    error (_("Usage: user-coredump-add-mem start_ptr length\n"));

  dont_repeat ();

  argv = buildargv (args);
  make_cleanup_freeargv (argv);

  if (argv == NULL)
    nomem (0);

  /*
   * Find how many arguments were passed to us.
   */
  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
    }

  if (argcnt != 2)
    error (_("Usage: user-coredump-add-mem start_ptr length\n"));

  start_ptr = parse_and_eval_address (argv[0]);
  length = parse_and_eval_address (argv[1]);

  if (m_info == NULL) {
    /* Create the note section.  */
    if (usercore_note_data != NULL && usercore_note_size != 0)
      {
	usercore_note_sec = bfd_make_section_anyway_with_flags (usercore_obfd, "note0",
								SEC_HAS_CONTENTS
								| SEC_READONLY
								| SEC_ALLOC);
	if (usercore_note_sec == NULL)
	  error (_("Failed to create 'note' section for corefile: %s"),
		 bfd_errmsg (bfd_get_error ()));
	
	bfd_set_section_vma (usercore_obfd, usercore_note_sec, 0);
	bfd_set_section_alignment (usercore_obfd, usercore_note_sec, 0);
	bfd_set_section_size (usercore_obfd, usercore_note_sec, usercore_note_size);
      }

    /*
     * Allocate 100 m_info entries, set the max and set the index to zero.
     */
    m_info = malloc(sizeof(struct mem_info)*200);
    max_mem_info = 200;
    current_mem_info_index = 0;
  } else if (current_mem_info_index == max_mem_info) {
    /*
     * We have used up all allocations. Need to realloc another 100 entries.
     */
    max_mem_info += 100;
    m_info = realloc(m_info, sizeof(struct mem_info) * max_mem_info);
  }

  /*
   * Grab an entry.
   */
  minfo = &m_info[current_mem_info_index++];
  minfo->start = start_ptr;
  minfo->length = length;
  mem_sec = bfd_make_section_anyway_with_flags (usercore_obfd, "load", flags);
  if (mem_sec == NULL)
    {
      warning (_("Couldn't make core segment: %s"),
	       bfd_errmsg (bfd_get_error ()));
      return ;
    }
  
  bfd_set_section_size (usercore_obfd, mem_sec, length);
  bfd_set_section_vma (usercore_obfd, mem_sec, start_ptr);
  bfd_section_lma (usercore_obfd, mem_sec) = 0; /* ??? bfd_set_section_lma?  */

  minfo->mem_sec = mem_sec;
}
 
/*
 * Functiont to add virtual memory information to core file.
 *    Arguments: virtual_start_address real_start_address, length
 */
static void
user_coredump_add_virtual_mem (char *args, int from_tty)
{
  char *arg;
  char **argv;
  int argcnt, i;
  CORE_ADDR virtual_start_ptr = 0;
  CORE_ADDR start_ptr = 0;
  CORE_ADDR length = 0;
  struct mem_info *minfo;

  asection *mem_sec = NULL;
  char *buffer;
  int buffer_size = 0x100000;
  int transfer_length = 0x100000;
  int offset = 0;
  flagword flags = SEC_ALLOC | SEC_HAS_CONTENTS | SEC_READONLY;

  if (args == NULL)
    error (_("Usage: user-coredump-add-mem virtual_start_address real_start_address length\n"));

  dont_repeat ();

  argv = buildargv (args);
  make_cleanup_freeargv (argv);

  if (argv == NULL)
    nomem (0);

  /*
   * Find how many arguments were passed to us.
   */
  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
    }

  if (argcnt != 3)
    error (_("Usage: user-coredump-add-mem virtual_start_address real_start_address length\n"));

  virtual_start_ptr = parse_and_eval_address (argv[0]);
  start_ptr = parse_and_eval_address (argv[1]);
  length = parse_and_eval_address (argv[2]);

  if (m_info == NULL) {
    /* Create the note section.  */
    if (usercore_note_data != NULL && usercore_note_size != 0)
      {
	usercore_note_sec = bfd_make_section_anyway_with_flags (usercore_obfd, "note0",
								SEC_HAS_CONTENTS
								| SEC_READONLY
								| SEC_ALLOC);
	if (usercore_note_sec == NULL)
	  error (_("Failed to create 'note' section for corefile: %s"),
		 bfd_errmsg (bfd_get_error ()));
	
	bfd_set_section_vma (usercore_obfd, usercore_note_sec, 0);
	bfd_set_section_alignment (usercore_obfd, usercore_note_sec, 0);
	bfd_set_section_size (usercore_obfd, usercore_note_sec, usercore_note_size);
      }

    /*
     * Allocate 100 m_info entries, set the max and set the index to zero.
     */
    m_info = malloc(sizeof(struct mem_info)*200);
    max_mem_info = 200;
    current_mem_info_index = 0;
  } else if (current_mem_info_index == max_mem_info) {
    /*
     * We have used up all allocations. Need to realloc another 100 entries.
     */
    max_mem_info += 100;
    m_info = realloc(m_info, sizeof(struct mem_info) * max_mem_info);
  }

  /*
   * Grab an entry.
   */
  minfo = &m_info[current_mem_info_index++];
  minfo->start = start_ptr;
  minfo->length = length;
  mem_sec = bfd_make_section_anyway_with_flags (usercore_obfd, "load", flags);
  if (mem_sec == NULL)
    {
      warning (_("Couldn't make core segment: %s"),
	       bfd_errmsg (bfd_get_error ()));
      return ;
    }
  
  bfd_set_section_size (usercore_obfd, mem_sec, length);
  bfd_set_section_vma (usercore_obfd, mem_sec, virtual_start_ptr);
  bfd_section_lma (usercore_obfd, mem_sec) = 0; /* ??? bfd_set_section_lma?  */

  minfo->mem_sec = mem_sec;
}

void
create_ptregs_from_live_registers (gdb_byte *destination)
{
  int current_eflags = (elf_elfheader(exec_bfd)->e_flags) &0xffff;

  if (core_bfd == 0) {
    if (current_eflags == 2) {
      ubicom32v4Regs_t *mainRegs = (ubicom32v4Regs_t *)current_cpu->registers;
      int *iptr;

      /* Copy d0 - mac_rc16 */
      memcpy(destination, &mainRegs->tRegs[ubicom32v3currentThread].dr[0], (16 + 8 + 3) * 4);
      destination += (16 + 8 + 3) * 4;

      /* Copy acc1_hi and acc1_lo */
      memcpy(destination, &mainRegs->tRegs[ubicom32v3currentThread].acc1_hi, 2 * 4);
      destination += 2 * 4;

      iptr = (int *)destination;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].source3;
      iptr++;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].csr;

      /* skip over all the way to trap_cause */
      iptr += 3;

      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].trap_cause;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].threadPc;
      iptr++;
      iptr++;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].previous_pc;
    } else {
      ubicom32v5Regs_t *mainRegs = (ubicom32v5Regs_t *)current_cpu->registers;
      int *iptr;

      /* Copy d0 - mac_rc16 */
      memcpy(destination, &mainRegs->tRegs[ubicom32v3currentThread].dr[0], (16 + 8 + 3) * 4);
      destination += (16 + 8 + 3) * 4;

      /* Copy acc1_hi and acc1_lo */
      memcpy(destination, &mainRegs->tRegs[ubicom32v3currentThread].acc1_hi, 2 * 4);
      destination += 2 * 4;

      iptr = (int *)destination;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].source3;
      iptr++;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].csr;

      /* skip over all the way to trap_cause */
      iptr += 3;

      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].trap_cause;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].threadPc;
      iptr++;
      iptr++;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].previous_pc;

      /* skip over all the way to ucsr */
      iptr += 3;
      *iptr++ = mainRegs->tRegs[ubicom32v3currentThread].ucsr;
    }
  } else {
    /* We have to read stuff out of the regcache. Everything is based on GDB register numbers.*/
    extern void create_ptregs_from_regcache (gdb_byte *destination, struct regcache *regcache);

    create_ptregs_from_regcache (destination, get_current_regcache());
  }
}

/*
 * Functiont to add register information to core file.
 *    Arguments: pid_ptr, pt_regs_ptr, loadmap_ptr, sizeof_pt_regs.
 */
static void
user_coredump_add_reg (char *args, int from_tty)
{
  char *arg;
  char **argv;
  int argcnt, i;
  CORE_ADDR pid_ptr = 0;
  CORE_ADDR pt_regs_ptr = 0;
  CORE_ADDR loadmap_ptr = 0;
  CORE_ADDR pt_reg_size = 0;
  int pr_status_size;

  char *pr_status = NULL;

  if (args == NULL)
    error (_("Usage: user-coredump-add-reg pid_ptr pt_regs_ptr loadmap_ptr sizeof pt_regs\n"));

  dont_repeat ();

  argv = buildargv (args);
  make_cleanup_freeargv (argv);

  if (argv == NULL)
    nomem (0);

  /*
   * Find how many arguments were passed to us.
   */
  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
    }

  if (argcnt != 4)
    {
      error (_("Usage: user-coredump-add-reg pid_ptr pt_regs_ptr loadmap_ptr sizeof pt_regs\n"));
    }

  /*
   * The register info is a 252 byte structure. cursig is at offset 12. We don't care. PID is at offset 24.
   * pt_regs is at offset 72. Loadmap is right after pt_regs.
   */

  pid_ptr = parse_and_eval_address (argv[0]);
  pt_regs_ptr = parse_and_eval_address (argv[1]);
  loadmap_ptr = parse_and_eval_address (argv[2]);
  pt_reg_size = parse_and_eval_address (argv[3]);

  pr_status_size = 72 + 12 + pt_reg_size;
  pr_status = malloc(pr_status_size);
  memset(pr_status, 0, pr_status_size );

  /*
   * Copy pid information.
   */
  if(target_read_memory(pid_ptr, (gdb_byte *)&pr_status[24], 4)) {
    free(pr_status);
    error("Target read bombed\n");
  }

  if (pt_regs_ptr) {
      if(target_read_memory(pt_regs_ptr, (gdb_byte *)&pr_status[72], pt_reg_size)) {
	free(pr_status);
	error("Target read bombed\n");
      }
  } else {
    /* user live registers. */
    create_ptregs_from_live_registers((gdb_byte *)&pr_status[72]);
  }

  if(target_read_memory(loadmap_ptr, (gdb_byte *)&pr_status[72 + pt_reg_size], 8)) {
    free(pr_status);
    error("Target read bombed\n");
  }

  /*
   * Add it to the notes section.
   */
  usercore_note_data = elfcore_write_note(usercore_obfd, usercore_note_data, &usercore_note_size, "CORE", NT_PRSTATUS, pr_status, pr_status_size);

  printf("Added data from pid 0x%08lx pt_regs 0x%08lx lmap 0x%08lx reg size %d\n", pid_ptr, pt_regs_ptr, loadmap_ptr, (int)pt_reg_size);
  free(pr_status);
}

static void
bail_out(char *ptr) {
  error ("Bad Input %s. Abandon rest of input\n", ptr);
}

/*
 * Function to change registers.
 * The args will look like the following strings.
 * pc: c026f328, previous_pc: c026f324
 * D00: 00000001, D01: 973956ff, D02: 00000001, D03: c063e4b0,
 * D04: 00000026, D05: 02d834dc, D06: d60cc700, D07: 000079b9,
 * D08: 00000001, D09: 00000200, D10: c063e380, D11: c07fad00,
 * D12: 0000003c, D13: 00000000, D14: 00000200, D15: 00000000,
 * A00: 973956ff, A01: cff40c60, A02: 00000000, A03: c0473ea0,
 * A04: cff40c60, A05: c026f478, A06: c0473ea0, A07: c033fe1c,
 * acc0: 00001194-00000000, acc1: 00000000-00000000
 * mac_rc16: 00000000, source3: 00000000
 * inst_cnt: bec11f6c, csr: 00000000
 * ucsr: 000000c4
 */
static void
change_regs(char *args, int from_tty)
{
  char *comma_ptr;
  char *colon_ptr;
  char *field_begin_ptr;
  struct regcache *current_regcache = get_current_regcache();
  int elf_flags;

  elf_flags = elf_elfheader (exec_bfd)->e_flags & 0xffff;

  field_begin_ptr = args;

  while(*field_begin_ptr) {
    char *field = field_begin_ptr;
    char reg_name_start = *field_begin_ptr++;
    CORE_ADDR reg_value;
    CORE_ADDR reg_value2;
    int ret;
    int reg_num;

    /*
     * Each register name begins with an ascii character a-zA-Z. Eliminate
     * any junk before that.
     */
    if (reg_name_start < 'A')
      continue;
    else if (reg_name_start > 'Z' && reg_name_start < 'a')
      continue;
    else if (reg_name_start > 'z')
      continue;

    /*
     * We have an ascii character.
     */
    switch(reg_name_start) {
    case 'p':
      if (strncmp(field, "pc: ", strlen("pc: "))== 0) {

	field += strlen("pc: ");
	if (sscanf(field, "%x", &reg_value) != 1) {
	  bail_out(field - strlen("pc: "));
	}

	field += 8;
	field_begin_ptr = field;

	/*
	 * We got the PC. Load it into the regcache.
	 */
	swizzled_supply_register(current_regcache, gdbarch_pc_regnum(current_gdbarch), (const void *)&reg_value);
      } else if (strncmp(field, "previous_pc: ", strlen("previous_pc: ")) == 0) {
	field += strlen("previous_pc: ");
	if (sscanf(field, "%x", &reg_value) != 1) {
	  bail_out(field + strlen("previous_pc: "));
	}

	field += 8;
	field_begin_ptr = field;

	switch(elf_flags){
	case 1:
	case 2:
	  reg_num = 38;
	  break;
	case 4:
	  reg_num = 39;
	  break;
	}

	swizzled_supply_register(current_regcache, reg_num, (const void *)&reg_value);
      } else {
	bail_out(field);
      }
      break;
    case 'D':
      ret = sscanf(field, "D%2d: %8x", &reg_num, &reg_value);
      field += 13;
      field_begin_ptr = field;
      swizzled_supply_register(current_regcache, reg_num, (const void *)&reg_value);
      break;
    case 'A':
      ret = sscanf(field, "A%2d: %8x", &reg_num, &reg_value);
      field += 13;
      field_begin_ptr = field;
      swizzled_supply_register(current_regcache, reg_num + 16, (const void *)&reg_value);
      break;
    case 'a':
      if (strncmp(field, "acc0: ", strlen("acc0: ")) == 0) {
	ret = sscanf(field, "acc0: %8x-%8x", &reg_value, &reg_value2);
	if (ret != 2)
	  bail_out(field);

	swizzled_supply_register(current_regcache, 24, (const void *)&reg_value);
	swizzled_supply_register(current_regcache, 25, (const void *)&reg_value);

	field += 23;
	field_begin_ptr = field;
      } else if (strncmp(field, "acc1: ", strlen("acc0: ")) == 0) {
	ret = sscanf(field, "acc1: %8x-%8x", &reg_value, &reg_value2);
	if (ret != 2)
	  bail_out(field);

	switch(elf_flags){
	case 1:
	case 2:
	  reg_num = 36;
	  break;
	case 4:
	  reg_num = 37;
	  break;
	}

	swizzled_supply_register(current_regcache, reg_num, (const void *)&reg_value);
	swizzled_supply_register(current_regcache, reg_num + 1, (const void *)&reg_value);
	field += 23;
	field_begin_ptr = field;
	return;
      } else {
	bail_out(field);
      }
      break;
    case 'm':
      if (strncmp(field, "mac_rc16: ", strlen("mac_rc16: "))) {
	bail_out(field);
      }

      field += strlen("mac_rc16: ");
      if (sscanf(field, "%x", &reg_value) != 1) {
	bail_out(field + strlen("mac_rc16: "));
      }

      field += 8;
      field_begin_ptr = field;
      swizzled_supply_register(current_regcache, 26, (const void *)&reg_value);
      break;
    case 'c':
      if (strncmp(field, "csr: ", strlen("csr: "))) {
	bail_out(field);
      }

      field += strlen("csr: ");
      if (sscanf(field, "%x", &reg_value) != 1) {
	bail_out(field + strlen("csr: "));
      }

      field += 8;
      field_begin_ptr = field;
      swizzled_supply_register(current_regcache, 29, (const void *)&reg_value);
      break;
    case 'i':
      if (strncmp(field, "inst_cnt: ", strlen("inst_cnt: "))) {
	bail_out(field);
      }

      field += strlen("inst_cnt: ");
      if (sscanf(field, "%x", &reg_value) != 1) {
	bail_out(field + strlen("inst_cnt: "));
      }

      field += 8;
      field_begin_ptr = field;
      break;
    case 'u':
      if (strncmp(field, "ucsr: ", strlen("ucsr: "))) {
	bail_out(field);
      }

      field += strlen("ucsr: ");
      if (sscanf(field, "%x", &reg_value) != 1) {
	bail_out(field + strlen("ucsr: "));
      }

      field += 8;
      field_begin_ptr = field;
      swizzled_supply_register(current_regcache, 40, (const void *)&reg_value);
      break;
    case 's':
      if (strncmp(field, "source3: ", strlen("source3: "))) {
	bail_out(field);
      }

      field += strlen("source3: ");
      if (sscanf(field, "%x", &reg_value) != 1) {
	bail_out(field + strlen("source3: "));
      }

      field += 8;
      field_begin_ptr = field;
      swizzled_supply_register(current_regcache, 27, (const void *)&reg_value);
      break;
    default:
      bail_out(field);
      break;
    }
  }
}

/*
 * Function to change PC in current_regcache
 *    Arguments: new_pc
 */
static void
change_pc (char *args, int from_tty)
{
  char *arg;
  char **argv;
  int argcnt, i;
  CORE_ADDR pc_value = 0;

  if (args == NULL)
    error (_("Usage: Usage: ubcom-set-pc: pc_value\n"));

  dont_repeat ();

  argv = buildargv (args);
  make_cleanup_freeargv (argv);

  if (argv == NULL)
    nomem (0);

  /*
   * Find how many arguments were passed to us.
   */
  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
    }

  if (argcnt != 1)
    {
      error (_("Usage: Usage: ubcom-set-pc: pc_value\n"));
    }

  /*
   * Extract the pc_value provided by the user.
   */
  pc_value = parse_and_eval_address (argv[0]);

  swizzled_supply_register(get_current_regcache(), gdbarch_pc_regnum(current_gdbarch), (const void *)&pc_value);
}

/*
 * Function to change SP in current_regcache
 *    Arguments: new_sp
 */
static void
change_sp (char *args, int from_tty)
{
  char *arg;
  char **argv;
  int argcnt, i;
  CORE_ADDR sp_value = 0;

  if (args == NULL)
    error (_("Usage: Usage: ubcom-set-sp: sp_value\n"));

  dont_repeat ();

  argv = buildargv (args);
  make_cleanup_freeargv (argv);

  if (argv == NULL)
    nomem (0);

  /*
   * Find how many arguments were passed to us.
   */
  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
    }

  if (argcnt != 1)
    {
      error (_("Usage: Usage: ubcom-set-sp: sp_value\n"));
    }

  /*
   * Extract the sp_value provided by the user.
   */
  sp_value = parse_and_eval_address (argv[0]);

  swizzled_supply_register(get_current_regcache(), gdbarch_sp_regnum(current_gdbarch), (const void *)&sp_value);

  reinit_frame_cache();
  get_selected_frame(NULL);
}

/*
 * Function to change SP in current_regcache
 *    Arguments: new_sp
 */
static void
reset_frames (char *args, int from_tty)
{
  reinit_frame_cache();
  get_selected_frame(NULL);
}

/* ubicom32v3_add_module_file.  */
static void
ubicom32v3_add_module_file (char *args, int from_tty)
{
  bfd *loadfile_bfd;
  asection *s;
  char *arg;
  char **argv;
  struct cleanup *my_cleanups = make_cleanup (null_cleanup, NULL);
  char *filename = NULL;
  CORE_ADDR text_addr = 0;
  CORE_ADDR ocm_text_addr = 0;
  CORE_ADDR dyn_text_addr;
  CORE_ADDR dyn_ocm_text_addr;
  CORE_ADDR data_addr;
  CORE_ADDR dyn_data_addr;
  int num_text, num_ocm_text, num_data, section_index;
  int do_ocm_text = 0;
  int argcnt = 0;
  struct section_addr_info *section_addrs;
  //int flags = OBJF_USERLOADED;
  int flags = OBJF_READNOW;
  struct objfile *ofiles = object_files;
  struct partial_symtab *psymtabs;

  if (args == NULL)
    error (_("add-module-file takes a file name and an address"));

  dont_repeat ();

  argv = buildargv (args);
  make_cleanup_freeargv (argv);

  if (argv == NULL)
    nomem (0);

  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
      /* Process the argument. */
      if (argcnt == 0)
	{
	  /* The first argument is the file name. */
	  filename = tilde_expand (arg);
	  make_cleanup (xfree, filename);
	}
      else
	if (argcnt == 1)
	  {
	    /* The second argument is always the address of the text section. */
	       text_addr = parse_and_eval_address (arg);
	  }
	else if (argcnt == 2)
	  {

	    if (strncmp(arg, "(nil)", strlen("(nil)")) == 0)
	      continue;

	    /* The third argument is always the address of the ocm_text_section */
	    ocm_text_addr = parse_and_eval_address (arg);
	    do_ocm_text = 1;
	  }
	else
	  {
	    error("Too many parameters\n.Usage: add-module-name text_address <ocm_text_address>\n");
	    return;
	  }
    }

  if(do_ocm_text)
    printf_filtered("add-module-file \"%s\" text_address= 0x%lx ocm_text_address= 0x%lx\n", filename, (long)text_addr, (long)ocm_text_addr);
  else
    printf_filtered("add-module-file %s text_address= 0x%lx\n", filename, (long)text_addr);

  /* Open the file. */
  //loadfile_bfd = bfd_openr (filename, gnutarget);
  loadfile_bfd = symfile_bfd_open (filename);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", filename);

  /* Loop through all the sections and find the OCM sections and calculate how many text and ocmtext sections we have. */
  dyn_text_addr = text_addr;
  dyn_ocm_text_addr = ocm_text_addr;

  num_text = num_ocm_text = num_data = 0;

  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      /*
       * Linux ignores .modinfo and __versions sections.  GDB needs to
       * follow the same rules as Linux.
       */
      if (strcmp(s->name, ".modinfo") == 0 ||
	  strcmp(s->name, "__versions") == 0)
        {
	  // Ignore .modinfo and __versions sections.  Do nothing.
        }
      /*
       * OCM text sections are not adjacent to standard text sections
       * since Linux will load them into OCM.  Verify that the user has
       * specified an OCM start address if this module has OCM text
       * sections.
       */
      else if (strncmp(s->name, ".ocm_text", strlen(".ocm_text")) == 0)
	{
	  if (do_ocm_text == 0)
	    {
	      error("File has ocm_text sections but no ocm base address has been provided.");
	    }
#ifdef UBICOM32_DEBUG_MODULE_LOAD
	  printf("OCM section: %s, flags %x\n", s->name, s->flags);
#endif

	  num_ocm_text ++;
	  dyn_ocm_text_addr += bfd_get_section_size (s);
	}
      /*
       * Text sections are marked with the ALLOC flag and the CODE flag.
       */
      else if ((s->flags & (SEC_ALLOC|SEC_CODE)) == (SEC_ALLOC|SEC_CODE) &&
	       strncmp(s->name, ".init", strlen(".init")) != 0)
	{
#ifdef UBICOM32_DEBUG_MODULE_LOAD
		printf("Text section: %s, flags %x\n", s->name, s->flags);
#endif
	  num_text ++;
	  dyn_text_addr += bfd_get_section_size (s);
	}
      /*
       * Data sections are marked with the ALLOC flag and no CODE flag.
       */
      else if ((s->flags & (SEC_ALLOC|SEC_CODE)) == SEC_ALLOC)
	{
#ifdef UBICOM32_DEBUG_MODULE_LOAD
	  printf("Data section: %s, flags %x\n", s->name, s->flags);
#endif
	  num_data ++;
	}
    }

  dyn_data_addr = dyn_text_addr;
  dyn_text_addr = text_addr;
  dyn_ocm_text_addr = ocm_text_addr;

  /* Alocation the structure to hold the addresses. */
  section_addrs = alloc_section_addr_info (num_text + num_ocm_text + num_data);
  section_index = 0;

  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      /*
       * Linux ignores .modinfo and __versions sections.  GDB needs to
       * follow the same rules as Linux.
       */
      if (strcmp(s->name, ".modinfo") == 0 ||
	  strcmp(s->name, "__versions") == 0)
        {
	  // Ignore .modinfo and __versions sections.  Do nothing.
        }
      /*
       * OCM text sections are not adjacent to standard text sections.
       */
      else if (strncmp(s->name, ".ocm_text", strlen(".ocm_text")) == 0)
	{
	  section_addrs->other[section_index].name = strdup(s->name);
	  make_cleanup(xfree, section_addrs->other[section_index].name);
	  section_addrs->other[section_index].addr = dyn_ocm_text_addr;
	  dyn_ocm_text_addr += bfd_get_section_size (s);
	  section_index ++;
	}
      /*
       * Text sections are marked with the ALLOC flag and the CODE flag.
       */
      else if ((s->flags & (SEC_ALLOC|SEC_CODE)) == (SEC_ALLOC|SEC_CODE) &&
	       strncmp(s->name, ".init", strlen(".init")) != 0)
	{
	  section_addrs->other[section_index].name = strdup(s->name);
	  make_cleanup(xfree, section_addrs->other[section_index].name);
	  section_addrs->other[section_index].addr = dyn_text_addr;
	  dyn_text_addr += bfd_get_section_size (s);
	  section_index ++;
	}
      /*
       * Data sections are marked with the ALLOC flag and no CODE flag.
       */
      else if ((s->flags & (SEC_ALLOC|SEC_CODE)) == SEC_ALLOC)
	{
	  unsigned long alignment_mask = (1 << s->alignment_power) - 1;
	  dyn_data_addr = (dyn_data_addr + alignment_mask) & ~alignment_mask;
	  section_addrs->other[section_index].name = strdup(s->name);
	  make_cleanup(xfree, section_addrs->other[section_index].name);
	  section_addrs->other[section_index].addr = dyn_data_addr;
	  dyn_data_addr += bfd_get_section_size (s);
	  section_index ++;
	}
    }

  if (section_index != (num_text + num_ocm_text + num_data))
    {
      error("#of output sections does not match input.");
    }

  bfd_close (loadfile_bfd);
  symbol_file_add (filename, from_tty, section_addrs, 0, flags);
  reinit_frame_cache ();
  do_cleanups (my_cleanups);

  /* find the second last file on the object_file list. The newly added file is the last file on the list. */
  ofiles = object_files;
  while(ofiles->next->next)
    {
      ofiles = ofiles->next;
    }

  if (ofiles == NULL)
    error("Cannot find %s of the object_file list\n", bfd_get_filename(loadfile_bfd));

  /* Find the 2nd last files psymtab. */
  psymtabs = ofiles->psymtabs;

  if (psymtabs == NULL)
    {
      /* Degenerate case. */
      ofiles->psymtabs = ofiles->next->psymtabs;
      return;
    }

  /* find the last entry on this psymtab linked list. */
  while(psymtabs->next)
    psymtabs = psymtabs->next;

  /* We have found the last entry. Chain the new file psymtab linked list to object_files psymtab linked list. */
  psymtabs->next = ofiles->next->psymtabs;
}

/* ubicom32v3_ocm_load_run.  */
static void
ubicom32v3_ocm_load_run (char *arg, int from_tty)
{
  bfd *loadfile_bfd;
  asection *s;
  int total_length = 0;
  int progress = 0;
  int addr, length, offs, size, endaddr;
  unsigned char *data, *verifybuf, *protect;
  unsigned int last_address = 0;
  unsigned int first_address = 0xffffffff;
  int ret;

  char *file;

  size = UBICOM32V3_LOAD_BLOCK_SIZE;
  data = malloc (size *2 );
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for write\n");
      return;
    }
  verifybuf = &data[size];

  file = get_exec_file(1);

  if (remote_debug)
    printf_filtered ("ubicom32v3_load: %s (tty=%d)\n", file, from_tty);

  /* Open the file.  */
  loadfile_bfd = bfd_openr (file, gnutarget);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  endaddr = -1;
  /* Loop through all the sections and find the OCM sections and get an estimate of how much we are loading. */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32v3_loadable_section (loadfile_bfd, s) != UBICOM32_OCM_SECTION)
	continue;

      length = bfd_get_section_size (s);
      addr = s->lma;
      if ((addr + length) > endaddr)
	endaddr = addr + length;
    }

  /* round up endaddr to word boundary. */
  endaddr += 7;
  endaddr &= ~0x3;

  /* Reset the system. */
  ret = ubicom32v3isp_reset();

  /* set up the fast downloader */
  ret = ubicom32v3isp_download_fast_transfer_code(endaddr);

  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32v3_loadable_section (loadfile_bfd, s) != UBICOM32_OCM_SECTION)
	continue;

      length = bfd_get_section_size (s);

      if (remote_debug)
	printf_filtered("%s: 0 / %d\n", s->name, length);

      size = UBICOM32V3_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  unsigned int retlen;
	  int readtransfer;
	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  retlen = ubicom32v3_write_bytes(addr, (char *)data, size);

	  /* read the data and verify */
	  readtransfer = size;
	  if (size %4)
	    {
	      /* round up size to a 4 byte boundary */
	      size += 3;
	      size &= ~3;
	    }

	  ret = ubicom32v3UltraFastReadMemoryRaw(addr, size/4, (int *)verifybuf);
	  if (memcmp (data, verifybuf, readtransfer) != 0)
	    {
	      int i;
	      char *badblock = verifybuf;
	      char *cmpblock = data;
	      int currentaddress = addr;
	      for(i = 0; i< readtransfer; i++, currentaddress++) {
		if(badblock[i] != cmpblock[i]){
		  printf("addr = 0x%08x expect 0x%02hhx got 0x%02hhx\n", currentaddress, (unsigned)cmpblock[i], (unsigned)badblock[i]);
		  break;
		}
	      }
	    }

	  offs += readtransfer;
	  length -= readtransfer;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %d\n", s->name, length);
	}
    }

  free(data);
  if(s != NULL)
    {
      printf_filtered("Ocm download and run failed.\n");
      return;
    }

  /* Code is downloaded and verified. */
  /* guarantee that thread0 will be alive when we give it control. */
  ret = ubicom32v3isp_make_thread0_alive();

  /* now transfer control to the .downloader section */
  addr = 0x3ffc0000;

  ret = current_cpu->write_register_fn(ubicom32v3currentThread, 34, (void *)&addr, 1);
  ret= ubicom32v3restartProcessor(1);

  if(ret)
    {
      printf_filtered("Ocm Run failed.\n");
    }
}

/* ubicom32v3_verify.  */
static void
ubicom32v3_verify (char *arg, int from_tty)
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

  size = UBICOM32V3_LOAD_BLOCK_SIZE;
  data = malloc (size *2 );
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for write\n");
      return;
    }
  verifybuf = &data[size];

  file = get_exec_file(1);

  if (remote_debug)
    printf_filtered ("ubicom32v3_load: %s (tty=%d)\n", file, from_tty);

  /* Open the file.  */
  loadfile_bfd = bfd_openr (file, gnutarget);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  /* set up the fast downloader */
  ret = ubicom32v3isp_download_fast_transfer_code(current_cpu->ocm_start);

  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32v3_loadable_section (loadfile_bfd, s) != UBICOM32_FLASH_SECTION)
	continue;

      length = bfd_get_section_size (s);

      if (remote_debug)
	printf_filtered("%s: 0 / %d\n", s->name, length);

      size = UBICOM32V3_LOAD_BLOCK_SIZE;

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
	  //readtransfer= ubicom32v3_read_bytes(addr, verifybuf, size);
	  readtransfer = size;
	  if (size %4)
	    {
	      /* round up size to a 4 byte boundary */
	      size += 3;
	      size &= ~3;
	    }

	  ret = ubicom32v3UltraFastReadMemoryRaw(addr, size/4, (int *)verifybuf);
	  if (memcmp (data, verifybuf, readtransfer) != 0)
	    {
	      int i;
	      char *badblock = verifybuf;
	      char *cmpblock = data;
	      int currentaddress = addr;
	      for(i = 0; i< readtransfer; i++, currentaddress++) {
		if(badblock[i] != cmpblock[i]){
		  printf("addr = 0x%08x expect 0x%02hhx got 0x%02hhx\n", currentaddress, (unsigned)cmpblock[i], (unsigned)badblock[i]);
		  break;
		}
	      }
	    }

	  offs += readtransfer;
	  length -= readtransfer;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %d\n", s->name, length);
	}
    }

  free(data);

}

/* ubicom32v3_load.  */
static void
ubicom32v3_load (char *file, int from_tty)
{
  bfd *loadfile_bfd;
  asection *s, *downloadersection;
  int total_length = 0;
  int progress = 0;
  int addr, length, offs, size;
  unsigned char *data, *protect, *foundK;
  unsigned int last_address = 0;
  unsigned int first_address = 0xffffffff;
  int ret;
  int has_downloader = 0;
  unsigned int reply;
  CORE_ADDR ocm_begin;
  extern int restart_watchdog;
  extern int leakage_enable;

  if (remote_debug)
    printf_filtered ("ubicom32v3_load: %s (tty=%d)\n", file, from_tty);

  /* Open the file.  */
  loadfile_bfd = bfd_openr (file, gnutarget);
  if (loadfile_bfd == NULL)
    error ("Error: Unable to open file %s\n", file);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      bfd_close (loadfile_bfd);
      error ("Error: File is not an object file\n");
    }

  /*
   * Scan through the sections looking for the .downloader section. We better find it
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

      has_downloader = 1;
      break;
    }

  if(has_downloader == 0)
    {
      printf_unfiltered("%s has no \".downloader\" section. Unable to download code.\n", file);
      return;
    }

  downloadersection = s;
  length = bfd_get_section_size (s);

  /* Get the downloader section data. */
  data = malloc(length+64);
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for .downloader section data.\n");
      return;
    }

  offs = 0;

  /* read in the .downloader section data from the the file. */
  bfd_get_section_contents (loadfile_bfd, s, data, offs, length);

  /* xxxxxxxxxxxxxxxxxxxxxxxxxx call the backed init */
  ocm_begin = ubicom32v3_lookup_symbol("__ocm_begin");
  if (ocm_begin == 0)
    {
      printf_unfiltered("No __ocm_begin symbol. Unable to initialize Boot Kernel\n");
      free(data);
      ubicom32v3_reset(0,0);
      return;
    }

  /*
   * Turn off the 2 wire interface if possible.
   */
  ret = turnOff2wire();

  /* We are clobbering the system. Wipe out the restart watchdog flag. */
  restart_watchdog = 0;
  leakage_enable = 0;

  ret = ubicom32v3isp_initBackendToBootKernel(length, (unsigned int*)data, ocm_begin);
  if(ret)
    {
      printf_unfiltered("Unable to initialize Boot Kernel\n");
      free(data);
      ubicom32v3_reset(0,0);
      return;
    }

  free(data);
  size = UBICOM32V3_LOAD_BLOCK_SIZE;
  data = malloc(size*2);
  if (!data)
    {
      printf_unfiltered ("error: unable to allocate memory for write\n");
      return;
    }

  /* xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx */

  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      unsigned char buf[4];
      CORE_ADDR length;
      int sec;
      unsigned int section_last_address;

      sec = ubicom32v3_loadable_section (loadfile_bfd, s);
      if (sec != UBICOM32_FLASH_SECTION)
	continue;

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

  last_address -= first_address;

  /*
   * Turn off the 2 wire interface if possible.
   */
  ret = turnOff2wire();

  /* tell spt that we are going to start code download. spt will issue the chip erase.
     This will block till the chip erase returns. */
  data[0] = 0;
  sprintf(data,"Begin Erase.\n 0x%x length 0x%x", first_address, last_address);
  printf("%s\n", data);

  ret = eraseVerifyFlash(first_address, last_address, ubicom32_erase_progress_callback, NULL);
  if(ret)
    return;

  printf("Begin download\n");
  /* Write flash.  */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32v3_loadable_section (loadfile_bfd, s) != UBICOM32_FLASH_SECTION)
	continue;

      length = bfd_get_section_size (s);

      if (remote_debug)
	printf_filtered("%s: 0 / %d\n", s->name, length);
      ubicom32v3_show_load_progress_default (NULL, 0, length, progress, total_length);

      size = UBICOM32V3_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  if(size > length)
	    size = length;

	  addr = s->lma + offs;

	  /*
	   * If the lead address is not aligned to a flash write line boundary then
	   * Send enough so that all subsequent transfers will be aligned to
	   * flash write line boundary.
	   */
	  if(addr & 0xff)
	    {
	      /* the start is misaligned */
	      int endaddr = addr + size;

	      if((endaddr & ~0xff) > addr)
		{
		  endaddr &= ~0xff;
		  size = endaddr - addr;
		}
	    }

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  ubicom32v3_write_flash_bytes(addr, data, size);

	  offs += size;
	  length -= size;
	  progress += size;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %d\n", s->name, length);
	  ubicom32v3_show_load_progress_default (NULL, 0, length, progress, total_length);

	}
    }

  /* Verify flash.  */
  for (s = loadfile_bfd->sections; s; s = s->next)
    {
      if (ubicom32v3_loadable_section (loadfile_bfd, s) != UBICOM32_FLASH_SECTION)
	continue;

      length = bfd_get_section_size (s);

      if (remote_debug)
	printf_filtered("%s: 0 / %d\n", s->name, length);

      size = UBICOM32V3_LOAD_BLOCK_SIZE;

      offs = 0;
      while(length)
	{
	  if(size > length)
	    size = length;

	  /* read in the data size bytes at a time from the section */
	  bfd_get_section_contents (loadfile_bfd, s, data, offs, size);

	  addr = s->lma + offs;

	  if (common_verify_bytes(addr, data, size) < 0)
	    printf_unfiltered("verify error in section %s (0x%08x - 0x%08x)\n", s->name, addr, addr + size);


	  offs += size;
	  length -= size;
	  progress += size;

	  if (remote_debug)
	    printf_filtered("%s: 0 / %d\n", s->name, length);
	}
    }

  ret = ubicom32v3stopProcessor();

  /* Turn on turbo 2 wire if possible. */
  ret = turnOn2wire();
  free(data);

  /* reset and restart */
  ubicom32v3isp_attach();
  ubicom32v3_reset(0, 0);
}

static void
ubicom32v3_verify_reg_read (char *file, int from_tty)
{
  extern void verify_reg_read(void);

  verify_reg_read();
}

#if 0
static void
ubicom32v3_mail_status (char *file, int from_tty)
{
  int status;
  extern int readStatus(int *);
  int ret = readStatus(&status);
  printf("mail status = 0x%8x\n", status);
}

static void
ubicom32v3_read_mailbox (char *file, int from_tty)
{
  int mboxData=0xaa55aa55;
  extern int readMailbox(int *);
  int ret = readMailbox(&mboxData);
  printf("Ret = %d Mailbox sent = 0x%8x\n", ret,  mboxData);
}
#endif

static int
ubicom32v3_insert_breakpoint (struct bp_target_info *bp_tgt)
{
  unsigned int *bp = (unsigned int *)gdbarch_breakpoint_from_pc(current_gdbarch, &bp_tgt->placed_address, &bp_tgt->placed_size);
  CORE_ADDR addr = bp_tgt->placed_address;
  unsigned int bpToSend;
  char *cptr = (char *)&bpToSend;
  int ret;

  memcpy((void *)&bpToSend, (void *)bp, bp_tgt->placed_size);
  cptr[3] =  (0xff);

  if (ubicom32v3_ops.to_translate_address != NULL)
    {
      addr = ubicom32v3_ops.to_translate_address(&ubicom32v3_ops, addr);
    }

  /* make sure the address is in the PRAM space. XXXXXX May change in the future */
  if(addr >= current_cpu->ocm_start &&
     current_cpu->ddr_end > addr)
    {
      /* valid breakpointable address */

      /* read out the old contents */
      ret = ubicom32v3_read_bytes (addr, bp_tgt->shadow_contents, bp_tgt->placed_size);

      if(ret!= bp_tgt->placed_size)
	return -1;
      /* write in the break point */
      ret = ubicom32v3_write_bytes (addr, (char *) &bpToSend, bp_tgt->placed_size);

      if(ret == bp_tgt->placed_size)
	{
	  /* flush dcache and invalidate the icache */
	  ret = ubicom32v3cacheflushinvalidate(addr, bp_tgt->placed_size);
	  return 0;
	}
      else
	return -1;
    }

  return EINVAL;
}

static int
ubicom32v3_remove_breakpoint (struct bp_target_info *bp_tgt)
{
  unsigned int *bp = (unsigned int *)gdbarch_breakpoint_from_pc(current_gdbarch, &bp_tgt->placed_address, &bp_tgt->placed_size);
  CORE_ADDR addr = bp_tgt->placed_address;
  int ret, i;
  char *prev_contents = (char *)malloc(bp_tgt->placed_size);
  char *cptr = (char *) bp;
  int *iptr = (int *)prev_contents;

  cptr[3] = 0xff;

  if (ubicom32v3_ops.to_translate_address != NULL)
    {
      addr = ubicom32v3_ops.to_translate_address(&ubicom32v3_ops, addr);
    }

  /* make sure the address is a breakpointable address */
  if(addr >= current_cpu->ocm_start &&
     current_cpu->ddr_end > addr)
    {

      /* read out the old contents */
      ret = ubicom32v3_read_bytes (addr, prev_contents, bp_tgt->placed_size);

      /* check if we do have a bp instruction at this address */
      for(ret =0; ret< bp_tgt->placed_size-1; ret++)
	if(cptr[ret] != prev_contents[ret])
	  return 0;

      /* write in the old contents */
      ret = ubicom32v3_write_bytes (addr, (char *) bp_tgt->shadow_contents, bp_tgt->placed_size);

      if(ret == bp_tgt->placed_size)
	{
	  /* flush dcache and invalidate the icache */
	  ret = ubicom32v3cacheflushinvalidate(addr, bp_tgt->placed_size);
	  return 0;
	}
      else
	return -1;
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
ubicom32v3_threads_extra_info (struct thread_info *tp)
{
  unsigned int thread_num;
  extern char * ubicom32v3_remote_threads_extra_info (struct thread_info *tp);
  extern void gdbarch_info_init (struct gdbarch_info *info);
  thread_num = PIDGET(tp->ptid);

  ubicom32v3_set_thread(thread_num);

  return (ubicom32v3_remote_threads_extra_info(tp));
}

CORE_ADDR 
ubicom32v5_translate_address (struct target_ops *ops, CORE_ADDR addr)
{
  return ubicom32v5translate_virtmem (addr);
}

CORE_ADDR (*target_translate_address) (struct target_ops *, CORE_ADDR addr) = NULL;

/* init_ubicom32v3_ops.  */
static void
init_ubicom32v3_ops (void)
{
  ubicom32v3_ops.to_shortname = "ubicom32v3";
  ubicom32v3_ops.to_longname = "Remote ubicom32v3 debug Via Ubicom Ethernet Dongle";
  ubicom32v3_ops.to_doc = "Remote ubicom32 debug Via Ubicom Ethernet Dongle.\n\
Connect to target ubicom32 board as follows:\n\
target ubicom32 dongle-ip-address:5010\n\
";
  ubicom32v3_ops.to_open = ubicom32v3_open;
  ubicom32v3_ops.to_close = ubicom32v3_close;
  ubicom32v3_ops.to_attach = ubicom32v3_attach;
  ubicom32v3_ops.to_post_attach = NULL;
  ubicom32v3_ops.to_detach = ubicom32v3_detach;
  ubicom32v3_ops.to_resume = ubicom32v3_resume;
  ubicom32v3_ops.to_wait = ubicom32v3_wait;
  //ubicom32v3_ops.to_post_wait = NULL;
  ubicom32v3_ops.to_fetch_registers = ubicom32v3_fetch_register;
  ubicom32v3_ops.to_store_registers = ubicom32v3_store_register;
  ubicom32v3_ops.to_prepare_to_store = ubicom32v3_prepare_to_store;
  ubicom32v3_ops.deprecated_xfer_memory = ubicom32v3_xfer_memory;
  ubicom32v3_ops.to_files_info = ubicom32v3_files_info;
  ubicom32v3_ops.to_kill = ubicom32v3_kill;
  ubicom32v3_ops.to_load = ubicom32v3_load;
  ubicom32v3_ops.to_insert_breakpoint = ubicom32v3_insert_breakpoint;
  ubicom32v3_ops.to_remove_breakpoint = ubicom32v3_remove_breakpoint;

  ubicom32v3_ops.to_terminal_init = NULL;
  ubicom32v3_ops.to_terminal_inferior = NULL;
  ubicom32v3_ops.to_terminal_ours_for_output = NULL;
  ubicom32v3_ops.to_terminal_ours = NULL;
  ubicom32v3_ops.to_terminal_info = NULL;
  ubicom32v3_ops.to_lookup_symbol = NULL;
  ubicom32v3_ops.to_create_inferior = NULL;
  ubicom32v3_ops.to_post_startup_inferior = NULL;
  ubicom32v3_ops.to_acknowledge_created_inferior = NULL;
  ubicom32v3_ops.to_insert_fork_catchpoint = NULL;
  ubicom32v3_ops.to_remove_fork_catchpoint = NULL;
  ubicom32v3_ops.to_insert_vfork_catchpoint = NULL;
  ubicom32v3_ops.to_remove_vfork_catchpoint = NULL;
  ubicom32v3_ops.to_insert_exec_catchpoint = NULL;
  ubicom32v3_ops.to_remove_exec_catchpoint = NULL;
  ubicom32v3_ops.to_reported_exec_events_per_exec_call = NULL;
  ubicom32v3_ops.to_has_exited = NULL;
  ubicom32v3_ops.to_mourn_inferior = NULL;
  ubicom32v3_ops.to_can_run = 0;
  ubicom32v3_ops.to_notice_signals = 0;
  ubicom32v3_ops.to_thread_alive = ubicom32v3_thread_alive;
  ubicom32v3_ops.to_pid_to_str = ubicom32v3_thread_pid_to_str;
  ubicom32v3_ops.to_stop = ubicom32v3_stop;
  ubicom32v3_ops.to_pid_to_exec_file = NULL;
  ubicom32v3_ops.to_stratum = process_stratum;
  ubicom32v3_ops.to_has_all_memory = 1;
  ubicom32v3_ops.to_has_memory = 1;
  ubicom32v3_ops.to_has_stack = 1;
  ubicom32v3_ops.to_has_registers = 1;
  ubicom32v3_ops.to_has_execution = 1;
  ubicom32v3_ops.to_sections = NULL;
  ubicom32v3_ops.to_sections_end = NULL;
  ubicom32v3_ops.to_magic = OPS_MAGIC;
  ubicom32v3_ops.to_extra_thread_info = ubicom32v3_threads_extra_info;
  ubicom32v3_ops.to_translate_address = NULL;
}

extern int ubicom32v3pollThreads(unsigned int threadNo, char *args);

unsigned int poll_thread;
static void
myubicom32v3_set_thread (char *args, int from_tty)
{
  sscanf(args, "%d", &poll_thread);
}

static void
ubicom32v3_poll_thread (char *args, int from_tty)
{
  (void) ubicom32v3pollThreads(poll_thread, args);
}

static void
ubicom32v3_trap_ignore(char *args, int from_tty)
{
  ignore_trap = 1;
}

static void
ubicom32v5_info_mmu (char *addr_exp, int from_tty)
{
  int thread;

  if (!target_has_registers) 
    error ("No target specified");

  ubicom32v5ReadMMU ();
  printf_unfiltered ("MMU at 0x%8.8x\n", UBICOM32V5_MMU);
  printf_unfiltered ("  config   = 0x%8.8x, ptec_cfg0 = 0x%8.8x, ptec_cfg1 = 0x%8.8x, ptec_cfg2 = 0x%8.8x\n",
		     ubicom32v5mmu.config, ubicom32v5mmu.ptec_cfg0, ubicom32v5mmu.ptec_cfg1, ubicom32v5mmu.ptec_cfg2);
  printf_unfiltered ("  missqw0  = 0x%8.8x, missqw1   = 0x%8.8x, restart   = 0x%8.8x, insertw0  = 0x%8.8x\n",
		     ubicom32v5mmu.missqw0, ubicom32v5mmu.missqw1, ubicom32v5mmu.restart, ubicom32v5mmu.insertw0);
  printf_unfiltered ("  insertw1 = 0x%8.8x, i_purge   = 0x%8.8x, d_purge   = 0x%8.8x, ptec_err  = 0x%8.8x\n",
		     ubicom32v5mmu.insertw1, ubicom32v5mmu.i_purge, ubicom32v5mmu.d_purge, ubicom32v5mmu.ptec_err);
  printf_unfiltered ("  tlb_idx  = 0x%8.8x, entryw0   = 0x%8.8x, entryw1   = 0x%8.8x, status    = 0x%8.8x\n",
		     ubicom32v5mmu.tlb_idx, ubicom32v5mmu.entryw0, ubicom32v5mmu.entryw1, ubicom32v5mmu.status);
  printf_unfiltered ("  bus_st0  = 0x%8.8x, bus_st1   = 0x%8.8x, bus_st2   = 0x%8.8x, bus_st3   = 0x%8.8x\n",
		     ubicom32v5mmu.bus_st0, ubicom32v5mmu.bus_st1, ubicom32v5mmu.bus_st2, ubicom32v5mmu.bus_st3);
  printf_unfiltered ("  bus_st4  = 0x%8.8x, bus_st5   = 0x%8.8x\n",
		     ubicom32v5mmu.bus_st4, ubicom32v5mmu.bus_st5);

  printf_unfiltered ("   thread    pgd       asids    asid_cmp  asid_mask\n");
  for (thread = 0; thread < 12; thread++)
    {
      printf_unfiltered ("     %2d  0x%8.8x 0x%8.8x 0x%8.8x 0x%8.8x\n",
			 thread + 1, ubicom32v5mmu.thread[thread].pgd, ubicom32v5mmu.thread[thread].asids, 
			 ubicom32v5mmu.thread[thread].asid_cmp, ubicom32v5mmu.thread[thread].asid_mask);
    }
}

/* Display PGD table.  
   (gdb) info pgd [<threadNo> | <pgd_address>]  */
static void
ubicom32v5_info_pgd (char *addr_exp, int from_tty)
{
  int entry;
  int thread = inferior_ptid.pid;
  unsigned int val;
  unsigned int pgd_addr = 0;
  unsigned int *pgd; 
  int alloc = 0;

  if (addr_exp && *addr_exp != '\0') 
    {
      val = strtoul (addr_exp, 0, 0);
      if (val <= 12)
	thread = val;
      else 
	{
	  thread = 0;
	  pgd_addr = val;
	}
    }

  if (pgd_addr == 0)
    {
      pgd = ubicom32v5ReadPGD (thread);
      if (pgd == 0)
	error ("thread has no PGD\n");
      pgd_addr = ubicom32v5mmu.thread[thread-1].pgd;
    }
  else 
    pgd = ubicom32v5ReadPGD_1 (pgd_addr, 0);

  /* Print PGD table.  */
  if (thread)
    printf_unfiltered ("Thread %d  ", thread);
  printf_unfiltered ("PGD @ 0x%8.8x:", pgd_addr);
  for (entry = 0; entry < PGD_TABLE_SIZE; entry++)
    {
      if ((entry % 4) == 0) 
	printf ("\n  %2.2x:  ", entry);
      printf_unfiltered ("%8.8x  ", pgd[entry]);
    }
  printf_unfiltered ("\n");
}

static void
print_pte (char *tag, ubicom32v5PTE_t pte)
{
  printf_unfiltered ("%s address = 0x%8.8x", tag, pmd_address(pte));
  printf_unfiltered ("  flags = { ");
  if (pte.val.valid) printf_unfiltered ("valid ");
  if (pte.val.user) printf_unfiltered ("user ");
  if (pte.val.execute) printf_unfiltered ("execute ");
  if (pte.val.write) printf_unfiltered ("write ");
  if (pte.val.read) printf_unfiltered ("read ");
  if (pte.val.cache) printf_unfiltered ("cache ");
  if (pte.val.young) printf_unfiltered ("young ");
  if (pte.val.bufferable) printf_unfiltered ("bufferable ");
  if (pte.val.dirty) printf_unfiltered ("dirty ");
  printf_unfiltered (" }\n");
}

/* Describe virtual address.  */
static void
ubicom32v5_info_vaddr (char *addr_exp, int from_tty)
{
  unsigned int addr, pgd_addr, pte_addr, phys_addr;
  unsigned int *pgd;
  ubicom32v5PTE_t pte, *pte_table;

  if (!addr_exp || *addr_exp == '\0') 
    error ("virtual address missing.");
  
  addr = strtoul (addr_exp, 0, 0);
  if (!(UBICOM32V5_MAP_START <= addr && addr <= UBICOM32V5_MAP_END))
    error ("unmapped address");

  printf_unfiltered ("addr = {pgd_index = 0x%2.2x, pte_index = 0x%4.4x, ofs = 0x%4.4x}\n",
		     pgd_index(addr), pte_index(addr), addr_offset(addr));

  ubicom32v5ReadMMU ();
  pgd_addr = ubicom32v5mmu.thread[ubicom32v3_current_thread-1].pgd;
  pgd = ubicom32v5ReadPGD (inferior_ptid.pid);
  if (pgd == 0) 
    error ("invalid address");
  pte.word = pgd[pgd_index(addr)];

  printf_unfiltered ("pgd base @ 0x%8.8x, pgd[0x%2.2x] = 0x%8.8x\n", 
		     pgd_addr, pgd_index(addr), pte.word);
  print_pte ("pmd", pte);

  pte_table = ubicom32v5ReadPTE (pmd_address(pte));
  pte = pte_table[pte_index(addr)];
  printf_unfiltered ("pte[0x%4.4x] = 0x%8.8x\n", pte_index(addr), pte.word);
  print_pte ("pte", pte);

  phys_addr = pmd_address(pte) + addr_offset(addr);
  printf_unfiltered ("physical address = 0x%8.8x\n", phys_addr);
}

/* Set internal MMU context values.  */
static void
ubicom32v5_setmmucontext (char *args, int from_tty)
{
  unsigned thread, pgd, asids, asid_cmp, asid_mask;

  if (!args || *args == '\0') 
    error ("Usage: setmmucontext <thread> <pgd-addr> <asids> <asid_cmp> <asid_mask>");
  thread = strtoul (args, &args, 0);
  pgd = strtoul (args, &args, 0);
  asids = strtoul (args, &args, 0);
  asid_cmp = strtoul (args, &args, 0);
  asid_mask = strtoul (args, &args, 0);
  
  if (thread == 0 || thread > 12)
    printf_unfiltered ("invalid thread value: %d\n", thread);

  ubicom32v5mmu.thread[thread-1].pgd = pgd;
  ubicom32v5mmu.thread[thread-1].asids = asids;
  ubicom32v5mmu.thread[thread-1].asid_cmp = asid_cmp;
  ubicom32v5mmu.thread[thread-1].asid_mask = asid_mask;
}

void
_initialize_ubicom32v3 (void)
{
  extern void common_reset(char *args, int from_tty);

  init_ubicom32v3_ops ();

  add_target (&ubicom32v3_ops);
  //add_com ("myreset", class_obscure, ubicom32v3_reset, "reset target.");
  /* Backward compatability.  */
  add_com ("ubicom32v3reset", class_obscure, common_reset,
	   "Backward compatability - use 'reset'.");
  add_com ("tpoll", class_obscure, ubicom32v3_poll_thread,
	   "Thread polling command.");
  add_com ("trapign", class_obscure, ubicom32v3_trap_ignore,
	   "Set debugger to ignore trap events.");
  add_com ("tset", class_obscure, myubicom32v3_set_thread,
	   "Sets thread for polling command.");
  add_com ("specialmon", class_obscure, ubicom32v3_special_mon,
	   "Hardware sampling command.");
  add_com ("bisttest", class_obscure, ubicom32v3_bist_test,
	   "Run bist sequence from gdb.");
  add_com ("plltest", class_obscure, ubicom32v3_pll_test,
	   "Run bist sequence from gdb.");
  add_com ("ddrtest", class_obscure, ubicom32v3_ddr_test,
	   "Run bist sequence from gdb.");
  add_com ("loadtest", class_obscure, ubicom32v3_load,
	   "Run bist sequence from gdb.");
  add_com ("prev_pc", class_obscure, ubicom32v3_previous_pc,
	   "Load PC with value from previous_pc register.");
  add_com ("cacheflush", class_obscure, ubicom32v3_cache_flush,
	   "Flush D-cache and I-cache from for particular address gdb.");
  add_com ("verifyregread", class_obscure, ubicom32v3_verify_reg_read,
	   "Flush D-cache and I-cache from for particular address gdb.");
  add_com ("verify5k", class_obscure, ubicom32v3_verify,
	   "Used to verify contents of flash.");
  add_com ("ocmrun", class_obscure, ubicom32v3_ocm_load_run,
	   "Used to load code into OCM and run it.");
  add_com ("add-module-file", class_obscure, ubicom32v3_add_module_file,
	   "Used to load symbols from a loadable module.");
  add_com ("erase-flash", class_obscure, ubicom32v3_erase_flash,
	   "Used to erase flash. Usage erase-flash start_addr length");
  add_com ("create-user-coredump", class_obscure, create_user_coredump,
	   "Start of creation of a user core dump. Usage: create-user-coredump Name srart_args end_args");
  add_com ("user-coredump-add-mem", class_obscure, user_coredump_add_mem,
	   "Adding memory information to user coredumps. Usage: user-coredump-add-mem: start_ptr length\n");
  add_com ("user-coredump-add-virtual-mem", class_obscure, user_coredump_add_virtual_mem,
	   "Adding virtual memory information to user coredumps. Usage: user-coredump-add-virtual-mem: virtual_start_address real_start_address length\n");
  add_com ("user-coredump-add-reg", class_obscure, user_coredump_add_reg,
	   "Adding register information to user coredumps. Usage: user-coredump-add-reg: pid_ptr pt_regs_ptr loadmap_ptr sizeof pt_regs");
  add_com ("close-user-coredump", class_obscure, close_user_coredump,
	   "Close and write out an open user coredump. Usage: close-user-coredump\n");
  add_com ("ubicom32v3coredump", class_obscure, ubicom32v3_core_dump,
	   "Used to create a core dump of the machine state.\n\
Usage:\n\
coredump\n\
\tThis will dump core to a file called Ubicom.core \n\twith 2MB of sdram dump.\n\n\
coredump n\n\
\tThis will dump core to a file called Ubicom.core\n\twith nMB of sdram dump.\n\tn is a number between 1 - 16 inclusive.\n\n\
coredump corefilename\n\
\tThis will dump core to a file called \"corefilename\"\n\twith 2MB of sdram dump.\n\n\
coredump corefilename n\n\
\tThis will dump core to a file called \"corefilename\"\n\twith nMB of sdram dump.\n\
\tn is a number between 1 - 16 inclusive.\n");
  add_com ("ubicom-set-pc", class_obscure, change_pc,
	   "Modifies PC in regcache to the passed value. Usage: ubcom-set-pc: pc_value");
  add_com ("ubicom-set-sp", class_obscure, change_sp,
	   "Modifies SP in regcache to the passed value. Usage: ubcom-set-sp: sp_value");
  add_com ("ubicom-set-regs", class_obscure, change_regs,
	   "Modifies registers. The data can be cut and pasted from a trap marker. Usage: ubcom-set-regs: pc: c026f328, previous_pc: c026f324");
  add_com ("ubicom-reset-frames", class_obscure, reset_frames,
	   "Reinitialize frame caches. Call after ubicom-set-regs operations. Usage: ubcom-reset-frames:");

#if 0
  add_com ("mstat", class_obscure, ubicom32v3_mail_status, "Printout mail box status.");
  add_com ("rdmail", class_obscure, ubicom32v3_read_mailbox, "Read data from mail box.");

  add_com ("mclear", class_obscure, ubicom32v3_mail_clear, "Load the \".protect\". section into Flash Boot sector.");
#endif

  add_info ("mmu", ubicom32v5_info_mmu, "Display MMU contents.");
  add_info ("pgd", ubicom32v5_info_pgd, "Display PGD table contents.");
  add_info ("vaddr", ubicom32v5_info_vaddr, "Display virtual address.");
  add_com ("setmmucontext", class_obscure, ubicom32v5_setmmucontext, "Set internal page translation values\n"
	   "Usage: setmmucontext <thread> <pgd-addr> <asids> <asid_cmp> <asid_mask>\n"
	   "Changes are internal to gdb; no changes are made to target");
}
