/* Target-dependent code for GDB, the GNU debugger.

   Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 
   2009
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#if defined(_WIN32)
#include <winsock.h>
#define usleep(x) Sleep(x/1000)
#else
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <string.h>
#include "ubicom32_dongle.h"
#include "remote-ubicom32-posix-backend.h"

#define MAX_PACKET_DATA 1280
struct posix_gdb_packet {
  unsigned int data[MAX_PACKET_DATA/4];
};

enum posix_gdb_commands {
	POSIX_GDB_READ_REGISTER,
	POSIX_GDB_WRITE_REGISTER,
	POSIX_GDB_READ_MEMORY,
	POSIX_GDB_WRITE_MEMORY,
	POSIX_GDB_POSIX_STATUS,
	POSIX_GDB_PROCESS_STATUS,
	POSIX_GDB_ATTACH,
	POSIX_GDB_DETACH,
	POSIX_GDB_RESTART,
	POSIX_GDB_RESTART_A_THREAD,
	POSIX_GDB_STOP,
	POSIX_GDB_EVENT,
	POSIX_GDB_TERMINATE,
	POSIX_GDB_RESET,
};

#define POSIX_GDB_HEADER_SIZE 8
#define POSIX_GDB_MAGIC 0xaabbccdd

static void
posix_packet_init(struct posix_gdb_packet *packet, unsigned int gdb_command)
{
  /*
   * Initialize the first location to Magic, set length to 12 bytes, and location after that to command.
   */
  packet->data[0] = POSIX_GDB_MAGIC;
  packet->data[1] = 12;
  packet->data[2] = gdb_command;
}

static unsigned int
posix_packet_append(struct posix_gdb_packet *packet, unsigned int pkt_data)
{
  packet->data[packet->data[1]/4] = pkt_data;
  packet->data[1] += 4;
  return packet->data[1];
}

static int
posix_packet_send(struct posix_gdb_packet *packet)
{
  /* First convert all the data to network byte order. */
  unsigned int count = packet->data[1]/4;
  unsigned int i, *src, *dst;
  char *ptr = (char *)packet;

  src = dst = packet->data;
  for(i=0; i< count; i++)
    *dst++ = htonl(*src++);

  if (dongle_write((char *) packet, count*4) != 0)
    {
      dongle_close ();
      dongle_error ("error: send communication error!");
    }

  /*
   * We are going to get at least 12 bytes (MAGIC, length and gdb command).
   */
  if(dongle_read(ptr, 12))
    {
      dongle_close ();
      dongle_error ("error: recv communication error!");
    }

  /*
   * Convert the 12 bytes we got back to host order. We will look at the length
   * and read more bytes if needed.
   */
  src = dst = packet->data;
  for(i=0; i< 3; i++)
    *dst++ = ntohl(*src++);

  /*
   * Make sure the magic we go back is sane.
   */
  if (packet->data[0] != POSIX_GDB_MAGIC)
    {
      dongle_error("Incorrect magic.");
      return -1;
    }

  if(packet->data[1] > 12)
    {
      /*
       * There is more data to be read in.
       */
      ptr += 12;
      if(dongle_read(ptr, packet->data[1]-12))
	{
	  dongle_close ();
	  dongle_error ("error: recv communication error!");
	}

      /*
       * Convert the data back to host byte order.
       */
      src = dst = (unsigned int *)ptr;
      count = (packet->data[1]-12)/4;
      for(i=0; i< count; i++)
	*dst++ = ntohl(*src++);
    }

  return 0;
}

int 
ubicom32_remote_posix_detach(void)
{
  struct posix_gdb_packet packet;

  posix_packet_init(&packet, POSIX_GDB_DETACH);

  return (posix_packet_send(&packet));
}

/*
 * Connect to the remote via ethernet.
 */
int 
ubicom32_remote_posix_connect(char *remote)
{
  if(dongle_interface_status())
    {
      ubicom32_remote_posix_detach();
      dongle_close();
    }

  return (dongle_open(remote));
}

/*
 * Shutdown the connection to remote ethernet.
 */
int 
ubicom32_remote_posix_close(void)
{
  dongle_close();
  return 0;
}

