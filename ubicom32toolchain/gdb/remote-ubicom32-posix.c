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
#include "remote-ubicom32-posix-backend.h"
#include "ubicom32_dongle.h"
#include "ubicom32-tdep.h"

volatile int ubicom32_posix_cntrl_c_abort;
int ubicom32_posix_current_thread = -1;

enum posix_state {
  POSIX_RUNNING,
  POSIX_STOPPED,
  POSIX_DETACHED,
  POSIX_STEPPING,
  POSIX_STEPPING_PROBLEMS,
  POSIX_TRAP_ERROR,
  POSIX_CORE_STOPPED,
};
enum posix_state remote_posix_state = POSIX_DETACHED;
struct breakpoint_storage {
  unsigned int *bp_addresses;
  unsigned int current_entry;
  unsigned int num_entries;
} inserted_bps;

/* GDB structure. */
struct target_ops posix_ops;

#define UBICOM32_FLASH_ADDR 0x60000000
#define UBICOM32_FLASH_END 	0x6ffffffc
#define UBICOM32_LOAD_BLOCK_SIZE 2048

#define UBICOM32_DDR_ADDR 0x40000000
#define UBICOM32_DDR_END  0x47fffffc

#define UBICOM32_OCM_ADDR 0x3ffc0000
#define UBICOM32_OCM_END  0x3ffffffc

#define UBICOM32_FLASH_SECTION 1
#define UBICOM32_DDR_SECTION 2
#define UBICOM32_OCM_SECTION 3

#define REGISTER_RAW_SIZE(x)  (4)

static int ubicom32_posix_read_bytes (CORE_ADDR address, char *buffer, int length);
static int ubicom32_posix_write_bytes (CORE_ADDR address, char *buffer, int length);

struct ubicom32_posix_process gdb_proc;
unsigned int text_start, text_size, data_start, data_size;

/* Software Breakpoint 'instruction' (not actually a valid
 * instruction). Note that this used to be 0xaabbccdd, but in the mach
 * v4 this now actually legal.
 */

#define UBICOM32_V4_SOFTWARE_BREAKPOINT {0xfa, 0xbb, 0xcc, 0xdd}

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
  if ((s->lma >= UBICOM32_DDR_ADDR) && (s->lma + length <= UBICOM32_DDR_END))
    return UBICOM32_DDR_SECTION;
  if ((s->lma >= UBICOM32_OCM_ADDR) && (s->lma + length <= UBICOM32_OCM_END))
    return UBICOM32_OCM_SECTION;
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
  

void
posix_swizzled_supply_register (struct regcache *regcache, int regnum, const void *val)
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

static int
ubicom32_posix_insert_breakpoint (struct bp_target_info *bp_tgt)
{
  char bpToSend[] = UBICOM32_V4_SOFTWARE_BREAKPOINT;
  int ret, i;

  bp_tgt->placed_size = sizeof(bpToSend);
  
  /* We should do an address check ???? */
  /* Read back old contents. */
  ret = ubicom32_posix_read_bytes(bp_tgt->placed_address, bp_tgt->shadow_contents, bp_tgt->placed_size);
  if (ret != bp_tgt->placed_size)
    return -1;

  /* Write out the break point.*/
  ret = ubicom32_posix_write_bytes(bp_tgt->placed_address, (char *)&bpToSend, bp_tgt->placed_size);
  if (ret != bp_tgt->placed_size)
    return -1;

  /* We need to store the breakpoint in the inserted_bps structure. */
  if (inserted_bps.current_entry == inserted_bps.num_entries)
    {
      /* We need to allocate more space as we have no space for the new entry. */
      inserted_bps.num_entries += 10;
      inserted_bps.bp_addresses = realloc(inserted_bps.bp_addresses, inserted_bps.num_entries * sizeof(unsigned int));
      if (inserted_bps.bp_addresses == NULL)
	{
	  printf_unfiltered("Failed allocation inserted_bps array.");
	  exit(-1);
	}
    }

  /* If this entry is already in the list then don't worry about it. */
  for (i=0; i< inserted_bps.current_entry; i++)
    {
      if (inserted_bps.bp_addresses[i] == bp_tgt->placed_address)
	break;
    }
  if (i != inserted_bps.current_entry)
    return 0;

  inserted_bps.bp_addresses[inserted_bps.current_entry++] = bp_tgt->placed_address;
  return 0;
}

static int
ubicom32_posix_remove_breakpoint (struct bp_target_info *bp_tgt)
{
  char previous_contents[4];
  char expected_contents[] = UBICOM32_V4_SOFTWARE_BREAKPOINT;

  int ret = ubicom32_posix_read_bytes(bp_tgt->placed_address, (char *)&previous_contents, bp_tgt->placed_size);
  
  /* check if we do have a bp instruction at this address */
  for(ret =0; ret< bp_tgt->placed_size; ret++)
    if(expected_contents[ret] != previous_contents[ret])
      return 0;

  /* write out the original data */
  ret = ubicom32_posix_write_bytes(bp_tgt->placed_address, bp_tgt->shadow_contents, bp_tgt->placed_size);

  return 0;
}

int 
ubicom32_posix_attach(void)
{
  int ret, i;
  ret = ubicom32_remote_posix_attach(text_start, text_size, data_start, data_size, &gdb_proc);
  if (ret)
    {
      if (ret == 1 || ret == 2)
	{
	  /* Shut down the ethernet connection. */
	  ubicom32_remote_posix_close();
	  remote_posix_state = POSIX_DETACHED;
	  if (ret == 1)
	    error("No matching process found in target.");
	  else if (ret == 2)
	    error("Process is already being debugged.");
	}
      else if (ret == 3)
	{
	  /* 
	   * This is a freshly launched process. 
	   * We need to set a breakpoint at the entry point and let it go. Once it stops we go grab status.
	   */
	  struct bp_target_info bp_tgt;
	  bp_tgt.placed_address = bfd_get_start_address(exec_bfd);
	  (void) ubicom32_posix_insert_breakpoint(&bp_tgt);

	  /* Get it going. */
	  ret = ubicom32_remote_posix_restart(0);

	  /* now keep polling the dongle till the breakpoint gets hit. */
	  while (1)
	    {
	      ret = dongle_poll();
	      if(ret< 0)
		{
		  printf_unfiltered ("error: unable to read status\n");
		  return ret;
		}
	      if(ret == 0)
		{
		  /* wait for 10 ms before trying again */
		  usleep(10000);
		  continue;
		}

	      /* Remote has sent a debug event. Collect it. */
	      ret = ubicom32_remote_posix_get_gdb_event(&gdb_proc);
	      if(ret< 0)
		{
		  printf_unfiltered ("error: unable to read status\n");
		  return ret;
		}
	      break;
	    }

	  /* Remove the breakpoint we set. */
	  (void) ubicom32_posix_remove_breakpoint(&bp_tgt);
	}
      else
	error("Unknown response from the target.");
    }

  /* Go get the processor status. */
  ret = ubicom32_remote_posix_get_status(&gdb_proc);

  /* We have attached to the process. */
  printf("Attached to process 0x%x\n", gdb_proc.proc);

  return ret;
}

/* ubicom32_posix_open
   Usage: target remote <ip_addr>:<ip_port>
   Connects to the dongle and then connects to target through the dongle.  */