int
ubicom32_remote_posix_get_status(struct ubicom32_posix_process *gdb_proc)
{
  int ret;
  struct posix_gdb_packet packet;

  /* Now send down POSIX_GDB_PROCESS_STATUS to get the pthread information. */
  posix_packet_init(&packet, POSIX_GDB_PROCESS_STATUS);
  /* Time to ship down the packet. */
  ret = posix_packet_send(&packet);
  if(ret)
    return ret;

  gdb_proc->num_pthreads = packet.data[3];
  
  /* copy the pthread info into gdb_proc structure. */
  memcpy(gdb_proc->pthreads, &packet.data[4], gdb_proc->num_pthreads*4);

  return 0;
}

/*
 * Attach to a specific posix process.
 */
int
ubicom32_remote_posix_attach(unsigned int text_start, unsigned int text_size, unsigned int data_start, unsigned int data_size, struct ubicom32_posix_process *gdb_proc)
{
  int ret;
  struct posix_gdb_packet packet;
  unsigned int count;

  /* Create a POSIX_GDB_ATTACH packet. */
  posix_packet_init(&packet, POSIX_GDB_ATTACH);
  count = posix_packet_append(&packet, text_start);
  count = posix_packet_append(&packet, text_size);
  count = posix_packet_append(&packet, data_start);
  count = posix_packet_append(&packet, data_size);

  /* Time to ship down the packet. */
  ret = posix_packet_send(&packet);
  if(ret)
    return ret;

  gdb_proc->proc = packet.data[4];
  return packet.data[3];
}

int
ubicom32_remote_fetch_registers(struct ubicom32_posix_process *gdb_proc)
{
  int ret;
  struct posix_gdb_packet packet;
  unsigned int count;

  /* Create a POSIX_GDB_READ_REGISTER packet. */
  posix_packet_init(&packet, POSIX_GDB_READ_REGISTER);

  /* Add the thread id to the packet. */
  count = posix_packet_append(&packet, gdb_proc->current_thread);

  /* Time to ship down the packet. */
  ret = posix_packet_send(&packet);
  if(ret)
    return ret;

  /* packet.data[3] returns the type of frame we got back. */
  if (packet.data[3] == 0)
    {
      /* Backend could not find the requested thread in its list. */
      return -2;
    }
  else if (packet.data[3] == 1)
    {
      /*
       * Regular preempted frame. Copy the reply block to the context structure.
       */
      memcpy(&gdb_proc->current_thread_context, &packet.data[4], sizeof (struct posix_context));
      return 0;
    }
  else if (packet.data[3] == 2)
    {
      /*
       * This is the syscall frame. We have to copy parts of this to the proper place.
       */
      struct posix_syscall_frame *sys_frame = (struct posix_syscall_frame *)&packet.data[4];
      struct posix_context *ctxt = &gdb_proc->current_thread_context;

      /* Copy the D registers. */
      memcpy(ctxt->dn, &sys_frame->reg_d0, 4*8);

      /* Copy a1 - a6 */
      memcpy(&ctxt->an[1], &sys_frame->reg_a1, 4*6);

      /* The stack pointer is sys_frame->frame_end */
      ctxt->an[7] = sys_frame->frame_end;

      /* PC is register a5 from the syscall frame. */
      ctxt->pc = sys_frame->reg_a5;

      return 0;
    }
  else
    {
      /* Unexpected reply. */
      return -3;
    }
}

int 
ubicom32_remote_posix_read_memory(unsigned int leadaddr, unsigned int toreadwords, int *recvbuffer)
{
  int ret;
  struct posix_gdb_packet packet;
  unsigned int count;

  while (toreadwords)
    {
      unsigned int transfer_size = (MAX_PACKET_DATA-16)/4;

      if (transfer_size > toreadwords)
	transfer_size = toreadwords;

      posix_packet_init(&packet, POSIX_GDB_READ_MEMORY);
      
      /* append the transfer_size */
      count = posix_packet_append(&packet, transfer_size);

      /* append the address of the transfer. */
      count = posix_packet_append(&packet, leadaddr);

      /* Ship it down. */
      ret = posix_packet_send(&packet);
      if(ret)
	return ret;

      /* Got the reply. Check status. */
      if (packet.data[3])
	return packet.data[3];

      /* Transfer data from the packet. */
      memcpy(recvbuffer, &packet.data[4], transfer_size * 4);

      /* Decrement and icrement various counters */
      recvbuffer += transfer_size * 4;
      toreadwords -= transfer_size;
      leadaddr += transfer_size * 4;
    }
  return 0;
}