static void
ubicom32_posix_open (char *args, int from_tty)
{
  int ret, i, has_section;
  struct gdbarch_info info;
  extern void gdbarch_info_init (struct gdbarch_info *info);
  char *env_dongle;
  asection *text_section, *data_section;

  /* zero out the gdb_proc structure. */
  memset(&gdb_proc, 0, sizeof(gdb_proc));
  if (exec_bfd == NULL)
    {
      /* If no exec file is provided we will be unable to do any debugging. */
      error("No POSIX elf file provided. Please provide one.");
    }

  /* look for "__i_am_posix" from the symbol table. */
  if (ubicom32_lookup_symbol("__i_am_posix") == 0)
    {
      error("%s is not a ubicom32 POSIX file.", bfd_get_filename (exec_bfd));
    }

  /* find the .text section. */
  has_section = 0;
  for (text_section = exec_bfd->sections; text_section; text_section = text_section->next)
    {
      if(strcmp(text_section->name, ".text"))
	{
	  continue;
	}
      has_section=1;
      break;
    }

  if (has_section ==0 )
    {
      error("%s has no .text section.", bfd_get_filename (exec_bfd));
    }

  /* find the .data section. */
  for (data_section = exec_bfd->sections; data_section; data_section = data_section->next)
    {
      if(strcmp(data_section->name, ".data"))
	{
	  continue;
	}
      break;
    }

  if (has_section ==0 )
    {
      error("%s has no .data section.", bfd_get_filename (exec_bfd));
    }

  if (remote_debug)
    {
      if (args)
        printf_filtered ("ubicom32_posix_open (%s)\n", args);
      else
        printf_filtered ("ubicom32_posix_open ()\n");
    }

  target_preopen (from_tty);

  /* Default device.  */
  if (!args)
    {
      env_dongle = getenv("UBICOM_DONGLE");
      if (env_dongle)
	args = env_dongle;
      else
	args = "localhost:5011";
    }

  /* Connect to dongle.  */
  ret = ubicom32_remote_posix_connect (args);

  if(ret == -1)
    error ("Unable to find dongle.");

  /* Extract .text section start and size. */
  text_start = text_section->lma;
  text_size = bfd_get_section_size (text_section);

  /* Extract .data section start and size. */
  data_start = data_section->lma;
  data_size = bfd_get_section_size (data_section);

  /* Attach to the target process. */
  ret = ubicom32_posix_attach();
  if (ret)
    {
      return;
    }

  gdb_proc.current_thread = gdb_proc.pthreads[0];

  /*
   * The remote process will be in a stopped state.
   */
  remote_posix_state = POSIX_STOPPED;
  push_target(&posix_ops);

  /*
   * Fix the gdbarch.
   */
  if (current_gdbarch == NULL) 
    {
      struct gdbarch_info info;
      extern void gdbarch_info_init (struct gdbarch_info *info);
      info.bfd_arch_info = bfd_scan_arch ("ubicom32posix");
      
      if (info.bfd_arch_info == NULL)
	internal_error (__FILE__, __LINE__,
			"set_architecture: bfd_scan_arch failed");
      (void)gdbarch_update_p (info);
    }
  else
    {
      struct gdbarch_tdep *tdep = gdbarch_tdep(current_gdbarch);
      char *arch_string = (char *)tdep->name;
      if (strcmp(arch_string, "Ubicom32 POSIX"))
	{
	  /* Architecture mismatch. We need to go find the "ubicom32posix" arch. */
	  gdbarch_info_init (&info);
	  info.bfd_arch_info = bfd_scan_arch ("ubicom32posix");
	  
	  if (info.bfd_arch_info == NULL)
	    internal_error (__FILE__, __LINE__,
			    "set_architecture: bfd_scan_arch failed");
	  (void)gdbarch_update_p (info);
	}
    }

  inferior_ptid = ptid_build((int)gdb_proc.proc, 0, (long)gdb_proc.pthreads[0]);
  start_remote(from_tty);

  /* Add threads. */
  for (i=0; i< gdb_proc.num_pthreads; i++)
    {
      ptid_t new_ptid = ptid_build((int)gdb_proc.proc, 0, (long)gdb_proc.pthreads[i]);
      if (!in_thread_list(new_ptid))
	{
	  add_thread(new_ptid);
	}
    }
}

/* ubicom32_posix_close.  */
static void
ubicom32_posix_close (int quitting)
{
  if(remote_posix_state == POSIX_DETACHED)
    return;

  if (remote_debug)
    printf_filtered ("ubicom32_posix_close (%d)\n", quitting);

  /* Disconnect.  */
  (void)ubicom32_remote_posix_close();
  remote_posix_state = POSIX_DETACHED;
}

/* ubicom32_posix_detach.
   Detach from the remote board.  */
static void
ubicom32_posix_detach (char *args, int from_tty)
{
  (void) ubicom32_remote_posix_detach();
  (void) ubicom32_remote_posix_close();
  remote_posix_state = POSIX_DETACHED;
  pop_target();

}

struct step_targets {
  unsigned int target1_pc;
  unsigned char t1_insn[4];
  unsigned int target1_active;
  unsigned int target2_pc;
  unsigned char t2_insn[4];
  unsigned int target2_active;
} ss_targets;

static int
ubicom32_posix_write_bytes (CORE_ADDR address, char *buffer, int length)
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
      ret = ubicom32_remote_posix_read_memory(leadAddr, 1, (int *)cptr);

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
      ret = ubicom32_remote_posix_read_memory(endAddr, 1, (int *)cptr);

      for(i=0; i< endBytes; i++)
	{
	  cptr[3-i] = (unsigned char) *buffer++;
	}
    }

  // Send the data to target
  ret = ubicom32_remote_posix_write_memory(leadAddr, toWriteWords, (int *)sendBuffer);
  if (ret == 2)
    error("Address 0x%x is out of POSIX bounds", leadAddr);

  free (freeBuffer);
  return (leadBytes + midBytes + endBytes);
}

static int
ubicom32_posix_read_bytes (CORE_ADDR address, char *buffer, int length)
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

  ret = ubicom32_remote_posix_read_memory(leadAddr, toReadWords, (int *)recvBuffer);

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

static int ubicom32_posix_setup_single_step(void)
{
  CORE_ADDR pc = read_pc();
  unsigned char cptr[4];
  char bpToSend[] = UBICOM32_V4_SOFTWARE_BREAKPOINT;
  int i;

  /* pull the current instruction from target. */
  int ret = ubicom32_posix_read_bytes(pc, (char *)cptr, 4);
  unsigned int insn = cptr[0] <<24 | cptr[1] << 16 | cptr[2] << 8 | cptr[3];
  unsigned char base_opcode = (cptr[0] >> 3) & 0x1f;

  memset(&ss_targets, 0, sizeof(ss_targets));

  /* analyze the instruction */
  switch(base_opcode)
    {
    case 0x1a:
      {
	/* jump conditionals */
	int offset = (insn & 0x1fffff)<<2;
	;
	if (offset & 0x00400000) 
	  {
	    /* set up a negative number */
	    offset |= 0xff800000;
	  }
	ss_targets.target1_pc = pc + 4;
	ss_targets.target2_pc = pc + offset;
	ss_targets.target1_active = 1;
	ss_targets.target2_active = 1;
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
	ss_targets.target1_pc = pc + offset;
	ss_targets.target1_active = 1;
	break;
      }
    case 0x1e:
      {
	/* CALLI. Build the 18 bit offset from the instruction */
	int offset = ((int)(cptr[0] & 0x7))<<15 | ((int)(cptr[1] & 0x1f) <<10) | ((int)(cptr[2] & 0x7) << 7) | ((int)(cptr[3] & 0x1f) << 2);
	int address_register = (cptr[3] >> 5) & 0x7;

	if (offset & (1<<17))
	  {
	    offset |= ~((1 << 18) - 1);
	  }

	/* Get the base address from the address register in the instruction. */
	ss_targets.target1_pc = gdb_proc.current_thread_context.an[address_register];

	/* add the offset */
	ss_targets.target1_pc += offset;
	ss_targets.target1_active = 1;
	break;
      }
    case 0x00:
      {
	int op_extension = (int)(cptr[2] >> 3) & 0x1f;
	if (op_extension == 4)
	  {
	    /* RET. Extract the S1 field from the instruction. */
	    unsigned int ea;
	    int s1 = insn & 0x7ff;
	    int address_register = (s1 & 0xe0) >> 5;
	    int data_register = (s1 & 0xf);
	    int direct_register = (s1 & 0xff);
	    int pre = s1 & 0x10;
	    int indirect_offset = (((s1 & 0x300) >> 3) | (s1 & 0x1f)) << 2;
	    int four_bit_immediate = data_register << 28;
	    int eight_bit_immediate = direct_register << 24;
	    four_bit_immediate = four_bit_immediate >> 26;
	    eight_bit_immediate = eight_bit_immediate >> 22;

	    ss_targets.target1_active = 1;
	    if ((s1 & 0x700) == (0x1 << 8))
	      {
		/* Direct register */
		if (direct_register < 32)
		  {
		    ss_targets.target1_pc = gdb_proc.current_thread_context.dn[direct_register];
		  }
		else if (direct_register < 40)
		  {
		    ss_targets.target1_pc = gdb_proc.current_thread_context.an[direct_register-32];
		  }
		else
		  {
		    printf_unfiltered("Unknown direct register %d\n", direct_register);
		    return -1;
		  }
	      } 
	    else if ((s1 & 0x700) == 0)
	      {
		/* immediate mode. */
		printf_unfiltered("Immediate mode is wrong.\n");
		return -1;
	      }
	    else
	      {
		if (s1 & 0x400)
		  {
		    /* Indirect with offset. */
		    ss_targets.target1_pc = indirect_offset;
		    ss_targets.target1_pc += gdb_proc.current_thread_context.an[address_register];
		  }
		else if ((s1 & 0x700) == (0x3 << 8))
		  {
		    /* Indirect with index. */
		    ss_targets.target1_pc = gdb_proc.current_thread_context.dn[data_register] << 2;
		    ss_targets.target1_pc += gdb_proc.current_thread_context.an[address_register];		
		  }
		else if ((s1 & 0x700) == (0x2 << 8))
		  {
		    /* Indirect with pre/post increment .*/
		    ss_targets.target1_pc = gdb_proc.current_thread_context.an[address_register];
		
		    if (pre)
		      {
			ss_targets.target1_pc += four_bit_immediate;
		      }
		  }

		/* read the memory we have computed above. It is the return address. */
		ret = ubicom32_posix_read_bytes(ss_targets.target1_pc, cptr, 4);
		if (ret != 4)
		  return -1;
		
		ss_targets.target1_pc = cptr[0] <<24 | cptr[1] << 16 | cptr[2] << 8 | cptr[3];
	      }
	  }
	else
	  {
	    ss_targets.target1_pc = pc+4;
	    ss_targets.target1_active = 1;
	  }
	break;
      }
      default:
	{
	  ss_targets.target1_pc = pc+4;
	  ss_targets.target1_active = 1;
	}
    }

  /* Test and check if both the break points are within range. */
  if (ss_targets.target1_active )
    {
      if ( ss_targets.target1_pc < text_start || ss_targets.target1_pc >= (text_start + text_size ))
	{
	  /* Invalid breakpoing address. */
	  ss_targets.target1_active = ss_targets.target2_active = 0;
	  return -2;
	}
    }

  if (ss_targets.target2_active )
    {
      if ( ss_targets.target2_pc < text_start || ss_targets.target2_pc >= (text_start + text_size ))
	{
	  /* Invalid breakpoing address. */
	  ss_targets.target1_active = ss_targets.target2_active = 0;
	  return -2;
	}
    }

  /* At this point we know how many targets to set up. Go ahead and deal with target 1. */
  /* Check if the target1 ps is in the inserted bp list. */
  for (i=0; i< inserted_bps.current_entry; i++)
    {
      if (inserted_bps.bp_addresses[i] == ss_targets.target1_pc)
	break;
    }
  if (i != inserted_bps.current_entry)
    {
      /* This entry already exists. We can ignore it. */
      ss_targets.target1_active = 0;
    }
  else
    {
      /* Read back the code at this location. */
      ret = ubicom32_posix_read_bytes(ss_targets.target1_pc, ss_targets.t1_insn, 4);
      if (ret != 4)
	return -1;

      /* Write the break point. */
      ret = ubicom32_posix_write_bytes(ss_targets.target1_pc, bpToSend, 4);
      if (ret != 4)
	return -1;
    }

  if (ss_targets.target2_active)
    {
      for (i=0; i< inserted_bps.current_entry; i++)
	{
	  if (inserted_bps.bp_addresses[i] == ss_targets.target2_pc)
	    break;
	}
      if (i != inserted_bps.current_entry)
	{
	  /* This entry already exists. We can ignore it. */
	  ss_targets.target2_active = 0;
	}
      else
	{
	  /* Read back the code at this location. */
	  ret = ubicom32_posix_read_bytes(ss_targets.target2_pc, ss_targets.t2_insn, 4);
	  if (ret != 4)
	    return -1;
	  
	  /* Write the break point. */
	  ret = ubicom32_posix_write_bytes(ss_targets.target2_pc, bpToSend, 4);
	  if (ret != 4)
	    return -1;
	}
    }

  return 0;
}