int 
ubicom32_remote_posix_write_memory(unsigned int leadaddr, unsigned int towritewords, int *sendbuffer)
{
  int ret;
  struct posix_gdb_packet packet;
  unsigned int count;

  while (towritewords)
    {
      unsigned int transfer_size = (MAX_PACKET_DATA-16)/4;

      if (transfer_size > towritewords)
	transfer_size = towritewords;

      posix_packet_init(&packet, POSIX_GDB_WRITE_MEMORY);
      
      /* append the transfer_size */
      count = posix_packet_append(&packet, transfer_size);

      /* append the address of the transfer. */
      count = posix_packet_append(&packet, leadaddr);

      /* Copy the data into the packet. */
      memcpy(&packet.data[count/4], sendbuffer, transfer_size * 4);
      packet.data[1] += transfer_size * 4;

      /* Ship it down. */
      ret = posix_packet_send(&packet);
      if(ret)
	return ret;

      /* Got the reply. Check status. */
      if (packet.data[3])
	return packet.data[3];

      /* Decrement and icrement various counters */
      sendbuffer += transfer_size * 4;
      towritewords -= transfer_size;
      leadaddr += transfer_size * 4;
    }
  return 0;
}

int
ubicom32_remote_posix_restart(unsigned int signal)
{
  struct posix_gdb_packet packet;
  unsigned int count;

  posix_packet_init(&packet, POSIX_GDB_RESTART);
  count = posix_packet_append(&packet, signal);

  /* Ship it down. */
  return (posix_packet_send(&packet));
}

int
ubicom32_remote_posix_restart_thread(unsigned int signal, struct ubicom32_posix_process *gdb_proc)
{
  struct posix_gdb_packet packet;
  unsigned int count;

  posix_packet_init(&packet, POSIX_GDB_RESTART_A_THREAD);
  count = posix_packet_append(&packet, signal);
  count = posix_packet_append(&packet, gdb_proc->current_thread);

  /* Ship it down. */
  return (posix_packet_send(&packet));
}

int
ubicom32_remote_posix_stop(void)
{
  struct posix_gdb_packet packet;

  posix_packet_init(&packet, POSIX_GDB_STOP);

  /* Ship it down. */
  return (posix_packet_send(&packet));
}

int
ubicom32_remote_posix_get_gdb_event(struct ubicom32_posix_process *gdb_proc)
{
  struct posix_gdb_packet packet;
  char *ptr = (char *)&packet;
  unsigned int i, *src, *dst;
  unsigned int count;

  /*
   * We are going to get at least 12 bytes (MAGIC, length and gdb command).
   */
  if(dongle_read(ptr, 12))
    {
      dongle_close ();
      dongle_error ("error: recv communication error!");
    }

  /*
   * Convert the 12 bytes we got back to host order. We will look at the length
   * and read more bytes if needed.
   */
  src = dst = packet.data;
  for(i=0; i< 3; i++)
    *dst++ = ntohl(*src++);

  /*
   * Make sure the magic we got back is sane.
   */
  if (packet.data[0] != POSIX_GDB_MAGIC)
    {
      dongle_error("Incorrect magic.");
      return -1;
    }

  if(packet.data[1] > 12)
    {
      /*
       * There is more data to be read in.
       */
      ptr += 12;
      if(dongle_read(ptr, packet.data[1]-12))
	{
	  dongle_close ();
	  dongle_error ("error: recv communication error!");
	}

      /*
       * Convert the data back to host byte order.
       */
      src = dst = (unsigned int *)ptr;
      count = (packet.data[1]-12)/4;
      for(i=0; i< count; i++)
	*dst++ = ntohl(*src++);
    }
  
  /* This better be a GDB/TERMINATE event. */
  if ((packet.data[2] != POSIX_GDB_EVENT) && (packet.data[2] != POSIX_GDB_TERMINATE))
    return -1;

  /* Load the signal and task id into gdb_proc structure. */
  if (packet.data[2] == POSIX_GDB_EVENT)
    {
      /* target is alive. */
      gdb_proc->target_state = 1;
    }
  else
    {
      /* target has terminated the process. */
      gdb_proc->target_state = 0;
    }
  gdb_proc->target_signal = packet.data[3];
  gdb_proc->gdb_event_thread = packet.data[4];
  return 0;
}

int
ubicom32_remote_posix_reset(struct ubicom32_posix_process *gdb_proc)
{
  struct posix_gdb_packet packet;
  unsigned int count;

  posix_packet_init(&packet, POSIX_GDB_RESET);
  count = posix_packet_append(&packet, gdb_proc->current_thread);

  /* Ship it down. */
  return (posix_packet_send(&packet));
}