static int ubicom32_posix_cleanup_single_step(void)
{
  char previous_contents[4];
  char expected_contents[] = UBICOM32_V4_SOFTWARE_BREAKPOINT;
  int ret;

  if (ss_targets.target1_active)
    {
      /* read the code and make sure it has the expected data in it. */
      ret = ubicom32_posix_read_bytes(ss_targets.target1_pc, previous_contents, 4);
      if (ret != 4)
	return -1;

      for(ret =0; ret< 4; ret++)
	if(expected_contents[ret] != previous_contents[ret])
	  break;

      /* write the original code back. */
      ret = ubicom32_posix_write_bytes(ss_targets.target1_pc, ss_targets.t1_insn, 4);
      if (ret != 4)
	return -1;
    }

  if (ss_targets.target2_active)
    {
      /* read the code and make sure it has the expected data in it. */
      ret = ubicom32_posix_read_bytes(ss_targets.target2_pc, previous_contents, 4);
      if (ret != 4)
	return -1;

      for(ret =0; ret< 4; ret++)
	if(expected_contents[ret] != previous_contents[ret])
	  break;

      /* write the original code back. */
      ret = ubicom32_posix_write_bytes(ss_targets.target2_pc, ss_targets.t2_insn, 4);
      if (ret != 4)
	return -1;
    }
  return 0;
}
static void
ubicom32_posix_resume (ptid_t ptid, int step, enum target_signal signal)
{
  if (step)
    {
      /* Single step case. */
      int ret = ubicom32_posix_setup_single_step(); 
      if (ret == -1)
	{
	  error("Cannot setup single stepping.");
	}

      if (ret == -2) 
	{
	  /* Single stepping was trying to set a break point to an outof bounds location. Just restart it. */
	  int ret = ubicom32_remote_posix_restart(signal);
	  remote_posix_state = POSIX_RUNNING;
	  inserted_bps.current_entry = 0;
	  return;
	}

      /* Normal case. */
      ret = ubicom32_remote_posix_restart_thread(signal, &gdb_proc);
      remote_posix_state = POSIX_STEPPING;

      if (ret)
	remote_posix_state = POSIX_STEPPING_PROBLEMS;

      inserted_bps.current_entry = 0;
    }
  else
    {
      /* Let everything run. */
      int ret = ubicom32_remote_posix_restart(signal);
      remote_posix_state = POSIX_RUNNING;
      inserted_bps.current_entry = 0;
    }
}

/* ubicom32_posix_cntrl_c.  */
static void
ubicom32_posix_cntrl_c (int signo)
{
  ubicom32_posix_cntrl_c_abort = 1;
}

static ptid_t
ubicom32_posix_wait (ptid_t ptid, struct target_waitstatus *status)
{
  static RETSIGTYPE (*prev_sigint) ();
  int ret, reason, i;
  ptid_t my_ptid;

  if (remote_debug)
    printf_filtered ("ubicom32_posix_wait\n");

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  if(remote_posix_state == POSIX_STOPPED || remote_posix_state == POSIX_STEPPING_PROBLEMS )
    {
      /* Things are already stopped */
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_TRAP;
      return (ptid_build((int)gdb_proc.proc, 0, (long)gdb_proc.current_thread));
    }

  ubicom32_posix_cntrl_c_abort = 0;
  prev_sigint = signal (SIGINT, ubicom32_posix_cntrl_c);

  while (1)
    {
      if (ubicom32_posix_cntrl_c_abort)
	{
	  ret = ubicom32_remote_posix_stop();
	  if(ret)
	    {
	      printf_unfiltered("error: Could not stop the processor\n");
	      signal (SIGINT, prev_sigint);
	      return null_ptid;
	    }
	  reason = 1;
	  break;
	}
      else
	{
	  ret = dongle_poll();
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

	  /* Remote has sent a debug event. Collect it. */
	  ret = ubicom32_remote_posix_get_gdb_event(&gdb_proc);
	  if(ret< 0)
	    {
	      printf_unfiltered ("error: unable to read status\n");
	      signal (SIGINT, prev_sigint);
	      return null_ptid;
	    }
	  reason = 2;
	  break;
	}
    }

  /* Go get the processor status. */
  ret = ubicom32_remote_posix_get_status(&gdb_proc);

  /* Add threads. */
  for (i=0; i< gdb_proc.num_pthreads; i++)
    {
      ptid_t new_ptid = ptid_build((int)gdb_proc.proc, 0, (long)gdb_proc.pthreads[i]);
      if (!in_thread_list(new_ptid))
	{
	  add_thread(new_ptid);
	}
    }

  status->kind = TARGET_WAITKIND_STOPPED;
  if (reason == 2)
    {
      /* Halted because of a GDB/TERMINATE event. */
      gdb_proc.current_thread = gdb_proc.gdb_event_thread;
      status->value.sig = gdb_proc.target_signal;

      if (gdb_proc.target_state == 0)
	{
	  /* Remote end is going to exit. */
	  if (gdb_proc.target_signal)
	    status->kind = TARGET_WAITKIND_SIGNALLED;
	  else
	    {
	      status->kind = TARGET_WAITKIND_EXITED;
	      status->value.integer = gdb_proc.gdb_event_thread;
	    }
	}
    }
  else
    {
      /* Halted because of user ^C. */
      gdb_proc.current_thread = gdb_proc.pthreads[0];
      status->value.sig = TARGET_SIGNAL_INT;
    }

  if (remote_posix_state == POSIX_STEPPING)
    {
      ret = ubicom32_posix_cleanup_single_step();
    }

  remote_posix_state = POSIX_STOPPED;

  signal (SIGINT, prev_sigint);
  ptid = ptid_build((int)gdb_proc.proc, 0, (long)gdb_proc.current_thread);
  return ptid;
}

static void
ubicom32_posix_fetch_register (struct regcache *regcache, int regno)
{
  int ret, i;
  int *rptr = (int *)&gdb_proc.current_thread_context.dn[0];

  gdb_proc.current_thread = (unsigned int)ptid_get_tid(inferior_ptid);
  if (gdb_proc.current_thread == 0)
    gdb_proc.current_thread = gdb_proc.pthreads[0];

  /* Zero out the context structure. */
  memset(&gdb_proc.current_thread_context, 0, sizeof(struct posix_context));
  ret = ubicom32_remote_fetch_registers(&gdb_proc);
  if (ret)
    {
      if (ret == -2)
	error("Backend did not find thread 0x%x", gdb_proc.current_thread);
      else if (ret ==-3)
	error("Unexpected reply frame type.");
    }
  
  /* Now supply the registers. */
  for (i=0; i< (sizeof(struct posix_context)/4); i++)
    posix_swizzled_supply_register(regcache, i, rptr++);
}

static void
ubicom32_posix_store_register(struct regcache *regcache, int regnum)
{
}

static void
ubicom32_posix_prepare_to_store (struct regcache *regcache)
{
  int i;
  gdb_byte buf[MAX_REGISTER_SIZE];

  /* Make sure all the necessary registers are cached.  */
  for (i = 0; i < gdbarch_num_regs(current_gdbarch); i++)
    regcache_raw_read (regcache, i, buf);
}

static int
ubicom32_posix_xfer_memory (CORE_ADDR mem_addr, gdb_byte *buffer, int mem_len,
		    int should_write, struct mem_attrib *attrib,
		    struct target_ops *target)
{
  CORE_ADDR end_addr = mem_addr + mem_len;
  int targ_len = mem_len;
  int res;

  if (mem_len <=0)
    return 0;

  /* Do a bounds check. */
  if (mem_addr > UBICOM32_DDR_END)
    return 0;

  if (mem_addr < UBICOM32_DDR_ADDR)
    return (mem_addr - UBICOM32_DDR_ADDR);

  if (end_addr > UBICOM32_DDR_END+4)
    mem_len = UBICOM32_DDR_END+4 - mem_addr;

  /* now we know how much to transfer */
  if (should_write)
    {
      res = ubicom32_posix_write_bytes (mem_addr, buffer, mem_len);
    }
  else
    res = ubicom32_posix_read_bytes (mem_addr, buffer, mem_len);

  return res;
}

static void
ubicom32_posix_files_info (struct target_ops *target)
{
  if (exec_bfd)
    {
      char *file = bfd_get_filename (exec_bfd);
      printf_unfiltered ("Debugging %s\n", file);
    }
  else
    printf_unfiltered ("No file loaded\n");
}

static void
ubicom32_posix_kill (void)
{
  if (remote_debug)
      printf_filtered ("ubicom32_posix_kill\n");

  inferior_ptid = null_ptid;

  /* Detach from target.  */
  ubicom32_posix_detach (NULL, 0);
}

static void
ubicom32_posix_load (char *file, int from_tty)
{
}

static int
ubicom32_posix_thread_alive (ptid_t ptid)
{
  unsigned int tid = (unsigned int)ptid_get_tid(ptid);
  unsigned int pid = (unsigned int)ptid_get_pid(ptid);
  unsigned int i;

  if (gdb_proc.proc != pid)
    return 0;

  /* Hunt through gdb_proc.pthreads and see if there is a match. */
  for (i=0; i<gdb_proc.num_pthreads; i++)
    if (gdb_proc.pthreads[i] == tid)
      break;

  if (i == gdb_proc.num_pthreads)
    return 0;

  /* Thread is alive. */
  return (1);
}

static char *
ubicom32_posix_thread_pid_to_str (ptid_t ptid)
{
  static char buf[30];
  unsigned int tid = (unsigned int)ptid_get_tid(ptid);

  sprintf(buf, "<Th_id 0x%08x>", tid);
  return buf;
}

static void
ubicom32_posix_stop (void)
{
  if (remote_debug)
    printf_filtered ("ubicom32_posix_stop\n");

  ubicom32_posix_cntrl_c_abort = 1;
}

static char *
ubicom32_posix_threads_extra_info (struct thread_info *tp)
{
  return NULL;
}

void 
ubicom32_posix_reset(char *args, int from_tty)
{
  int ret, i;
  if (ubicom32_remote_posix_reset(&gdb_proc))
    return;

  /* Attach to the target process. */
  ret = ubicom32_posix_attach();

  gdb_proc.current_thread = gdb_proc.pthreads[0];

  /*
   * The remote process will be in a stopped state.
   */
  remote_posix_state = POSIX_STOPPED;

  inferior_ptid = ptid_build((int)gdb_proc.proc, 0, (long)gdb_proc.pthreads[0]);
  start_remote(from_tty);

  /* Add threads. */
  for (i=0; i< gdb_proc.num_pthreads; i++)
    {
      ptid_t new_ptid = ptid_build((int)gdb_proc.proc, 0, (long)gdb_proc.pthreads[i]);
      if (!in_thread_list(new_ptid))
	{
	  add_thread(new_ptid);
	}
    }
}

/* init_posix_ops.  */
static void
init_ubicom32_posix_ops (void)
{
  posix_ops.to_shortname = "ubicom32-posix";
  posix_ops.to_longname = "Remote Ubicom32 POSIX debug Via Ubicom Ethernet";
  posix_ops.to_doc = "Remote Ubicom32 POSIX debug Via Ethernet.\n\
Connect to target board as follows:\n\
target ubicom32-posix ip-address:5010\n\
";
  posix_ops.to_open = ubicom32_posix_open;
  posix_ops.to_close = ubicom32_posix_close;
  posix_ops.to_attach = NULL;
  posix_ops.to_post_attach = NULL;
  posix_ops.to_detach = ubicom32_posix_detach;
  posix_ops.to_resume = ubicom32_posix_resume;
  posix_ops.to_wait = ubicom32_posix_wait;
  //posix_ops.to_post_wait = NULL;
  posix_ops.to_fetch_registers = ubicom32_posix_fetch_register;
  posix_ops.to_store_registers = ubicom32_posix_store_register;
  posix_ops.to_prepare_to_store = ubicom32_posix_prepare_to_store;
  posix_ops.deprecated_xfer_memory = ubicom32_posix_xfer_memory;
  posix_ops.to_files_info = ubicom32_posix_files_info;
  posix_ops.to_kill = ubicom32_posix_kill;
  posix_ops.to_load = ubicom32_posix_load;
  posix_ops.to_insert_breakpoint = ubicom32_posix_insert_breakpoint;
  posix_ops.to_remove_breakpoint = ubicom32_posix_remove_breakpoint;

  posix_ops.to_terminal_init = NULL;
  posix_ops.to_terminal_inferior = NULL;
  posix_ops.to_terminal_ours_for_output = NULL;
  posix_ops.to_terminal_ours = NULL;
  posix_ops.to_terminal_info = NULL;
  posix_ops.to_lookup_symbol = NULL;
  posix_ops.to_create_inferior = NULL;
  posix_ops.to_post_startup_inferior = NULL;
  posix_ops.to_acknowledge_created_inferior = NULL;
  posix_ops.to_insert_fork_catchpoint = NULL;
  posix_ops.to_remove_fork_catchpoint = NULL;
  posix_ops.to_insert_vfork_catchpoint = NULL;
  posix_ops.to_remove_vfork_catchpoint = NULL;
  posix_ops.to_insert_exec_catchpoint = NULL;
  posix_ops.to_remove_exec_catchpoint = NULL;
  posix_ops.to_reported_exec_events_per_exec_call = NULL;
  posix_ops.to_has_exited = NULL;
  posix_ops.to_mourn_inferior = ubicom32_posix_kill;
  posix_ops.to_can_run = 0;
  posix_ops.to_notice_signals = 0;
  posix_ops.to_thread_alive = ubicom32_posix_thread_alive;
  posix_ops.to_pid_to_str = ubicom32_posix_thread_pid_to_str;
  posix_ops.to_stop = ubicom32_posix_stop;
  posix_ops.to_pid_to_exec_file = NULL;
  posix_ops.to_stratum = process_stratum;
  posix_ops.to_has_all_memory = 1;
  posix_ops.to_has_memory = 1;
  posix_ops.to_has_stack = 1;
  posix_ops.to_has_registers = 1;
  posix_ops.to_has_execution = 1;
  posix_ops.to_sections = NULL;
  posix_ops.to_sections_end = NULL;
  posix_ops.to_magic = OPS_MAGIC;
  posix_ops.to_extra_thread_info = ubicom32_posix_threads_extra_info;
}

void
_initialize_ubicom32_posix(void)
{
  init_ubicom32_posix_ops();
  add_target (&posix_ops);
  memset (&inserted_bps, 0, sizeof(inserted_bps));
}
