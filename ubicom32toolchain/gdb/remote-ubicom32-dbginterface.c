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
#include "remote-ubicom32-isd.h"
#include "remote-ubicom32-dbginterface.h"
#include "ubicom32_dongle.h"

//#define DEBUG 1
#if defined(_WIN32)
unsigned int currentThread=0;
unsigned int debuggerThreadNo=0;
unsigned int dontDebugMask=0;	// Threads that we will not debug
unsigned int debugging_processor;
#else
extern unsigned int debuggerThreadNo;
extern unsigned int dontDebugMask;
extern unsigned int currentThread;
extern unsigned int debugging_processor;
#endif

int turnOnconsole(void);
int turnOffconsole(void);

int rdBufCnt = 0;

#define RDBUF_MAX 20
int rdBuffer[20];

#if 0
enum isd_command {
  ISD_NOP,
  ISD_OPEN,
  ISD_CLOSE,
  ISD_DEVICE_ID,
  ISD_DEVICE_VER,
  ISD_MAIL_STATUS,
  ISD_DEBUG_RST_REQ,
  ISD_WRITE_MAIL_BOX,
  ISD_READ_MAIL_BOX,
  ISD_CLEAN_MAIL,
  ISD_TEST_MODE
};
#endif

enum isd_command {
  ISD_NOP,
  ISD_OPEN,
  ISD_CLOSE,
  ISD_DEVICE_ID,
  ISD_DEVICE_VER,
  ISD_MAIL_STATUS,
  ISD_DEBUG_RST_REQ,
  ISD_WRITE_MAIL_BOX,
  ISD_READ_MAIL_BOX,
  ISD_CLEAN_MAIL,
  ISD_TEST_MODE,
  ISD_WR_MME,
  ISD_WR_MEC,
  ISD_WR_MEP,
  ISD_WR_MRC,
  ISD_WR_MOA,
  ISD_RD_MB,
  ISD_WR_MBRE = 0x10,
  ISD_WR_MEM,
  ISD_RD_MST,
  ISD_HALT_MP,
  ISD_REL_MP,
  ISD_FORCE_MP,
  ISD_RD_MP_REG,
  ISD_SET_MP_REG_ADDR,
  ISD_WR_MP_REG,
  ISD_RD_IPORT_STAT,
  ISD_WR_IBUF,
  ISD_WR_RST_HALT_MP_EN,
  ISD_RD_RST_HALT_MP,
};

unsigned int  rosr, pc, mt_active, mt_dbg_active, mt_en;
unsigned int mt_sched, mt_pri;

int ubicom32MainGdbIndexToRegAddrMap []={
  0x0,0x4,0x8,0xc,		// Data registers 0 - 15
  0x10,0x14,0x18,0x1c,
  0x20,0x24,0x28,0x2c,
  0x30,0x34,0x38,0x3c,
  0x80,0x84,0x88,0x8c,		// Address registers 0 - 7
  0x90,0x94,0x98,0x9c,
  0xa0,0xa4,0xa8,0xac,		// acc0_hi - source3
  0xb0,0xb4,0xb8,0xbc,		// Inst_cnt - Iread_data
  0xc0,0xc4,			// int_mask0 int_mask1
  0xd0,				// Thread PC
				// Global registers
  0x100, 0x104, 0x108,		// 35 Chip_id - int_stat1
  0x114, 0x118,			// int_set0 inst_set1
  0x124, 0x128,			// int_clr0 inst_clr1
  0x134,			// 42  Global Control
  0x13c, 0x140,		// Mt_active_set, mt_active clr
  0x138,			// 45  mt_active
  0x148,			// mt_dbg_active_set
  0x144,			// 47 mt_dbg_active
  0x14c, 0x150, 0x154,		// 48 mt_en, mt_hpri, mt_hrt
  0x15c,			// mt_break_clr
  0x158,			// 52 mt_break
  0x160, 0x164,			// 53 mt_single_step, mt_min_delay_en
  0x17c,			// mt_dbg_active_clr
  0x16c,			// 56 perr_addr
  0x178,			// 57 dcapt_tnum
  0x174,			// 58 dcapt_pc
  0x170,			// 59 dcapt
  0x180, 0x184, 0x188, 0x18c	// 60 scratchpad0 - scratchpad3
};


// Snaphot of the registers
mercuryMainRegs_t mainRegisters;

//extern int ubicom32ispisd_packet_send (struct ubicom32ispisd_packet *packet);
int ubicom32isp_detach(void);

char
tohex( int x )
{
  return ( x > 9 ? x+'a'-10 : x+'0' );
}

unsigned int okToDoubleBuffer = 0;
unsigned int dongleHasMarsSupport=0;
unsigned int dongleIsUbipod = 0;
unsigned int ubipodControl2wire = 0;
unsigned int ubipodControlconsole = 0;
unsigned int errorExpected = 0;
unsigned int doubleBuffer=0;
struct ubicom32command_packet *pending_packet= NULL;
unsigned int pending_packet_length;
unsigned int pending_packet_opcode;

unsigned char dkmajor, dkminro, dkstate, dkincrement;
unsigned int dkunique;
unsigned int single_step_buffer_address;

int
compareVersion(unsigned char maj, unsigned char min, unsigned char st,
	       unsigned char inc, unsigned int unk)
{
  if(maj != dkmajor)
    return 1;

  if(min != dkminro)
    return 1;

  if(st != dkstate)
    return 1;

  if(inc != dkincrement)
    return 1;

  if(unk != dkunique)
    return 1;

  return 0;
}

void
retrieve_pending_packet(void)
{
  unsigned int count;
  unsigned char *ptr;
  struct ubicom32ispisd_packet *pending_packet_header = &pending_packet->header;

  count = 0;
  ptr = (char *) pending_packet;

  if(dongle_read(ptr, pending_packet_length))
    {
      dongle_close ();
      dongle_error ("error: Pending recv communication error!");
    }

  pending_packet_header->magic = ntohl (pending_packet_header->magic);
  pending_packet_header->length = ntohl (pending_packet_header->length);
  pending_packet_header->opcode = ntohl (pending_packet_header->opcode);
  pending_packet_header->operand = ntohl (pending_packet_header->operand);

  if ((pending_packet_header->magic != ISPISD_PACKET_MAGIC) || (pending_packet_header->length != pending_packet_length)
      || (pending_packet_header->opcode != pending_packet_opcode))
    {
      dongle_close ();
      dongle_error ("error: Pending invalid communication response!\n");
    }
}

/* ubicom32ispisd_packet_send.  */
int
ubicom32ispisd_packet_send (struct ubicom32ispisd_packet *packet)
{
  unsigned int length, count, opcode, operand;
  unsigned char *ptr;

  if(dongle_interface_status() == 0)
    {
      printf("No connection to target. Request denied.\n");
      return -1;
    }

  length = packet->length;
  opcode = packet->opcode;
  operand = packet->operand;

  if ((packet->magic != ISPISD_PACKET_MAGIC)
      || (length < sizeof (struct ubicom32ispisd_packet)))
    {
      printf ("error: invalid packet!\n");
      return -1;
    }

  packet->magic = htonl (packet->magic);
  packet->length = htonl (packet->length);
  packet->opcode = htonl (packet->opcode);
  packet->operand = htonl (packet->operand);

  if (dongle_write((char *) packet, length) != 0)
    {
      dongle_close ();
      dongle_error ("error: send communication error!");
    }

  if(doubleBuffer==0)
    {
      if(opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK||
	 opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK_MARS||
	 opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK_UBIPOD||
	 opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_2WIRE||
	 opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_2WIRE||
	 opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_CONSOLE||
	 opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_CONSOLE||
	 opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_UNCLOG_MAILBOX||
	 opcode == ISPISD_OPCODE_UBICOM32COMMAND_WRITE_MAILBOX)
	{
	  /* we are expecting back only 16 bytes */
	  length = 16;
	}
      else if(opcode == ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX)
	{
	  /* expecting back header size + operand * 4 */
	  length = 16 + operand * 4;
	}

      count = 0;
      ptr = (char *) packet;

      if(dongle_read(ptr, length))
	{
	  if(errorExpected)
	    return -1;

	  dongle_close ();
	  dongle_error ("error: recv communication error!");
	}

      packet->magic = ntohl (packet->magic);
      packet->length = ntohl (packet->length);
      packet->opcode = ntohl (packet->opcode);
      packet->operand = ntohl (packet->operand);

      if ((packet->magic != ISPISD_PACKET_MAGIC) || (packet->length != length)
	  || (packet->opcode != opcode))
	{
	  dongle_close ();
	  dongle_error ("error: invalid communication response!\n");
	}
    }
  else
    {
      if(pending_packet == NULL)
	return 0;

      retrieve_pending_packet();
    }
  return 0;
}

/* ubicom32command_packet_init.  */
void
ubicom32command_packet_init (struct ubicom32command_packet *packet)
{
  packet->header.magic = ISPISD_PACKET_MAGIC;
  packet->header.length = 0;
  packet->header.opcode = ISPISD_OPCODE_UBICOM32COMMAND;
  packet->header.operand = 0;
}

/* ubicom32command_packet_init.  */
void
ubicom32command_packet_init_opcode (struct ubicom32command_packet *packet, unsigned int opcode)
{
  packet->header.magic = ISPISD_PACKET_MAGIC;
  packet->header.length = 0;
  packet->header.opcode = opcode;
  packet->header.operand = 0;
}

/* ubicom32command_packet_send.  */
int
ubicom32command_packet_send (struct ubicom32command_packet *packet)
{
  unsigned int count;
  int ret;

  count = packet->header.operand;
  if (count > UBICOM32COMMAND_MAX_COMMANDS_PER_PACKET &&
      packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND)
    {
      printf ("error: invalid packet!\n");
      return -1;
    }
  if(packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND||
     packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK||
     packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK_UBIPOD||
     packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_2WIRE||
     packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_2WIRE||
     packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_CONSOLE||
     packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_CONSOLE||
     packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_UNCLOG_MAILBOX||
    packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK_MARS)
    {
      packet->header.length =
	sizeof (struct ubicom32ispisd_packet) +
	sizeof (struct ubicom32command) * packet->header.operand;
    }

  ret = ubicom32ispisd_packet_send ((struct ubicom32ispisd_packet *) packet);
  if (ret < 0)
    return -1;

  if(doubleBuffer == 0)
    {
      if(packet->header.opcode == ISPISD_OPCODE_UBICOM32COMMAND)
	{
	  if (packet->header.operand != count)
	    {
#ifdef DEBUG
	      printf ("execute failure: %ld/%ld\n", packet->header.operand,
		      count);
#endif
	      return -1;
	    }
	}
    }
  return ret;
}

/* ubicom32command_packet_append.  */
int
ubicom32command_packet_append (struct ubicom32command_packet *packet,
			   unsigned char options, unsigned char opcode,
			   unsigned int operand)
{
  struct ubicom32command *command;
  int count;

  count = (int) packet->header.operand++;
  command = &packet->commands[count];

  command->options = options;
  command->opcode = opcode;
  command->operand0 = (unsigned char)((operand >> 24) &0xff);
  command->operand1 = (unsigned char)((operand >> 16) &0xff);
  command->operand2 = (unsigned char)((operand >> 8) &0xff);
  command->operand3 = (unsigned char)(operand & 0xff);

  return count;
}

/* ubicom32command_packet_get_next.  */
int
ubicom32command_packet_get_next (struct ubicom32command_packet *packet)
{
  return (int) packet->header.operand;
}

/* ubicom32command_packet_get_free.  */
int
ubicom32command_packet_get_free (struct ubicom32command_packet *packet)
{
  return UBICOM32COMMAND_MAX_COMMANDS_PER_PACKET - (int) packet->header.operand;
}


int deviceId;
int deviceVersion;
int chipID;
int
getSingleStepBufferAddress(void)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  ubicom32command_packet_init(&packet);

  // Issue the command to get debugger thread #
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)GET_SGL_STP_BUF);

  // the command is echoed back. Go get it
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  // get the buffer address
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[1];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != GET_SGL_STP_BUF_DONE)
    exit(1);

  command++;

  single_step_buffer_address = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  return 0;
}


int
debugOpen(void)
{
  int ret, count;
  struct ubicom32command *command;
  struct ubicom32command_packet packet;

  ubicom32command_packet_init(&packet);

  // Send down ISD_OPEN
  count = ubicom32command_packet_append(&packet, 0, ISD_OPEN, 0);

  // Send down ISD_DEVICE_ID
  count = ubicom32command_packet_append(&packet, 0, ISD_DEVICE_ID, 0);

  // Send down ISD_DEVICE_VER
  count = ubicom32command_packet_append(&packet, 0, ISD_DEVICE_VER, 0);

  // Send down ISD_RD_MP_REG chip id register
  count = ubicom32command_packet_append(&packet, 0, ISD_RD_MP_REG, 0x100);

  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[1];

  // extract deviceId
  deviceId = (command->operand0<<24) | (command->operand1<<16) | (command->operand2<<8) | command->operand3;

  // extract deviceVersion
  command++;
  deviceVersion = (command->operand0<<24) | (command->operand1<<16) | (command->operand2<<8) | command->operand3;

  // extract chipID
  command++;
  chipID = (command->operand0<<24) | (command->operand1<<16) | (command->operand2<<8) | command->operand3;

  //printf("devID =0x%x version =0x%x chipID =0x%x\n", deviceId, deviceVersion, chipID);
  if(deviceId == 0 && deviceVersion == 0 && (chipID >= 0x20000))
    debugging_processor = 5133;
  else
    debugging_processor = 3023;

  return 0;
}

int
debugReset(void)
{
  int ret, count;

  struct ubicom32command_packet packet;
  ubicom32command_packet_init(&packet);

  // Send down ISD_DEBUG_RST_REQ
  count = ubicom32command_packet_append(&packet, OPTION_IGNORE_RESPONSE|OPTION_EXTENDED_TIMEOUT, ISD_DEBUG_RST_REQ, 0);

  ret = ubicom32command_packet_send(&packet);

  return ret;
}

int
debugClose(void)
{
  int ret, count;

  struct ubicom32command_packet packet;
  ubicom32command_packet_init(&packet);

  // Send down ISD_OPEN
  count = ubicom32command_packet_append(&packet, 0, ISD_CLOSE, 0);

  ret = ubicom32command_packet_send(&packet);

  return ret;
}

int
readStatus(int *status)
{
  int ret, count;

  unsigned int stat;
  struct ubicom32command *command;
  struct ubicom32command_packet packet;
  ubicom32command_packet_init(&packet);

  // Send down ISD_MAIL_STATUS
  count = ubicom32command_packet_append(&packet, 0, ISD_MAIL_STATUS, 0);

  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[0];

  // extract status
  stat = (command->operand0<<24) | (command->operand1<<16) | (command->operand2<<8) | command->operand3;
  *status = (int)stat;
  return ret;
}

int
readMailbox(int *mboxData)
{
  int ret, count;

  unsigned int stat;
  struct ubicom32command *command;
  struct ubicom32command_packet packet;
  ubicom32command_packet_init(&packet);

  // Send down ISD_READ_MAIL_BOX
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[0];

  // extract status
  stat = (command->operand0<<24) | (command->operand1<<16) | (command->operand2<<8) | command->operand3;
  *mboxData = (int)stat;
  return ret;
}

int
readRegisters( int threadNo,
	       int regIndex,
	       int *dest,
	       unsigned int length)
{
  int ret, count;
  int commandEchoIndex;

  struct ubicom32command_packet packet;
  ubicom32command_packet_init(&packet);

  // send down the READ_REGISTERS command
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, READ_REGISTERS);

  // send down the thread #
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)threadNo);

  // send down the start address of the first register to read back
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX,
				    ubicom32MainGdbIndexToRegAddrMap[regIndex]);

  // Send a length of transfer
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)length);

  // The entry at this index is going to contain READ_REGISTERS_DONE
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  commandEchoIndex = count;

  while(length)
    {
      unsigned int transferLength = ubicom32command_packet_get_free(&packet);
      unsigned int i;
      int transferStartIndex;
      struct ubicom32command *command;
      unsigned int reply;

      if(transferLength > length)
	transferLength = length;

      // Index of the first transfer entry
      transferStartIndex = ubicom32command_packet_get_next(&packet);
      command = &packet.commands[transferStartIndex];

      for(i=0; i< transferLength; i++)
	count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      // Ship the packet for processing
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	return ret;

      // Process the return data
      if(commandEchoIndex)
	{
	  // Test the reply for READ_REGISTERS_DONE
	  command = &packet.commands[commandEchoIndex];
	  reply = (command->operand0<<24) | (command->operand1<<16) |
	    (command->operand2<<8) | command->operand3;

	  if(reply != READ_REGISTERS_DONE)
	    {
	      printf("Reply 0x%x does not match READ_REGISTERS_DONE\n", reply);
	      return -1;
	    }
	  else
	    commandEchoIndex = 0;
	}

      // Move the reply data into destination buffer
      command = &packet.commands[transferStartIndex];
      for(i=0; i< transferLength; i++, command++)
	{
	  reply = (command->operand0<<24) | (command->operand1<<16) |
	    (command->operand2<<8) | command->operand3;

	  *dest++ = (int)reply;
	}

      // readjust length etc.
      ubicom32command_packet_init(&packet);
      length = length - transferLength;
    }

  return 0;
}


int
writeRegisters( int threadNo,
		int regIndex,
		int *inBuf,
		unsigned int length)
{
  int ret, count;
  unsigned int i;
  int commandEchoIndex;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  ubicom32command_packet_init(&packet);
  if(regIndex <= 34)
    {
      // invalidate the Per Thread entry
      mainRegisters.perThreadRead[threadNo]=0;
    }
  else
    {
      // invalidate the global entry
      mainRegisters.globalsRead=0;
    }

  /* send down the WRITE_REGISTERS command */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, WRITE_REGISTERS);

  /* send down the thread # */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)threadNo);

  /* send down the start address of the first register to read back */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX,
				    ubicom32MainGdbIndexToRegAddrMap[regIndex]);
  /* Send a length of transfer */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)length);

  while(length)
    {
      unsigned int transferLength = ubicom32command_packet_get_free(&packet);

      if(transferLength > length)
	transferLength = length;

      length = length - transferLength;

      for(i=0; i< transferLength; i++)
	{
	  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX,
					    (unsigned int)(*inBuf++));
	}

      if(length || (ubicom32command_packet_get_free(&packet) == 0))
	{
	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(&packet);
	  if(ret)
	    return ret;
	}
      else
	break;

      ubicom32command_packet_init(&packet);
    }


  // We are expecting and echo of WRITE_REGISTERS_DONE
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  commandEchoIndex = count;

  // Ship the packet for processing
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  // Test the reply
  command = &packet.commands[commandEchoIndex];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != WRITE_REGISTERS_DONE)
    {
      printf("Reply 0x%x does not match WRITE_REGISTERS_DONE\n", reply);
      return -1;
    }

  return 0;
}

int
gdbMainGetallRegs(unsigned int threadNo, mercuryMainRegs_t *mainRegs)
{
  int ret, temp;
  int *dptr = &mainRegs->tRegs[threadNo].dr[0];

  if(threadNo == debuggerThreadNo)
    {
      // Register requests from debugger thread. Do not honor it
      return 0;
    }

  if(mainRegs->perThreadRead[threadNo] ==0)
    {
      /* get all per thread debugger registers for the given thread */
      mainRegs->perThreadRead[threadNo]=1;

#ifndef DEBUG_RTL
      /* read the data register block for the given thread */
      ret = readRegisters(threadNo, 0, dptr, 16);
      if(ret)
	return (ret);

      /* read the address register block */
      dptr = &mainRegs->tRegs[threadNo].ar[0];
      ret = readRegisters(threadNo, 16, dptr, 18);
      if(ret)
	return(ret);

      /* read the thread pc */
      dptr = &mainRegs->tRegs[threadNo].threadPc;
      ret = readRegisters(threadNo, 34, dptr, 1);
      if(ret)
	return ret;
#endif
    }

  // patch rosr
  temp = mainRegs->tRegs[threadNo].rosr;
  temp &= ~0xc;
  temp |= (threadNo<<2);
  mainRegs->tRegs[threadNo].rosr = temp;


  if(mainRegs->globalsRead == 0)
    {
      mainRegs->globalsRead = 1;

#ifndef DEBUG_RTL
      /* Start on the globals. Begin athe chip_id */
      dptr = &mainRegs->globals.chip_id;
      ret = readRegisters(1, 35, dptr, 3);
      if(ret)
	return ret;

      /* read global control */
      dptr = &mainRegs->globals.global_ctrl;
      ret = readRegisters(1, 42, dptr, 1);
      if(ret)
	return ret;

      /* read mt_active */
      dptr = &mainRegs->globals.mt_active;
      ret = readRegisters(1, 45, dptr, 1);
      if(ret)
	return ret;

      /* read mt_dbg_active */
      dptr = &mainRegs->globals.mt_dbg_active;
      ret = readRegisters(1, 47, dptr, 1);
      if(ret)
	return ret;

      /* read mt_en etc */
      dptr = &mainRegs->globals.mt_en;
      ret = readRegisters(1, 48, dptr, 3);
      if(ret)
	return ret;

#if 0
      // Nuke thread 0 out of the picture
      temp = *dptr;
      temp &= ~1;
      *dptr = temp;
#endif

      dptr = &mainRegs->globals.mt_break;
      ret = readRegisters(1, 52, dptr, 1);
      if(ret)
	return ret;

      dptr = &mainRegs->globals.mt_single_step;
      ret = readRegisters(1, 53, dptr, 2);
      if(ret)
	return ret;

      dptr = &mainRegs->globals.perr_addr;
      ret = readRegisters(1, 56, dptr, 1);
      if(ret)
	return ret;

      dptr = &mainRegs->globals.dcapt_tnum;
      ret = readRegisters(1, 57, dptr, 1);
      if(ret)
	return ret;

      dptr = &mainRegs->globals.dcapt_pc;
      ret = readRegisters(1, 58, dptr, 1);
      if(ret)
	return ret;

      dptr = &mainRegs->globals.dcapt;
      ret = readRegisters(1, 59, dptr, 1);
      if(ret)
	return ret;

      dptr = &mainRegs->globals.scratchpad0;
      ret = readRegisters(1, 60, dptr, 4);
      if(ret)
	return ret;
#endif
    }

  return 0;
}

int
gdbMainGetTPacketRegs(int threadNo, mercuryMainRegs_t *mainRegs)
{
  extern unsigned int  mt_active, mt_dbg_active, mt_en, mt_sched, mt_pri;
  int ret, temp;
  /* get d0 register for the given thread */

  /* read the data register block for the given thread */
  int *dptr = &mainRegs->tRegs[threadNo].dr[0];

  ret = readRegisters(threadNo, 0, dptr, 1);
  if(ret)
    return (ret);

  /* read the address a5-a7 register block */
  dptr = &mainRegs->tRegs[threadNo].ar[5];
  ret = readRegisters(threadNo, 21, dptr, 3);
  if(ret)
    return(ret);

  /* read the thread rosr */
  dptr = &mainRegs->tRegs[threadNo].rosr;
  ret = readRegisters(threadNo, 30, dptr, 1);
  if(ret)
    return ret;

  temp = *dptr;
  temp &= ~0xc;
  temp |= (threadNo<<2);
  *dptr = temp;

  /* read the thread pc */
  dptr = &mainRegs->tRegs[threadNo].threadPc;
  ret = readRegisters(threadNo, 34, dptr, 1);
  if(ret)
    return ret;

  /* read mt_active */
  dptr = &mainRegs->globals.mt_active;
  ret = readRegisters(threadNo, 45, dptr, 1);
  if(ret)
    return ret;

  /* read mt_dbg_active */
  dptr = &mainRegs->globals.mt_dbg_active;
  ret = readRegisters(threadNo, 47, dptr, 1);
  if(ret)
    return ret;

  /* read mt_en etc */
  dptr = &mainRegs->globals.mt_en;
  ret = readRegisters(threadNo, 48, dptr, 3);
  if(ret)
    return ret;

  // Update some of our internal copies

  mt_active = (int) mainRegisters.globals.mt_active;
  mt_dbg_active = (int) mainRegisters.globals.mt_dbg_active;
  mt_en = (int) mainRegisters.globals.mt_en;
  mt_pri = (int) mainRegisters.globals.mt_hpri;
  mt_sched = (int) mainRegisters.globals.mt_hrt;

#if 0
  // Nuke thread 0 out of the picture
  temp = *dptr;
  temp &= ~1;
  *dptr = temp;
#endif
  return 0;
}

int
readDataMemory(unsigned int destAddr, unsigned int length, int *data)
{
  if(okToDoubleBuffer && length > 4 && ((dkmajor > 1) || (dkincrement >=4)))
    {
      int ret, count, commandEchoIndex;
      int pending_transferStartIndex = 0;
      int pending_transferLength = 0;
      int i;
      unsigned int reply;

      struct ubicom32command_packet packets[2];
      struct ubicom32command_packet *packet;
      struct ubicom32command *command= NULL;
      unsigned char *cptr;

      packet = packets;
      ubicom32command_packet_init(packet);

      doubleBuffer = 1;
      pending_packet = NULL;

      /* send down the READ_DATA_MEMORY command */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, READ_DATA_MEMORY);

      /* send down the start address of where to read from */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, (unsigned int)(length*4));

      count = ubicom32command_packet_append(packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
      commandEchoIndex = count;	// The entry at this index is going to contain READ_DATA_MEMORY_DONE

      /* ship the first part of the command down */
      ret = ubicom32command_packet_send(packet);
      if(ret)
	{
	  doubleBuffer = 0;
	  pending_packet = NULL;
	  return ret;
	}

      // Setup the pending packet
      pending_packet = packet;
      pending_packet_length = ntohl (pending_packet->header.length);
      pending_packet_opcode = ntohl (pending_packet->header.opcode);

      if(packet == packets)
	packet = &packets[1];
      else
	packet = packets;

      while(length)
	{
	  unsigned int transferLength = 256;

	  // initialize packet for reading mailbox
	  ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX);

	  if(transferLength > length)
	    transferLength = length;

	  // load length into operand field
	  packet->header.operand = transferLength;
	  packet->header.length = 16;

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(packet);
	  if(ret)
	    {
	      doubleBuffer = 0;
	      pending_packet = NULL;
	      return ret;
	    }

	  if(pending_packet)
	    {
	      // Process the return data
	      if(commandEchoIndex)
		{
		  // Test the reply for READ_DATA_MEMORY_DONE
		  command = &pending_packet->commands[commandEchoIndex];
		  reply = (command->operand0<<24) | (command->operand1<<16) |
		    (command->operand2<<8) | command->operand3;

		  if(reply != READ_DATA_MEMORY_DONE)
		    {
		      printf("Reply 0x%x does not match READ_DATA_MEMORY_DONE\n", reply);
		      doubleBuffer = 0;
		      pending_packet = NULL;
		      return -1;
		    }
		  else
		    commandEchoIndex = 0;
		}

	      // Move the reply data into destination buffer
	      cptr = (unsigned char *)&pending_packet->commands[pending_transferStartIndex];
	      for(i=0; i< pending_transferLength; i++, cptr +=4)
		{
		  reply = (cptr[0]<<24) | (cptr[1]<<16) |
		    (cptr[2]<<8) | cptr[3];

		  *data++ = (int)reply;
		}
	    }

	  // Setup the pending packet
	  pending_packet = packet;
	  pending_packet_length = (ntohl (pending_packet->header.length) + 4*transferLength);
	  pending_packet_opcode = ntohl (pending_packet->header.opcode);
	  pending_transferLength = transferLength;

	  if(packet == packets)
	    packet = &packets[1];
	  else
	    packet = packets;

	  // readjust length etc.
	  // ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX);
	  length = length - transferLength;
	}

      if(pending_packet)
	{
	  retrieve_pending_packet();

	  // Process the return data
	  if(commandEchoIndex)
	    {
	      // Test the reply for READ_DATA_MEMORY_DONE
	      command = &pending_packet->commands[commandEchoIndex];
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      if(reply != READ_DATA_MEMORY_DONE)
		{
		  printf("Reply 0x%x does not match READ_DATA_MEMORY_DONE\n", reply);
		  doubleBuffer = 0;
		  pending_packet = NULL;
		  return -1;
		}
	      else
		commandEchoIndex = 0;
	    }

	  // Move the reply data into destination buffer
	  cptr = (unsigned char *)&pending_packet->commands[pending_transferStartIndex];
	  for(i=0; i< pending_transferLength; i++, cptr +=4)
	    {
	      reply = (cptr[0]<<24) | (cptr[1]<<16) |
		(cptr[2]<<8) | cptr[3];

	      *data++ = (int)reply;
	    }
	}

      pending_packet = NULL;
      doubleBuffer = 0;
      return 0;
    }
  else
    {
      int ret, count;
      int commandEchoIndex;
      unsigned int reply;

      struct ubicom32command_packet packet;
      ubicom32command_packet_init(&packet);

      /* send down the READ_DATA_MEMORY command */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, READ_DATA_MEMORY);

      /* send down the start address of where to read from */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)(length*4));

      // The entry at this index is going to contain READ_DATA_MEMORY_DONE
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      commandEchoIndex = count;

      while(length)
	{
	  unsigned int transferLength = ubicom32command_packet_get_free(&packet);
	  unsigned int i;
	  int transferStartIndex;
	  struct ubicom32command *command;

	  if(transferLength > length)
	    transferLength = length;

	  // Index of the first transfer entry
	  transferStartIndex = ubicom32command_packet_get_next(&packet);
	  command = &packet.commands[transferStartIndex];

	  for(i=0; i< transferLength; i++)
	    count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(&packet);
	  if(ret)
	    return ret;

	  // Process the return data
	  if(commandEchoIndex)
	    {
	      // Test the reply for READ_REGISTERS_DONE
	      command = &packet.commands[commandEchoIndex];
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      if(reply != READ_DATA_MEMORY_DONE)
		{
		  printf("Reply 0x%x does not match READ_DATA_MEMORY_DONE\n", reply);
		  return -1;
		}
	      else
		commandEchoIndex = 0;
	    }

	  // Move the reply data into destination buffer
	  command = &packet.commands[transferStartIndex];
	  for(i=0; i< transferLength; i++, command++)
	    {
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      *data++ = (int)reply;
	    }

	  // readjust length etc.
	  ubicom32command_packet_init(&packet);
	  length = length - transferLength;
	}

      return 0;
    }
}

int
readDataMemoryRaw(unsigned int destAddr, unsigned int length, int *data)
{
  if(okToDoubleBuffer && length > 4 && ((dkmajor > 1) || (dkincrement >=4)))
    {
      int ret, count, commandEchoIndex;
      int pending_transferStartIndex = 0;
      int pending_transferLength = 0;
      unsigned int reply;

      struct ubicom32command_packet packets[2];
      struct ubicom32command_packet *packet;
      struct ubicom32command *command= NULL;
      unsigned char *cptr;

      packet = packets;
      ubicom32command_packet_init(packet);

      doubleBuffer = 1;
      pending_packet = NULL;

      /* send down the READ_DATA_MEMORY command */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, READ_DATA_MEMORY);

      /* send down the start address of where to read from */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, (unsigned int)(length*4));

      count = ubicom32command_packet_append(packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
      commandEchoIndex = count;	// The entry at this index is going to contain READ_DATA_MEMORY_DONE

      /* ship the first part of the command down */
      ret = ubicom32command_packet_send(packet);
      if(ret)
	{
	  doubleBuffer = 0;
	  pending_packet = NULL;
	  return ret;
	}

      // Setup the pending packet
      pending_packet = packet;
      pending_packet_length = ntohl (pending_packet->header.length);
      pending_packet_opcode = ntohl (pending_packet->header.opcode);

      if(packet == packets)
	packet = &packets[1];
      else
	packet = packets;

      while(length)
	{
	  unsigned int transferLength = 256;

	  // initialize packet for reading mailbox
	  ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX);

	  if(transferLength > length)
	    transferLength = length;

	  // load length into operand field
	  packet->header.operand = transferLength;
	  packet->header.length = 16;

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(packet);
	  if(ret)
	    {
	      doubleBuffer = 0;
	      pending_packet = NULL;
	      return ret;
	    }

	  if(pending_packet)
	    {
	      // Process the return data
	      if(commandEchoIndex)
		{
		  // Test the reply for READ_DATA_MEMORY_DONE
		  command = &pending_packet->commands[commandEchoIndex];
		  reply = (command->operand0<<24) | (command->operand1<<16) |
		    (command->operand2<<8) | command->operand3;

		  if(reply != READ_DATA_MEMORY_DONE)
		    {
		      printf("Reply 0x%x does not match READ_DATA_MEMORY_DONE\n", reply);
		      doubleBuffer = 0;
		      pending_packet = NULL;
		      return -1;
		    }
		  else
		    commandEchoIndex = 0;
		}

	      // Move the reply data into destination buffer
	      cptr = (unsigned char *)&pending_packet->commands[pending_transferStartIndex];

#if 0
	      for(i=0; i< pending_transferLength; i++, cptr +=4)
		{
		  reply = (cptr[0]<<24) | (cptr[1]<<16) |
		    (cptr[2]<<8) | cptr[3];

		  *data++ = (int)reply;
		}
#endif
	      memcpy(data, cptr, pending_transferLength*4);
	      data += pending_transferLength;
	    }

	  // Setup the pending packet
	  pending_packet = packet;
	  pending_packet_length = (ntohl (pending_packet->header.length) + 4*transferLength);
	  pending_packet_opcode = ntohl (pending_packet->header.opcode);
	  pending_transferLength = transferLength;

	  if(packet == packets)
	    packet = &packets[1];
	  else
	    packet = packets;

	  // readjust length etc.
	  // ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX);
	  length = length - transferLength;
	}

      if(pending_packet)
	{
	  retrieve_pending_packet();

	  // Process the return data
	  if(commandEchoIndex)
	    {
	      // Test the reply for READ_DATA_MEMORY_DONE
	      command = &pending_packet->commands[commandEchoIndex];
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      if(reply != READ_DATA_MEMORY_DONE)
		{
		  printf("Reply 0x%x does not match READ_DATA_MEMORY_DONE\n", reply);
		  doubleBuffer = 0;
		  pending_packet = NULL;
		  return -1;
		}
	      else
		commandEchoIndex = 0;
	    }

	  // Move the reply data into destination buffer
	  cptr = (unsigned char *)&pending_packet->commands[pending_transferStartIndex];

#if 0
	  for(i=0; i< pending_transferLength; i++, cptr +=4)
	    {
	      reply = (cptr[0]<<24) | (cptr[1]<<16) |
		(cptr[2]<<8) | cptr[3];

	      *data++ = (int)reply;
	    }
#endif

	  if(pending_transferLength)
	    {
	      memcpy(data, cptr, pending_transferLength*4);
	      data += pending_transferLength;
	    }
	}

      pending_packet = NULL;
      doubleBuffer = 0;
      return 0;
    }
  else
    {
      int ret, count;
      int commandEchoIndex;
      unsigned int reply;

      struct ubicom32command_packet packet;
      ubicom32command_packet_init(&packet);

      /* send down the READ_DATA_MEMORY command */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, READ_DATA_MEMORY);

      /* send down the start address of where to read from */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)(length*4));

      // The entry at this index is going to contain READ_DATA_MEMORY_DONE
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      commandEchoIndex = count;

      while(length)
	{
	  unsigned int transferLength = ubicom32command_packet_get_free(&packet);
	  unsigned int i;
	  int transferStartIndex;
	  struct ubicom32command *command;

	  if(transferLength > length)
	    transferLength = length;

	  // Index of the first transfer entry
	  transferStartIndex = ubicom32command_packet_get_next(&packet);
	  command = &packet.commands[transferStartIndex];

	  for(i=0; i< transferLength; i++)
	    count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(&packet);
	  if(ret)
	    return ret;

	  // Process the return data
	  if(commandEchoIndex)
	    {
	      // Test the reply for READ_REGISTERS_DONE
	      command = &packet.commands[commandEchoIndex];
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      if(reply != READ_DATA_MEMORY_DONE)
		{
		  printf("Reply 0x%x does not match READ_DATA_MEMORY_DONE\n", reply);
		  return -1;
		}
	      else
		commandEchoIndex = 0;
	    }

	  // Move the reply data into destination buffer
	  command = &packet.commands[transferStartIndex];
	  for(i=0; i< transferLength; i++, command++)
	    {
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      *data++ = (int)reply;
	    }

	  // readjust length etc.
	  ubicom32command_packet_init(&packet);
	  length = length - transferLength;
	}

      return 0;
    }
}

int
writeDataMemory(unsigned int destAddr, unsigned int length, int *data)
{
  int ret, count, commandEchoIndex;
  unsigned int i;
  struct ubicom32command *command;
  struct ubicom32command_packet packet;
  unsigned int reply;
  ubicom32command_packet_init(&packet);

  /* send down the WRITE_DATA_MEMORY command */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, WRITE_DATA_MEMORY);

  /* send down the start address  */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, destAddr);

  /* Send a length of transfer */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)(length*4));

  while(length)
    {
      unsigned int transferLength = ubicom32command_packet_get_free(&packet);

      if(transferLength > length)
	transferLength = length;

      length -= transferLength;

      for(i=0; i< transferLength; i++)
	{
	  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)(*data++));
	}

      if(length || (ubicom32command_packet_get_free(&packet) == 0))
	{
	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(&packet);
	  if(ret)
	    return ret;
	}
      else
	break;

      // readjust length etc.
      ubicom32command_packet_init(&packet);
    }

  // We are expecting and echo of WRITE_DATA_MEMORY_DONE
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  commandEchoIndex = count;

  // Ship the packet for processing
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  // Test the reply
  command = &packet.commands[commandEchoIndex];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != WRITE_DATA_MEMORY_DONE)
    {
      printf("Reply 0x%x does not match WRITE_DATA_MEMORY_DONE\n", reply);
      return -1;
    }

  return 0;
}

int
readPgmMemory(unsigned int destAddr, unsigned int length, int *data)
{
  if(okToDoubleBuffer && length > 4 && ((dkmajor > 1) || (dkincrement >=4)))
    {
      int ret, count, commandEchoIndex;
      int pending_transferStartIndex = 0;
      int pending_transferLength = 0;
      int i;
      unsigned int reply;

      struct ubicom32command_packet packets[2];
      struct ubicom32command_packet *packet;
      struct ubicom32command *command= NULL;
      unsigned char *cptr;

      packet = packets;
      ubicom32command_packet_init(packet);

      doubleBuffer = 1;
      pending_packet = NULL;

      /* send down the READ_DATA_MEMORY command */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, READ_PGM_MEMORY);

      /* send down the start address of where to read from */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, (unsigned int)(length));

      count = ubicom32command_packet_append(packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
      commandEchoIndex = count;	// The entry at this index is going to contain READ_PGM_MEMORY_DONE

      /* ship the first part of the command down */
      ret = ubicom32command_packet_send(packet);
      if(ret)
	{
	  doubleBuffer = 0;
	  pending_packet = NULL;
	  return ret;
	}

      // Setup the pending packet
      pending_packet = packet;
      pending_packet_length = ntohl (pending_packet->header.length);
      pending_packet_opcode = ntohl (pending_packet->header.opcode);

      if(packet == packets)
	packet = &packets[1];
      else
	packet = packets;

      while(length)
	{
	  unsigned int transferLength = 256;

	  // initialize packet for reading mailbox
	  ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX);

	  if(transferLength > length)
	    transferLength = length;

	  // load length into operand field
	  packet->header.operand = transferLength;
	  packet->header.length = 16;

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(packet);
	  if(ret)
	    {
	      doubleBuffer = 0;
	      pending_packet = NULL;
	      return ret;
	    }

	  if(pending_packet)
	    {
	      // Process the return data
	      if(commandEchoIndex)
		{
		  // Test the reply for READ_PGM_MEMORY_DONE
		  command = &pending_packet->commands[commandEchoIndex];
		  reply = (command->operand0<<24) | (command->operand1<<16) |
		    (command->operand2<<8) | command->operand3;

		  if(reply != READ_PGM_MEMORY_DONE)
		    {
		      printf("Reply 0x%x does not match READ_PGM_MEMORY_DONE\n", reply);
		      doubleBuffer = 0;
		      pending_packet = NULL;
		      return -1;
		    }
		  else
		    commandEchoIndex = 0;
		}

	      // Move the reply data into destination buffer
	      cptr = (unsigned char *)&pending_packet->commands[pending_transferStartIndex];
	      for(i=0; i< pending_transferLength; i++, cptr +=4)
		{
		  reply = (cptr[0]<<24) | (cptr[1]<<16) |
		    (cptr[2]<<8) | cptr[3];

		  *data++ = (int)reply;
		}
	    }

	  // Setup the pending packet
	  pending_packet = packet;
	  pending_packet_length = (ntohl (pending_packet->header.length) + 4*transferLength);
	  pending_packet_opcode = ntohl (pending_packet->header.opcode);
	  pending_transferLength = transferLength;

	  if(packet == packets)
	    packet = &packets[1];
	  else
	    packet = packets;

	  // readjust length etc.
	  // ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX);
	  length = length - transferLength;
	}

      if(pending_packet)
	{
	  retrieve_pending_packet();

	  // Process the return data
	  if(commandEchoIndex)
	    {
	      // Test the reply for READ_PGM_MEMORY_DONE
	      command = &pending_packet->commands[commandEchoIndex];
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      if(reply != READ_PGM_MEMORY_DONE)
		{
		  printf("Reply 0x%x does not match READ_PGM_MEMORY_DONE\n", reply);
		  doubleBuffer = 0;
		  pending_packet = NULL;
		  return -1;
		}
	      else
		commandEchoIndex = 0;
	    }

	  // Move the reply data into destination buffer
	  cptr = (unsigned char *)&pending_packet->commands[pending_transferStartIndex];
	  for(i=0; i< pending_transferLength; i++, cptr +=4)
	    {
	      reply = (cptr[0]<<24) | (cptr[1]<<16) |
		(cptr[2]<<8) | cptr[3];

	      *data++ = (int)reply;
	    }
	}

      pending_packet = NULL;
      doubleBuffer = 0;
      return 0;
    }
  else
    {
      int ret, count, commandEchoIndex;

      struct ubicom32command_packet packet;
      ubicom32command_packet_init(&packet);

      /* send down the READ_DATA_MEMORY command */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, READ_PGM_MEMORY);

      /* send down the start address of where to read from */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)(length));

      count = ubicom32command_packet_append(&packet, READ_CHECK|OPTION_EXTENDED_TIMEOUT, ISD_READ_MAIL_BOX, 0);
      commandEchoIndex = count;	// The entry at this index is going to contain READ_PGM_MEMORY_DONE

      while(length)
	{
	  unsigned int transferLength = ubicom32command_packet_get_free(&packet);
	  unsigned int i;
	  struct ubicom32command *command;
	  unsigned int reply;
	  int transferStartIndex;

	  if(transferLength > length)
	    transferLength = length;

	  // Index of the first transfer entry
	  transferStartIndex = ubicom32command_packet_get_next(&packet);
	  command = &packet.commands[transferStartIndex];

	  for(i=0; i< transferLength; i++)
	    count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(&packet);
	  if(ret)
	    return ret;

	  // Process the return data
	  if(commandEchoIndex)
	    {
	      // Test the reply for READ_PGM_MEMORY_DONE
	      command = &packet.commands[commandEchoIndex];
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      if(reply != READ_PGM_MEMORY_DONE)
		{
		  printf("Reply 0x%x does not match READ_PGM_MEMORY_DONE\n", reply);
		  return -1;
		}
	      else
		commandEchoIndex = 0;
	    }

	  // Move the reply data into destination buffer
	  command = &packet.commands[transferStartIndex];
	  for(i=0; i< transferLength; i++, command++)
	    {
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      *data++ = (int)reply;
	    }

	  // readjust length etc.
	  ubicom32command_packet_init(&packet);
	  length = length - transferLength;
	}

      return 0;
    }
}

int
readPgmMemoryRaw(unsigned int destAddr, unsigned int length, int *data)
{
  if(okToDoubleBuffer && length > 4 && ((dkmajor > 1) || (dkincrement >=4)))
    {
      int ret, count, commandEchoIndex;
      int pending_transferStartIndex = 0;
      int pending_transferLength = 0;
      unsigned int reply;

      struct ubicom32command_packet packets[2];
      struct ubicom32command_packet *packet;
      struct ubicom32command *command= NULL;
      unsigned char *cptr;

      packet = packets;
      ubicom32command_packet_init(packet);

      doubleBuffer = 1;
      pending_packet = NULL;

      /* send down the READ_DATA_MEMORY command */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, READ_PGM_MEMORY);

      /* send down the start address of where to read from */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, (unsigned int)(length));

      count = ubicom32command_packet_append(packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
      commandEchoIndex = count;	// The entry at this index is going to contain READ_PGM_MEMORY_DONE

      /* ship the first part of the command down */
      ret = ubicom32command_packet_send(packet);
      if(ret)
	{
	  doubleBuffer = 0;
	  pending_packet = NULL;
	  return ret;
	}

      // Setup the pending packet
      pending_packet = packet;
      pending_packet_length = ntohl (pending_packet->header.length);
      pending_packet_opcode = ntohl (pending_packet->header.opcode);

      if(packet == packets)
	packet = &packets[1];
      else
	packet = packets;

      while(length)
	{
	  unsigned int transferLength = 256;

	  // initialize packet for reading mailbox
	  if (debugging_processor == 3023 && dongleIsUbipod &&
	      ((0x20000000 <= destAddr) && (destAddr < 0x20400000)) )
	    {
	      /* Reading back flash data via Ubipod from a 3k. Use the slower interface. */
	      ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX_UBICOM32);
	    }
	  else
	    ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX);


	  if(transferLength > length)
	    transferLength = length;

	  // load length into operand field
	  packet->header.operand = transferLength;
	  packet->header.length = 16;

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(packet);
	  if(ret)
	    {
	      doubleBuffer = 0;
	      pending_packet = NULL;
	      return ret;
	    }

	  if(pending_packet)
	    {
	      // Process the return data
	      if(commandEchoIndex)
		{
		  // Test the reply for READ_PGM_MEMORY_DONE
		  command = &pending_packet->commands[commandEchoIndex];
		  reply = (command->operand0<<24) | (command->operand1<<16) |
		    (command->operand2<<8) | command->operand3;

		  if(reply != READ_PGM_MEMORY_DONE)
		    {
		      printf("Reply 0x%x does not match READ_PGM_MEMORY_DONE\n", reply);
		      doubleBuffer = 0;
		      pending_packet = NULL;

		      return -1;
		    }
		  else
		    commandEchoIndex = 0;
		}

	      // Move the reply data into destination buffer
	      cptr = (unsigned char *)&pending_packet->commands[pending_transferStartIndex];

#if 0
	      for(i=0; i< pending_transferLength; i++, cptr +=4)
		{
		  reply = (cptr[0]<<24) | (cptr[1]<<16) |
		    (cptr[2]<<8) | cptr[3];

		  *data++ = (int)reply;
		}
#endif
	      if (pending_transferLength)
		{
		  memcpy(data, cptr, pending_transferLength*4);
		  data += pending_transferLength;
		}
	    }

	  // Setup the pending packet
	  pending_packet = packet;
	  pending_packet_length = (ntohl (pending_packet->header.length) + 4*transferLength);
	  pending_packet_opcode = ntohl (pending_packet->header.opcode);
	  pending_transferLength = transferLength;

	  if(packet == packets)
	    packet = &packets[1];
	  else
	    packet = packets;

	  // readjust length etc.
	  // ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX);
	  length = length - transferLength;
	}

      if(pending_packet)
	{
	  retrieve_pending_packet();

	  // Process the return data
	  if(commandEchoIndex)
	    {
	      // Test the reply for READ_PGM_MEMORY_DONE
	      command = &pending_packet->commands[commandEchoIndex];
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      if(reply != READ_PGM_MEMORY_DONE)
		{
		  printf("Reply 0x%x does not match READ_PGM_MEMORY_DONE\n", reply);
		  doubleBuffer = 0;
		  pending_packet = NULL;
		  return -1;
		}
	      else
		commandEchoIndex = 0;
	    }

	  // Move the reply data into destination buffer
	  cptr = (unsigned char *)&pending_packet->commands[pending_transferStartIndex];

#if 0
	  for(i=0; i< pending_transferLength; i++, cptr +=4)
	    {
	      reply = (cptr[0]<<24) | (cptr[1]<<16) |
		(cptr[2]<<8) | cptr[3];

	      *data++ = (int)reply;
	    }
#endif
	  memcpy(data, cptr, pending_transferLength*4);
	  data += pending_transferLength;

	}

      pending_packet = NULL;
      doubleBuffer = 0;
      return 0;
    }
  else
    {
      int ret, count, commandEchoIndex;

      struct ubicom32command_packet packet;
      ubicom32command_packet_init(&packet);

      /* send down the READ_DATA_MEMORY command */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, READ_PGM_MEMORY);

      /* send down the start address of where to read from */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)(length));

      count = ubicom32command_packet_append(&packet, READ_CHECK|OPTION_EXTENDED_TIMEOUT, ISD_READ_MAIL_BOX, 0);
      commandEchoIndex = count;	// The entry at this index is going to contain READ_PGM_MEMORY_DONE

      while(length)
	{
	  unsigned int transferLength = ubicom32command_packet_get_free(&packet);
	  unsigned int i;
	  struct ubicom32command *command;
	  unsigned int reply;
	  int transferStartIndex;

	  if(transferLength > length)
	    transferLength = length;

	  // Index of the first transfer entry
	  transferStartIndex = ubicom32command_packet_get_next(&packet);
	  command = &packet.commands[transferStartIndex];

	  for(i=0; i< transferLength; i++)
	    count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(&packet);
	  if(ret)
	    return ret;

	  // Process the return data
	  if(commandEchoIndex)
	    {
	      // Test the reply for READ_PGM_MEMORY_DONE
	      command = &packet.commands[commandEchoIndex];
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      if(reply != READ_PGM_MEMORY_DONE)
		{
		  printf("Reply 0x%x does not match READ_PGM_MEMORY_DONE\n", reply);
		  return -1;
		}
	      else
		commandEchoIndex = 0;
	    }

	  // Move the reply data into destination buffer
	  command = &packet.commands[transferStartIndex];
	  for(i=0; i< transferLength; i++, command++)
	    {
	      reply = (command->operand0<<24) | (command->operand1<<16) |
		(command->operand2<<8) | command->operand3;

	      *data++ = (int)reply;
	    }

	  // readjust length etc.
	  ubicom32command_packet_init(&packet);
	  length = length - transferLength;
	}

      return 0;
    }
}

int
crcPgmMemory(unsigned int destAddr, unsigned int length, int *data)
{
  int ret, count, commandEchoIndex, crcResultIndex;
  struct ubicom32command *command;
  unsigned int reply;
  unsigned int crc, actual_crc;
  unsigned char *ptr;

  struct ubicom32command_packet packet;
  ubicom32command_packet_init(&packet);

  /* send down the READ_DATA_MEMORY command */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, CRC_PGM_MEMORY);

  /* send down the start address of where to read from */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, destAddr);

  /* Send a length of transfer */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)(length));

  count = ubicom32command_packet_append(&packet, READ_CHECK|OPTION_EXTENDED_TIMEOUT, ISD_READ_MAIL_BOX, 0);
  commandEchoIndex = count;	// The entry at this index is going to contain CRC_PGM_MEMORY_DONE

  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  crcResultIndex = count;

  // Ship the packet for processing
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  // Test the reply for CRC_PGM_MEMORY_DONE
  command = &packet.commands[commandEchoIndex];
  reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

  if(reply != CRC_PGM_MEMORY_DONE)
    {
      printf("Reply 0x%x does not match CRC_PGM_MEMORY_DONE\n", reply);
      return -1;
    }

  // Move the reply data into destination buffer
  command = &packet.commands[crcResultIndex];
  actual_crc = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

  // Calculate good crc
  count = length * 4;
  ptr = (unsigned char *)data;
  crc = 0xFFFFFFFF;
  while (count--)
    {
      int index = (crc & 0x0FF) ^ *ptr++;
      crc >>= 8;
      if (index &   1) crc ^= 0x77073096;
      if (index &   2) crc ^= 0xEE0E612C;
      if (index &   4) crc ^= 0x076DC419;
      if (index &   8) crc ^= 0x0EDB8832;
      if (index &  16) crc ^= 0x1DB71064;
      if (index &  32) crc ^= 0x3B6E20C8;
      if (index &  64) crc ^= 0x76DC4190;
      if (index & 128) crc ^= 0xEDB88320;
    }
  crc ^= 0xFFFFFFFF;

  //Compare crc result
  if (crc != actual_crc)
    return -1;

  return 0;
}

unsigned int loadStats[2048];
unsigned int loadStatIndex;
unsigned int loadLengths[2048];

int
writePgmMemory(unsigned int destAddr, unsigned int length, int *data)
{
  if(okToDoubleBuffer && length > 4 && (((dkmajor > 1) || (dkincrement >= 4)) || debugging_processor == 5133))
    {
      int ret, count;
      unsigned int i;
      struct ubicom32command *command;
      unsigned int reply;
      struct ubicom32command_packet packets[2], *packet;

      packet = packets;
      doubleBuffer = 1;
      pending_packet = NULL;

      ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND);

      /* send down the WRITE_PGM_MEMORY command */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, WRITE_PGM_MEMORY);

      /* send down the start address  */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, (unsigned int)(length));

      /* ship the first part of the command down */
      ret = ubicom32command_packet_send(packet);
      if(ret)
	{
	  doubleBuffer = 0;
	  pending_packet = NULL;
	  return ret;
	}

      // Setup the pending packet
      pending_packet = packet;
      pending_packet_length = ntohl (pending_packet->header.length);
      pending_packet_opcode = ntohl (pending_packet->header.opcode);

      if(packet == packets)
	packet = &packets[1];
      else
	packet = packets;


      while(length)
	{
	  unsigned int transferLength = 256;
	  unsigned int *dest;

	  // initialize packet for writing mailbox
	  ubicom32command_packet_init_opcode(packet, ISPISD_OPCODE_UBICOM32COMMAND_WRITE_MAILBOX);

	  if(transferLength > length)
	    transferLength = length;

	  // load length into operand field
	  packet->header.operand = transferLength;
	  packet->header.length = 16 + transferLength * 4;

	  // adjust length
	  length -= transferLength;

	  /* copy the data into the command data space */
	  dest = (unsigned int *)&packet->commands[0];
	  for(i=0; i< transferLength; i++, dest++, data++)
	    {
	      unsigned char *s, *d;
	      d = (unsigned char *)dest;
	      s = (unsigned char *)data;

	      d[3] = s[0];
	      d[2] = s[1];
	      d[1] = s[2];
	      d[0] = s[3];
	    }
	  //memcpy(dest, (void *) data, transferLength * 4);

	  // Ship the packet for processing
	  ret = ubicom32command_packet_send(packet);
	  if(ret)
	    {
	      doubleBuffer = 0;
	      pending_packet = NULL;
	      return ret;
	    }

	  // Setup the pending packet
	  pending_packet = packet;
	  pending_packet_length = 16;
	  pending_packet_opcode = ntohl (pending_packet->header.opcode);

	  if(packet == packets)
	    packet = &packets[1];
	  else
	    packet = packets;
	}

      // There is data to be read. Go get it
      ubicom32command_packet_init(packet);

      // Go read the data from the mailbox
      count = ubicom32command_packet_append(packet, READ_CHECK|OPTION_EXTENDED_TIMEOUT, ISD_READ_MAIL_BOX, 0);

      /* Ship the packet down for execution */
      ret = ubicom32command_packet_send(packet);
      if(ret)
	{
	  doubleBuffer = 0;
	  pending_packet = NULL;
	  return ret;
	}

      // Setup the pending packet
      pending_packet = packet;
      pending_packet_length = ntohl (pending_packet->header.length);
      pending_packet_opcode = ntohl (pending_packet->header.opcode);


      // We are expecting and echo of WRITE_PGM_MEMORY_DONE
      retrieve_pending_packet();

      // Test the reply
      command = &pending_packet->commands[0];

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      if(reply != WRITE_PGM_MEMORY_DONE)
	{
	  printf("Reply 0x%x does not match WRITE_PGM_MEMORY_DONE\n", reply);
	  doubleBuffer = 0;
	  pending_packet = NULL;
	  return -1;
	}

      pending_packet = NULL;
      doubleBuffer = 0;
      return 0;
    }
  else
    {
      int ret, count, commandEchoIndex;
      unsigned int i;
      struct ubicom32command *command;
      unsigned int reply;
      struct ubicom32command_packet packet;
      ubicom32command_packet_init(&packet);

      /* send down the WRITE_PGM_MEMORY command */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, WRITE_PGM_MEMORY);

      /* send down the start address  */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, destAddr);

      /* Send a length of transfer */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, (unsigned int)(length));

      while(length)
	{
	  unsigned int transferLength = ubicom32command_packet_get_free(&packet);

	  if(transferLength > length)
	    transferLength = length;

	  length -= transferLength;

	  for(i=0; i< transferLength; i++)
	    {
	      count = ubicom32command_packet_append(&packet, WRITE_CHECK|OPTION_IGNORE_RESPONSE, ISD_WRITE_MAIL_BOX, (unsigned int)(*data++));
	    }

	  if(length || (ubicom32command_packet_get_free(&packet) == 0))
	    {
	      // Ship the packet for processing
	      ret = ubicom32command_packet_send(&packet);
	      if(ret)
		return ret;
	    }
	  else
	    break;

	  // readjust the packet
	  ubicom32command_packet_init(&packet);
	}

      // We are expecting and echo of WRITE_PGM_MEMORY_DONE
      count = ubicom32command_packet_append(&packet, READ_CHECK|OPTION_EXTENDED_TIMEOUT, ISD_READ_MAIL_BOX, 0);
      commandEchoIndex = count;

      // Ship the packet for processing
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	return ret;

      // Test the reply
      command = &packet.commands[commandEchoIndex];

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      if(reply != WRITE_PGM_MEMORY_DONE)
	{
	  printf("Reply 0x%x does not match WRITE_PGM_MEMORY_DONE\n", reply);
	  return -1;
	}

      return 0;
    }
}

int
jumpToPc(unsigned int address)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  ubicom32command_packet_init(&packet);

  /* send down JUMP_TO_PC */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, JUMP_TO_PC);

  /* send down the address */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, address);

  /* wait for JUMP_TO_PC_DONE */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[2];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != JUMP_TO_PC_DONE)
    {
      printf("Reply 0x%x does not match JUMP_TO_PC_DONE\n", reply);
      return -1;
    }

  return 0;
}

int
singleStep(unsigned int threadNo)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  memset((void *)&mainRegisters, 0, sizeof(mainRegisters));
  ubicom32command_packet_init(&packet);

  /* send down SINGLE_STEP */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, SINGLE_STEP);

  /* send down thread # */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, threadNo);

  /* wait for SINGLE_STEP_DONE */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[2];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != SINGLE_STEP_DONE)
    {
      printf("Reply 0x%x does not match SINGLE_STEP_DONE\n", reply);
      return -1;
    }

  return 0;
}

// Status holds the state of all threads and the processor
int status;

int
stopProcessor(void)
{
  int i, mask;
  int ret, count, statCopy;
  struct ubicom32command *command;
  unsigned int reply;
  struct ubicom32command_packet packet;

  memset((void *)&mainRegisters, 0, sizeof(mainRegisters));

  ubicom32command_packet_init(&packet);

  // stop all the threads
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, READ_STATUS);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  // We need to wait for the reply
  count = 0;

  while(count++ < 100 ) {
	  int data;
	  ret = readStatus(&data);

	  if(ret) {
		  /* some thing crapped out. Issue a reset and return control back to gdb */
		  ret = ubicom32isp_reset();
		  currentThread = 0;
		  return 0;
	  }

	  if(data & M_BOX_OUT_Q_EMPTY) {
		  // Mailbox queue is empty. Sleep for 10ms and try again
		  usleep(10000);
	  }
  }

  if(count == 100) {
	  /* Things look quite dead. */
	  ret = ubicom32isp_reset();
	  currentThread = 0;
	  return 0;
  }


  ubicom32command_packet_init(&packet);

  /* wait for READ_STATUS_DONE */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Read back the status word */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);


  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[0];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != READ_STATUS_DONE)
    {
      printf("Reply 0x%x does not match READ_STATUS_DONE\n", reply);
      return -1;
    }

  command++;
  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  status = (int) reply;

  statCopy= status;
  currentThread = 0xffff;

  // scan through status and figure out the currentThread
  for(i=0, mask=0x3; i< 8; i++)
    {
      if(statCopy & mask)
      {
	// enabled thread
	if((statCopy & mask) == 0x1)
	  {
	    // suspended thread
	    continue;
	  }
	else
	  {
	    // enabled and active thread
	    if((statCopy & mask) == 0x2)
	      {
		// Breakpointed thread
		currentThread = i;
		break;
	      }
	    if((dontDebugMask & (1<<i)) == 0)
	      {
		currentThread = i;
	      }
	  }
      }

      statCopy >>=2;
    }

  if(currentThread == 0xffff)
    {
      // all threads appear to be suspended. Set currentThread to be 1
      currentThread = 0;
    }

  return 0;
}

int
restartProcessor(void)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  memset((void *)&mainRegisters, 0, sizeof(mainRegisters));

  ubicom32command_packet_init(&packet);

  // start all the threads
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, START);

  /* wait for START_DONE */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[1];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != START_DONE)
    {
      printf("Reply 0x%x does not match START_DONE\n", reply);
      return -1;
    }

  return 0;
}

int
attachDetachDebugger(unsigned attach, unsigned int cpuType)
{
  int sendExpect;
  int ret, count;

  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  if(attach)
    sendExpect = ATTACH_DEBUGGER;
  else
    sendExpect = DETACH_DEBUGGER;

  ubicom32command_packet_init(&packet);

  // send down the attach/dettach command
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)sendExpect);

  // send down the cpuType
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)cpuType);

  /* wait for DONE */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[2];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != (unsigned int)sendExpect)
    {
      exit(1);
    }

  return 0;
}

void
cleanupBackend(unsigned int cpuType)
{
  int ret;
  memset((void *)&mainRegisters, 0, sizeof(mainRegisters));

  // Inform kernel about disconnecting the debugger
  ret = attachDetachDebugger(0, cpuType);
}

int
whoareyou(unsigned int *reply)
{
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  int count, ret;

  ubicom32command_packet_init(&packet);

  // Send down ISD_CLEAN_MAIL
  count = ubicom32command_packet_append(&packet, OPTION_IGNORE_RESPONSE, ISD_CLEAN_MAIL, 0);

  ret = ubicom32command_packet_send(&packet);

  ubicom32command_packet_init(&packet);

  // stop all the threads
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, WHO_ARE_YOU);

  /* wait for WHO_ARE_YOU_DONE */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Read back the whoAmI word */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    {
      /*
       * our attempt to query the debug kernel has failed
       */
      exit(1);
    }

  command = &packet.commands[1];

  *reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(*reply != WHO_ARE_YOU_DONE)
    {
      /* Incorrect reply */
      printf("Incorrect reply of 0x%x expect 0x%x\n", *reply, WHO_ARE_YOU_DONE);
      exit(1);
    }

  command++;
  *reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  printf("The reply is 0x%x\n", *reply);

  return 0;
}

int
getVersion(void)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;
  int commandEchoIndex;

  ubicom32command_packet_init(&packet);

  /* send down the GET_VERSION command */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, GET_VERSION);

  // We are expecting and echo of  GET_VERSION_DONE
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  commandEchoIndex = count;

  // We expect 2 pieces of data after this
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  // Ship the packet for processing
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  // Test the reply
  command = &packet.commands[commandEchoIndex];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != GET_VERSION_DONE)
    {
#ifdef DEBUG
      printf("Reply 0x%x does not match GET_VERSION_DONE\n", reply);
#endif
      return 0;
    }
  command++;
  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  dkmajor = (reply>>24)&0xff;
  dkminro = (reply>>16)&0xff;
  dkstate = (reply>>8)&0xff;
  dkincrement = reply &0xff;
  dkunique = reply&0xff;

  command++;
  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  dkunique = reply;

  return 0;
}

int
setupForDownload(void)
{
  int ret;
  extern int getFlashDetails(void);

  ret = getFlashDetails();
  if(ret)
    return ret;

  ret = attachDetachDebugger(1, 1);
  if(ret)
    return ret;

  ret = getVersion();
  if(ret)
    return ret;

  if(dkincrement ==0)
    return 0;

  return ret;
}

int
initBackendToBootKernel(unsigned int cpuType)
{
  int ret, count;
  static int getStartSending = 0;
  int num_attempts = 0;
  enum connection_status  connectStatus;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;
  extern int getFlashDetails(void);

  // Open connection to hardware
  while(num_attempts++ <10)
    {
      ret = debugOpen();
      if(ret)
	continue;
      else
	break;
    }

  if(num_attempts > 10)
    {
      printf("Unable to make connection to target. %d\n", num_attempts);
      exit(1);
    }

  num_attempts = 0;
  connectStatus=OPEN;

  /* We are going to sit here and attempt to hook up to the debug kernel by hook or by crook */
  while(num_attempts++ <= 10)
    {
      ret = debugOpen();

      /* debugOpen should never fail */
      if(ret)
	return ret;

      ubicom32command_packet_init(&packet);

      // Send down ISD_CLEAN_MAIL
      count = ubicom32command_packet_append(&packet, OPTION_IGNORE_RESPONSE, ISD_CLEAN_MAIL, 0);

      ret = ubicom32command_packet_send(&packet);

      ubicom32command_packet_init(&packet);

      // send down WHO_ARE_YOU
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, WHO_ARE_YOU);

      /* wait for WHO_ARE_YOU_DONE */
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      /* Read back the whoAmI word */
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      /* Ship the packet down for execution */
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	{
	  /*
	   * our attempt to talk to the debug kernel has failed. Issue a ISD reset and try again
	   * after 300 ms.
	   */
	  if(connectStatus==OPEN||
	     ((connectStatus == CONN_BOOT_LOADER) &&
	      num_attempts == 8))
	    {
	      connectStatus = RESET_ISSUED;
	      debugReset();
	      usleep(700000);
	      continue;
	    }
	  else
	    {
	      ubicom32command_packet_init(&packet);

	      // Send down ISD_OPEN
	      count = ubicom32command_packet_append(&packet, OPTION_IGNORE_RESPONSE, ISD_CLEAN_MAIL, 0);

	      ret = ubicom32command_packet_send(&packet);
	    }

	  /* try again */
	  usleep(500000);
	  continue;
	}

      command = &packet.commands[1];

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      if(reply != WHO_ARE_YOU_DONE)
	{
	  /* reset and try again */
	  debugReset();
	  usleep(700000);
	  continue;
	}

      command++;
      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      printf("The reply is 0x%x\n", reply);
      if(reply == 0xb00ce)
	{
	  connectStatus = CONN_BOOT_LOADER;
	  break;
	}
      else
	{
	  /* we got garbage. Try the reset sequence again */
	  connectStatus = OPEN;
	  debugReset();
	  usleep(700000);
	  continue;
	}
    }

  if(num_attempts > 10)
    {
      printf("There appears to be no debug kernel present\n");
      exit(1);
    }

  // Stop all mp threads
  ret = stopProcessor();
  if(ret)
    return ret;

  ret = getFlashDetails();
  ret = attachDetachDebugger(1, cpuType);
  getVersion();
  single_step_buffer_address = 0;

  if(dkincrement ==0)
    return 0;
  return ret;
}

int
initBackend(unsigned int cpuType)
{
  int ret, count;
  struct ubicom32command *command;
  struct ubicom32command_packet packet;
  unsigned int reply;
  enum connection_status  connectStatus;
  int num_attempts = 0;
  int num_open_attempts = 0;
  extern int getFlashDetails(void);

  // Open connection to hardware
  while(num_attempts++ <10)
    {
#if DEBUG
      printf("IB DOP\n");
#endif

      ret = debugOpen();
      if(ret)
	{
	  usleep(100000);
	  continue;
	}
      else
	break;
    }

  if(num_attempts > 10)
    {
      // printf("Unable to make connection to target. %d\n", num_attempts);
      // return -2 telling upper layer that ISD connection itself is broken
      return -2;
    }

  if(debugging_processor == 5133)
    return -4;

  num_attempts = 0;
  connectStatus=OPEN;

  /* We are going to sit here and attempt to hook up to the debug kernel by hook or by crook */
  while(num_attempts++ <= 10)
    {
      num_open_attempts = 0;
      while(num_open_attempts++< 10){
#if DEBUG
	printf("IB1 DOP\n");
#endif
	ret = debugOpen();

	/* debugOpen should never fail */
	if(ret)
	  {
#ifdef DEBUG
	    printf("Open bombed\n");
#endif
	  }
	else
	  break;

      }

      if(ret)
	return ret;

      ubicom32command_packet_init(&packet);

      // Send down ISD_CLEAN_MAIL
      count = ubicom32command_packet_append(&packet, OPTION_IGNORE_RESPONSE, ISD_CLEAN_MAIL, 0);

      ret = ubicom32command_packet_send(&packet);

      ubicom32command_packet_init(&packet);

      // send down WHO_ARE_YOU
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, WHO_ARE_YOU);

      /* wait for WHO_ARE_YOU_DONE */
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      /* Read back the whoAmI word */
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

#if DEBUG
      printf("ISPR SWAREU\n");
#endif

      /* Ship the packet down for execution */
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	{
	  /*
	   * our attempt to talk to the debug kernel has failed. Issue a ISD reset and try again
	   * after 300 ms.
	   */
	  if(connectStatus==OPEN||
	     ((connectStatus == CONN_BOOT_LOADER) &&
	      num_attempts >= 8))
	    {
#ifdef DEBUG
	      printf("3\n");
#endif
	      if(connectStatus == CONN_BOOT_LOADER)
		{
		  ret = initBackendToBootKernel(1);
		}
	      else
		{
		  if(connectStatus == OPEN)
		    {
#ifdef DEBUG
		      printf("4\n");
#endif
		      connectStatus = RESET_ISSUED;
		      debugReset();
		      usleep(700000);
		    }
		  else
		    usleep(300000);
		}
	      continue;
	    }
	  else
	    {
#ifdef DEBUG
	      printf("2 num att =%d\n", num_attempts);
#endif
	      ubicom32command_packet_init(&packet);

	      // Send down ISD_OPEN
	      count = ubicom32command_packet_append(&packet, OPTION_IGNORE_RESPONSE, ISD_CLEAN_MAIL, 0);

	      ret = ubicom32command_packet_send(&packet);
	    }

	  /* try again */
	  usleep(500000);
	  continue;
	}

      command = &packet.commands[1];

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      if(reply != WHO_ARE_YOU_DONE)
	{
	  /* reset and try again */
	  debugReset();
	  usleep(700000);
	  continue;
	}

      command++;
      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      printf("The reply is 0x%x\n", reply);
      getVersion();
      single_step_buffer_address = 0;
      if(reply == 0xb00ce)
	 printf("Boot Sector Loader Kernel Version %d.%d.%c%d.%d\n", dkmajor, dkminro, dkstate, dkincrement, dkunique);
      else if(reply == 0xabb)
	{
	  printf("Application Debug Kernel Version %d.%d.%c%d.%d\n", dkmajor, dkminro, dkstate, dkincrement, dkunique);

	  /* if the kernel version is greater than or equal to  2.0.D1.6 then we can find the single step buffer address from the 3k */
	  if ((dkmajor >2 ) || ((dkmajor ==2) && (dkunique >= 6))) {
	    (void) getSingleStepBufferAddress();
	  }
	}
      else if(reply == 0xb0dce)
	 printf("Downloader Kernel Version %d.%d.%c%d.%d\n", dkmajor, dkminro, dkstate, dkincrement, dkunique);
      else
	 printf("Unexpected reply\n");

#ifdef DEBUG
      printf("connectStatu = %d\n", connectStatus);
#endif
      if(reply == 0xabb)
	{
	  connectStatus = CONN_APP;
	  break;
	}
      else if(reply == 0xb00ce)
	{
	  if(connectStatus == CONN_BOOT_LOADER)
	    break;
	  connectStatus = CONN_BOOT_LOADER;
#ifdef DEBUG
	  printf("1\n");
#endif
	  num_attempts = 0;
	}
      else
	{
	  /* we got garbage. Try the reset sequence again */
	  debugReset();
	  usleep(700000);
	  continue;
	}

      /* The goal here is to restart and get into the APP debug kernel if possible */
      ret = restartProcessor();
      if(ret)
	{
	  /*
	   * our attempt to talk to the debug kernel has failed. Issue a ISD reset and try again
	   * after 300 ms.
	   */
	  connectStatus = OPEN;
	  debugReset();
	  usleep(700000);
	  continue;
	}

      /* good restart. Sleep for about 0.5 s and Issue the WHO_ARE_YOU_SEQUENCE again */
      usleep(500000);

#if MORE_ROBUST_RESET
      unsigned int resBuf[32];
      unsigned int numEntries;
      numEntries = 8;

      debugOpen();
      ret = hw_monitor(0, 1, 0, 0x80000000, 0x80000000, numEntries, resBuf, NULL);
      while (ret == 0) {
	      /* analyze and see if any other thread other than 0 is executing */
	      int i;
	      for(i=0 ; i< numEntries; i++) {
		      if((resBuf[i] & 0x80000000) &&
			 ((resBuf[i] >> 26) & 0x7))
			      break;
	      }

	      if (i< numEntries)
		      break;

#if DEBUG
	      printf("No Debugger yet.\n");
#endif

	      usleep(1000);
	      ret = hw_monitor(0, 1, 0, 0x80000000, 0x80000000, numEntries, resBuf, NULL);
      }
#endif

      //sleep(2);
      continue;
    }

  if(num_attempts > 10)
    {
      //printf("There appears to be no debug kernel present\n");
      // return -3 indicating that there is no debug kernel
      return -3;
    }

  // Stop all mp threads
  ret = stopProcessor();
  if(ret)
    return ret;

  if(dkincrement == 0)
    return 0;

  ret = attachDetachDebugger(1, cpuType);

  ret = getFlashDetails();

  return ret;
}

void
printTPacketMP(unsigned char *buffer, unsigned int threadNo)
{
  unsigned char *saveptr = buffer+1;
  unsigned char chksum=0;

  // print out d0
  sprintf((char *)buffer,"$T05%x:%08x;",
	  0, (int) mainRegisters.tRegs[threadNo].dr[0]);

  buffer +=strlen((char *)buffer);

  // print out a5, a6, a7
  sprintf((char *)buffer,"%x:%08x;%x:%08x;%x:%08x;",
	  21, (int) mainRegisters.tRegs[threadNo].ar[5],
	  22, (int) mainRegisters.tRegs[threadNo].ar[6],
	  23, (int) mainRegisters.tRegs[threadNo].ar[7]);

  buffer +=strlen((char *)buffer);

  // print out rosr pc
  sprintf((char *)buffer,"%x:%08x;%x:%08x;%x:%08x;%x:%08x;%x:%08x;",
	  30, (int) mainRegisters.tRegs[threadNo].rosr,
	  34, (int) mainRegisters.tRegs[threadNo].threadPc,
	  45, (int) mainRegisters.globals.mt_active,
	  47, (int) mainRegisters.globals.mt_dbg_active,
	  48, (int) mainRegisters.globals.mt_en);

  buffer += strlen((char *) buffer);

  // print the thread number also
  sprintf((char *) buffer, "thread:%x;", threadNo+1);
  buffer += strlen((char *)buffer);

  /* compute the check sum */

  while(saveptr < buffer)
    chksum += *saveptr++;

  buffer[0] = '#';
  buffer[1]= tohex( (chksum>>4) & 0x0f );
  buffer[2]= tohex( chksum & 0x0f );
  buffer[3] = 0;
}

int
createTpacketMP(unsigned char *buffer, unsigned int threadNo)
{
  // Retrieve d0, a5-a7, rosr, pc, mt_active, dbg_active, mt_enable
  // We will need these to construct a T packet
  int ret = gdbMainGetTPacketRegs((int) threadNo, &mainRegisters);
  unsigned int *iptr = (unsigned int *)buffer;

  if(ret)
    {
      printf("error: Could not retrieve registers for thread %d\n", threadNo);
      return ret;
    }

  /* load d0, a5-a7, rosr, pc, mt_active, dbg_active, mt_enable into the buffer */
  *iptr++ = mainRegisters.tRegs[threadNo].dr[0];
  *iptr++ = mainRegisters.tRegs[threadNo].ar[5];
  *iptr++ = mainRegisters.tRegs[threadNo].ar[6];
  *iptr++ = mainRegisters.tRegs[threadNo].ar[7];
  *iptr++ = mainRegisters.tRegs[threadNo].rosr;
  *iptr++ = mainRegisters.tRegs[threadNo].threadPc;
  *iptr++ = mainRegisters.globals.mt_active;
  *iptr++ = mainRegisters.globals.mt_dbg_active;
  *iptr++ = mainRegisters.globals.mt_en;
  return ret;
}

int
createGpacket(unsigned char *buffer, unsigned int cpuType)
{
  int ret;
  extern int createGpacketMP(unsigned char *buffer);

  // Create G packet for MP
  ret = createGpacketMP(buffer);
  return ret;
}

int
createGpacketMP(unsigned char *buffer)
{
  unsigned char chksum=0;
  unsigned char *saveptr = buffer+1;
  int i, ret;
  extern unsigned int  mt_active, mt_dbg_active, mt_en, mt_sched, mt_pri;

  *buffer++ = '$';
  ret= gdbMainGetallRegs((int) currentThread, &mainRegisters);

  if(ret)
    return ret;

  // print out d0-d15
  for(i=0; i<16; i++)
    {
      sprintf((char *)buffer,"%08x",
	      (int) mainRegisters.tRegs[currentThread].dr[i]);

      buffer +=strlen((char *)buffer);
    }

  // print out a0-a7
  for(i=0; i< 8; i++)
    {
      sprintf((char *)buffer,"%08x",
	      (int) mainRegisters.tRegs[currentThread].ar[i]);

      buffer +=strlen((char *)buffer);
    }

  // print out rosr pc
  sprintf((char *)buffer,"%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x",
	  (int) mainRegisters.tRegs[currentThread].acc0_hi,
	  (int) mainRegisters.tRegs[currentThread].acc0_lo,
	  (int) mainRegisters.tRegs[currentThread].mac_rc16,
	  (int) mainRegisters.tRegs[currentThread].source3,
	  (int) mainRegisters.tRegs[currentThread].inst_cnt,
	  (int) mainRegisters.tRegs[currentThread].csr,
	  (int) mainRegisters.tRegs[currentThread].rosr,
	  (int) mainRegisters.tRegs[currentThread].iread_data,
	  (int) mainRegisters.tRegs[currentThread].int_mask0,
	  (int) mainRegisters.tRegs[currentThread].int_mask1,
	  (int) mainRegisters.tRegs[currentThread].threadPc);

  buffer += strlen((char *) buffer);

  sprintf((char *) buffer,"%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x",
	  (int) mainRegisters.globals.chip_id,
	  (int) mainRegisters.globals.int_stat0,
	  (int) mainRegisters.globals.int_stat1,
	  (int) mainRegisters.globals.int_set0,
	  (int) mainRegisters.globals.int_set1,
	  (int) mainRegisters.globals.int_clr0,
	  (int) mainRegisters.globals.int_clr1,
	  (int) mainRegisters.globals.global_ctrl,
	  (int) mainRegisters.globals.mt_active_set,
	  (int) mainRegisters.globals.mt_active_clr);

  buffer += strlen((char *) buffer);

  sprintf((char *) buffer,"%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x",
	  (int) mainRegisters.globals.mt_active,
	  (int) mainRegisters.globals.mt_dbg_active_set,
	  (int) mainRegisters.globals.mt_dbg_active,
	  (int) mainRegisters.globals.mt_en,
	  (int) mainRegisters.globals.mt_hpri,
	  (int) mainRegisters.globals.mt_hrt,
	  (int) mainRegisters.globals.mt_break_clr,
	  (int) mainRegisters.globals.mt_break,
	  (int) mainRegisters.globals.mt_single_step,
	  (int) mainRegisters.globals.mt_min_delay_en);

  buffer += strlen((char *) buffer);

  sprintf((char *) buffer,"%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x",
	  (int) mainRegisters.globals.mt_debug_active_clr,
	  (int) mainRegisters.globals.perr_addr,
	  (int) mainRegisters.globals.dcapt_tnum,
	  (int) mainRegisters.globals.dcapt_pc,
	  (int) mainRegisters.globals.dcapt,
	  (int) mainRegisters.globals.scratchpad0,
	  (int) mainRegisters.globals.scratchpad1,
	  (int) mainRegisters.globals.scratchpad2,
	  (int) mainRegisters.globals.scratchpad3,
	  0);

  buffer += strlen((char *) buffer);

  /* compute the check sum */

  while(saveptr < buffer)
    chksum += *saveptr++;

  buffer[0] = '#';
  buffer[1]= tohex( (chksum>>4) & 0x0f );
  buffer[2]= tohex( chksum & 0x0f );
  buffer[3] = 0;

  // Update some of our internal copies

  mt_active = (int) mainRegisters.globals.mt_active;
  mt_dbg_active = (int) mainRegisters.globals.mt_dbg_active;
  mt_en = (int) mainRegisters.globals.mt_en;
  mt_pri = (int) mainRegisters.globals.mt_hpri;
  mt_sched = (int) mainRegisters.globals.mt_hrt;

  return ret;
}

int
waitForBP(bpReason_t *reason)
{
  int data;
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

#if DEBUG
  printf("WBP\n");
#endif

  // call readStatus to get the state of the Mail Box fifos
  ret = readStatus(&data);
  if(ret)
    {
      /* some thing crapped out. Issue a reset and return control back to gdb */
      ret = ubicom32isp_reset();
      return 1;
    }

  if(data & M_BOX_OUT_Q_EMPTY)
    {
      // Mailbox queue is empty. Return 0 as there is nothing to read
      return 0;
    }

  // There is data to be read. Go get it

  ubicom32command_packet_init(&packet);

  // Go read the data from the mailbox
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return -1;

  command = &packet.commands[0];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply == BREAKPOINT_HIT)
    {
      // got the break point, retrieve the thread No from the mailbox
      ubicom32command_packet_init(&packet);
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      /* Ship the packet down for execution */
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	return ret;

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      return (int)(reply+1);
    }
  else if(reply == DCAPT_ERROR)
    {

      // got dcapt error, retrieve the status and pc from the mailbox
      ubicom32command_packet_init(&packet);
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      /* Ship the packet down for execution */
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	return ret;

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      reason->reason = DCAPT_ERROR;
      reason->addr = reply;

      command++;

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      reason->status = reply;
      // We now know the thread No. It is in the status field
      reply = reason->status & 0x1f;

      return (int)(reply+1);
    }
  else if(reply == PARITY_ERROR)
    {
      // got a parity error, retrieve the error address from the mailbox
      ubicom32command_packet_init(&packet);
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      /* Ship the packet down for execution */
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	return ret;

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      reason->reason = PARITY_ERROR;
      reason->addr = reply;
      if(debuggerThreadNo!= 0)
	return (int)(1);
      else
	return 2;
    }
  else
    {
      /* some thing crapped out. Issue a reset and return control back to gdb */
#if DEBUG
      printf("WBP reset\n");
#endif
      ret = ubicom32isp_reset();
#if DEBUG
      printf("WBP reset exit\n");
#endif
      return 1;

    }
  return 0;
}

int
getDebuggerThreadNo(void)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  ubicom32command_packet_init(&packet);

  // Issue the command to get debugger thread #
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)DEBUGGER_THREAD_NO);

  // the command is echoed back. Go get it
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  // get the thread #
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[1];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != DEBUGGER_THREAD_NO_DONE)
    exit(1);

  command++;

  debuggerThreadNo = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  return 0;
}

int
setDebuggerDontDebugMask(unsigned int mask)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  ubicom32command_packet_init(&packet);

  // Issue the command to Set the dont debug mask
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)DEBUGGER_DONT_DEBUG_MASK);

  // Write out the mask
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)mask);

  // the command is echoed back. Go get it
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[2];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != (unsigned int)DEBUGGER_DONT_DEBUG_MASK_DONE)
    {
      exit(1);
    }

  return 0;
}

int
getDebuggerStaticBpAddrs(int *data, unsigned int numEntries)
{
  int ret, count;
  unsigned int i;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  ubicom32command_packet_init(&packet);

  // Issue the command to retrieve the Thread Pcs
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)GET_STATIC_BP_ADDRS);

  /* the command is echoed back. Go get it */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  // now receive the data from target
  for(i=0; i< numEntries; i++)
    count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[1];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != (unsigned int)GET_STATIC_BP_ADDRS)
    {
      exit(1);
    }
  command++;

  // now receive the data from target
  for(i=0; i< numEntries; i++, command++)
    {
      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;
      *data++ = (int)reply;
    }
  return 0;
}

int
getDebuggerThreadPcs(int *data, unsigned int numEntries)
{
  int ret, count;
  unsigned int i;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  ubicom32command_packet_init(&packet);

  // Issue the command to retrieve the Thread Pcs
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)GET_THREAD_PCS);

  /* the command is echoed back. Go get it */
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  // now receive the data from target
  for(i=0; i< numEntries; i++)
    count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[1];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != (unsigned int)GET_THREAD_PCS_DONE)
    {
      exit(1);
    }
  command++;

  // now receive the data from target
  for(i=0; i< numEntries; i++, command++)
    {
      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;
      *data++ = (int)reply;
    }
  return 0;
}


enum threadState
threadStatus(unsigned int threadNo)
{
  unsigned int mask;

  threadNo--;
  mask = 1<< threadNo;

  if(mt_dbg_active&mask)
    {
      return UNCONTROLLED;
    }
  else if(!(mt_en & mask))
    {
      return DISABLED;
    }
  else if (!(mt_active & mask))
    {
      return SUSPENDED;
    }
  else
    return OK;
}

/* This routine only establishes connection to the dongle */
int
ubicom32isp_connect(char *remote)
{
  struct ubicom32command_packet packet;
  int ret;

  if(dongle_interface_status())
    {
      ubicom32isp_detach();
      dongle_close();
    }

  if(dongle_open(remote) == -1)
    return -1;

  pending_packet = NULL;
  doubleBuffer = 0;

  /* See if we can control the console interface. */
  ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_CONSOLE);
  errorExpected = 1;
  ret = ubicom32command_packet_send(&packet);
  errorExpected = 0;

  if(ret == 0)
    {
      ubipodControlconsole = 1;
    }
  else
    {
      ubipodControlconsole = 0;
      dongle_close();

      if(dongle_open(remote) == -1)
	return -1;
    }

  /* Test and see if we are hooked up to a Ubipod dongle. */
  ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK_UBIPOD);
  errorExpected = 1;
  ret = ubicom32command_packet_send(&packet);
  errorExpected = 0;

  if(ret ==0)
    {
      dongleIsUbipod = 1;
      dongleHasMarsSupport = 1;
      okToDoubleBuffer = 1;

      /* See if we can control the 2 wire interface. */
      ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_2WIRE);
      errorExpected = 1;
      ret = ubicom32command_packet_send(&packet);
      errorExpected = 0;

      if(ret == 0)
	{
	  ubipodControl2wire = 1;
	}
      else
	{
	  ubipodControl2wire = 0;
	  dongle_close();

	  if(dongle_open(remote) == -1)
	    return -1;
	}

      return 0;
    }
  else
    {
      dongleIsUbipod = 0;
      dongle_close();

      if(dongle_open(remote) == -1)
	return -1;
    }

  ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK_MARS);
  errorExpected = 1;
  ret = ubicom32command_packet_send(&packet);
  errorExpected = 0;

  if(ret ==0)
    {
      dongleHasMarsSupport = 1;
      okToDoubleBuffer = 1;
      return 0;
    }
  else
    {
      dongleHasMarsSupport=0;
      dongle_close();

      if(dongle_open(remote) == -1)
	return -1;
    }

  ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK);
  errorExpected = 1;
  ret = ubicom32command_packet_send(&packet);
  errorExpected = 0;

  if(ret ==0) {
    okToDoubleBuffer = 1;
  }
  else
    {
      okToDoubleBuffer = 0;
      dongle_close();
      if(dongle_open(remote) == -1)
	return -1;
    }

  return 0;
}

int
ubicom32isp_close(void)
{
  if (ubipodControlconsole)
    {
      /*
       * call turnOnconsole after disconnect from dongle in ubicom32isp_close()
       */
      int ret = turnOnconsole();
    }

  return (dongle_close());
}

/*
 * This routine will contact the debug kernel and also figure out which thread is the debug thread
 * By the time this is called the serial connection is alive.
 */
int
ubicom32isp_attach(void)
{
  int ret;

  if(dongle_interface_status() == 0)
    return -3;

  // connect to the debug kernel
  if((ret = initBackend(1)))
    return ret;

  // Find the debugger thread number
  if((ret = getDebuggerThreadNo()))
    return ret;

  dontDebugMask=(1<<debuggerThreadNo);
  ret = stopProcessor();
  return 0;
}

int
ubicom32isp_detach(void)
{
  cleanupBackend(1);

  return(restartProcessor());
}

int
ubicom32isp_reset(void)
{
  /* reset the board through the mailbox interface */
  int ret;

  if(dongle_interface_status() == 0)
    {
      printf("No connection to target. Request denied.\n");
      return -1;
    }

  /* reinitialize the backend */
  initBackend(1);

  ret = debugReset();

  /* sleep for .7 s before you attempt a connection */
  usleep(700000);

#ifdef MORE_ROBUST_RESET
  unsigned int resBuf[32];
  unsigned int numEntries;
  numEntries = 8;

  debugOpen();
  ret = hw_monitor(0, 1, 0, 0x80000000, 0x80000000, numEntries, resBuf, NULL);
  while (ret != -6) {
#ifdef DEBUG
	  printf("Live threads\n");
#endif
	  usleep(1000);
	  ret = hw_monitor(0, 1, 0, 0x80000000, 0x80000000, numEntries, resBuf, NULL);
  }
#endif

  /* reinitialize the backend */
  initBackend(1);

  // grab the debggerThreadNo
  (void) getDebuggerThreadNo();
  dontDebugMask=(1<<debuggerThreadNo);
  ret = stopProcessor();
  return 0;
}

#define UBICOM32_ERASE_BLOCK (128*1024)

int
eraseVerifyFlash(unsigned int startAddr, unsigned int verifyLength, EraseProgressCallback proc, void *param)
{
  int ret, count;

  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;
  int commandEchoIndex;
  int blockLength;
  int totalLength = verifyLength;
  int lengthSoFar = 0;

  if(proc)
    {
      /* Call the erase progress process and get it initialized */
      proc(totalLength, lengthSoFar, UBICOM32_ERASE_BLOCK, param);
    }

  while (verifyLength)
    {
      if (verifyLength > UBICOM32_ERASE_BLOCK)
	blockLength = UBICOM32_ERASE_BLOCK;
      else
	blockLength = verifyLength;

      ubicom32command_packet_init(&packet);
      /* send down the ERASE_FLASH command */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, ERASE_FLASH);

      /* send down the start address  */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, startAddr);

      /* Send a verify length */
      count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)(blockLength));

      // Ship the packet for processing
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	return ret;

      while(1)
	{
	  unsigned int data;
	  ret = readStatus(&data);
	  if(ret)
	    {
	      /* some thing crapped out. Issue a reset and return control back to gdb */
	      ret = ubicom32isp_reset();
	      return 1;
	    }

	  if(data & M_BOX_OUT_Q_EMPTY)
	    {
	      // Mailbox queue is empty. Return 0 as there is nothing to read
	      continue;
	    }
	  else
	    break;
	}

      // There is data to be read. Go get it
      ubicom32command_packet_init(&packet);


      // We are expecting and echo of ERASE_VERIFY_DONE and a success/failure indicator
      count = ubicom32command_packet_append(&packet, READ_CHECK|OPTION_EXTENDED_TIMEOUT, ISD_READ_MAIL_BOX, 0);
      commandEchoIndex = count;

      // this is for the return status
      count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

      // Ship the packet for processing
      ret = ubicom32command_packet_send(&packet);
      if(ret)
	return ret;

      // Test the reply
      command = &packet.commands[commandEchoIndex];

      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      if(reply != ERASE_FLASH_DONE)
	{
	  printf("Reply 0x%x does not match ERASE_FLASH_DONE\n", reply);
	    return -1;
	}

      command++;
      reply = (command->operand0<<24) | (command->operand1<<16) |
	(command->operand2<<8) | command->operand3;

      if(reply != 0)
	{
	  printf("Flash Erase failed at address 0x%x\n", reply);
	  return -1;
	}

      startAddr += blockLength;
      verifyLength -= blockLength;
      lengthSoFar += blockLength;

      if(proc)
	{
	  /* Call the erase progress process to show progress. */
	  proc(totalLength, lengthSoFar, UBICOM32_ERASE_BLOCK, param);
	}
    }
  return 0;
}

unsigned int flashMID, flashType, flashSectorSize, flashSize;

int
getFlashDetails(void)
{
  int ret, count;

  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;
  int commandEchoIndex;

  ubicom32command_packet_init(&packet);
  /* send down the GET_FLASH_DETAILS command */
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, GET_FLASH_DETAILS);

  // We are expecting and echo of  GET_FLASH_DETAILS_DONE
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  commandEchoIndex = count;

  // We expect 4 pieces of data after this
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  // Ship the packet for processing
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  // Test the reply
  command = &packet.commands[commandEchoIndex];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != GET_FLASH_DETAILS_DONE)
    {
      printf("Reply 0x%x does not match GET_FLASH_DETAILS_DONE\n", reply);
      return -1;
    }

  command++;
  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  flashMID = reply;

  command++;
  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  flashType = reply;

  command++;
  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  flashSectorSize = reply;

  command++;
  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  flashSize = reply;

#ifdef DEBUG
  printf("Flash Details :- MID = 0x%x Type 0x%x SectorSize 0x%x Size = 0x%x\n",
	 flashMID, flashType, flashSectorSize, flashSize);
#endif
  return 0;
}

#if 0
enum hw_monitor_command {
  ISD_WR_MME = 0xb,
  ISD_WR_MEC,
  ISD_WR_MEP,
  ISD_WR_MRC,
  ISD_WR_MOA,
  ISD_WR_MBRE,
  ISD_WR_MEM,
  ISD_RD_MST
};
#endif

int
hw_monitor(unsigned int addr, unsigned int needEvent,
	   unsigned int triggerEvent, unsigned int eventPattern,
	   unsigned int eventMask,
	   unsigned int runCounter, unsigned int *resBuf, char *args)
{
  int ret, count;
  unsigned int stat, numTimes;

  struct ubicom32command_packet packet;
  struct ubicom32command *command;

  if(dongle_interface_status() == 0)
    {
      if(args == NULL)
	return -3;
      ubicom32isp_connect(args);

      if(dongle_interface_status() == 0)
	return -4;

      ret = debugOpen();
      if(ret)
	return ret;
    }

  ubicom32command_packet_init(&packet);

  // Initialize by writing 0 to MME register
  count = ubicom32command_packet_append(&packet, 0, ISD_WR_MME, 0);

  // Write the Memory Object Address (MOA) register
  count = ubicom32command_packet_append(&packet, 0, ISD_WR_MOA, addr);

  if(needEvent)
    {
      // Write Event Pattern register
      count = ubicom32command_packet_append(&packet, 0, ISD_WR_MEP, eventPattern);

      // Write the Memory Event Mask register
      count = ubicom32command_packet_append(&packet, 0, ISD_WR_MEM, eventMask);

      // Write the Memory Event Control register with Event Enable (bit 15) on
      triggerEvent |= 0x8000;
    }
  else
    triggerEvent = 0;

  //Writeto Memory event Control Register
  count = ubicom32command_packet_append(&packet, 0, ISD_WR_MEC, triggerEvent);

  // Set the Monitor Run Counter register
  count = ubicom32command_packet_append(&packet, 0, ISD_WR_MRC, runCounter);

  // Start monitoring by writing a 1 to Monitor Mode Enable
  count = ubicom32command_packet_append(&packet, 0, ISD_WR_MME, 1);

  // Ship this packet down to get monitoring going
  // Ship the packet for processing
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  numTimes = 0;
  while(1)
    {
      // Monitor the status
      ubicom32command_packet_init(&packet);

      // Send down ISD_RD_MST
      count = ubicom32command_packet_append(&packet, 0, ISD_RD_MST, 0);

      ret = ubicom32command_packet_send(&packet);
      if(ret)
	return ret;

      command = &packet.commands[0];

      // extract status
      stat = (command->operand0<<24) | (command->operand1<<16) | (command->operand2<<8) | command->operand3;

      if(stat &1)
	break;

      numTimes++;
      if(numTimes > 2)
	break;
    }

  if(numTimes > 2)
    return -6;
#ifdef DEBUG
  printf("status returned = 0x%08x\n", stat);
#endif

  ubicom32command_packet_init(&packet);

  if(runCounter > 32)
    runCounter = 32;


  for(numTimes=0; numTimes< runCounter; numTimes++)
    {
      // send down ISD_WR_MBRE command
      count = ubicom32command_packet_append(&packet, 0, ISD_WR_MBRE, 0);
    }

  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[runCounter-1];

  for(numTimes=0; numTimes< runCounter; numTimes++, command--)
    {
      stat = (command->operand0<<24) | (command->operand1<<16) | (command->operand2<<8) | command->operand3;

      *resBuf++ = stat;
    }

  return 0;
}

int
protectBootSector(void)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  ret = initBackendToBootKernel(1);
  ubicom32command_packet_init(&packet);

  // Issue the command to get debugger thread #
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)PROTECT_SECTOR);

  // the command is echoed back. Go get it
  count = ubicom32command_packet_append(&packet, READ_CHECK|OPTION_EXTENDED_TIMEOUT, ISD_READ_MAIL_BOX, 0);
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[1];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != PROTECT_SECTOR_DONE)
    exit(1);

  command++;

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply)
    {
      printf("Protect boot sector failed\n");
    }

  return 0;
}

int
unprotectBootSector(void)
{
  int ret, count;
  struct ubicom32command_packet packet;
  struct ubicom32command *command;
  unsigned int reply;

  //ret = initBackendToBootKernel(1);
  ubicom32command_packet_init(&packet);

  // Issue the command to get debugger thread #
  count = ubicom32command_packet_append(&packet, WRITE_CHECK, ISD_WRITE_MAIL_BOX, (unsigned int)UNPROTECT_SECTOR);

  // the command is echoed back. Go get it
  count = ubicom32command_packet_append(&packet, READ_CHECK|OPTION_EXTENDED_TIMEOUT, ISD_READ_MAIL_BOX, 0);
  count = ubicom32command_packet_append(&packet, READ_CHECK, ISD_READ_MAIL_BOX, 0);

  /* Ship the packet down for execution */
  ret = ubicom32command_packet_send(&packet);
  if(ret)
    return ret;

  command = &packet.commands[1];

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply != UNPROTECT_SECTOR_DONE)
    exit(1);

  command++;

  reply = (command->operand0<<24) | (command->operand1<<16) |
    (command->operand2<<8) | command->operand3;

  if(reply)
    {
      printf("Unprotect boot sector failed\n");
    }

  return 0;
}

int
ubicom32_write_bytes (unsigned int address, char *buffer, unsigned int length)
{
  // split the transfer into lead, aligned middle and end
  unsigned truncBytes = (address & 0x3);
  unsigned truncAddress = address & ~0x3;
  unsigned int leadBytes, leadAddr, midBytes, midAddr, endBytes, endAddr, leadIndex;
  unsigned toWriteWords;
  unsigned char  *sendBuffer;
  unsigned char *freeBuffer;
  unsigned char *cptr;
  unsigned int i;
  int j, ret;
  char printbuffer[200];

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
      switch (leadAddr >> 28)
	{
	  case 0x0:
	    // a data section read
	    ret = readDataMemory(leadAddr, 1, (int *)cptr);
	    break;
	  case 0x2:
	    // flash read => assume neutral write
	    *(int *)cptr = -1;
	    break;
	  case 0x4:
	    // a pram section read
	    ret = readPgmMemory(leadAddr, 1, (int *)cptr);
	    break;
	  default:
	    sprintf(printbuffer, "unknown address: 0x%08x\n", leadAddr);
	    dongle_error(printbuffer);
	}

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
      switch (endAddr >> 28)
	{
	  case 0x0:
	    // a data section read
	    ret = readDataMemory(endAddr, 1, (int *)cptr);
	    break;
	  case 0x2:
	    // flash read => assume neutral write
	    *(int *)cptr = -1;
	    break;
	  case 0x4:
	    // a pram section read
	    ret = readPgmMemory(endAddr, 1, (int *)cptr);
	    break;
	  default:
	    sprintf(printbuffer, "unknown address: 0x%08x\n", endAddr);
	    dongle_error(printbuffer);
	}

      for(i=0; i< endBytes; i++)
	{
	  cptr[3-i] = (unsigned char) *buffer++;
	}
    }

  // Send the data to target
  if(leadAddr < 0x20000000)
    {
      // a data section write
      ret = writeDataMemory(leadAddr, toWriteWords, (int *)sendBuffer);
    }
  else
    {
      // a code section write
      ret = writePgmMemory(leadAddr, toWriteWords, (int *)sendBuffer);
    }

  free (freeBuffer);
  return (leadBytes + midBytes + endBytes);
}

int
common_read_bytes_raw (unsigned int address, char *buffer, unsigned int length)
{
  // split the transfer into lead, aligned middle and end
  unsigned truncBytes = (address & 0x3);
  unsigned truncAddress = address & ~0x3;
  unsigned int leadBytes, leadAddr, midBytes, midAddr, endBytes, endAddr, leadIndex;
  unsigned toReadWords;
  unsigned char  *recvBuffer;
  unsigned char *freeBuffer;
  unsigned char *cptr;
  unsigned int i;
  int j, ret;

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

  // Read the words from flash
  ret = readPgmMemory(leadAddr, toReadWords, (int *)recvBuffer);

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

int
common_verify_bytes (unsigned int address, char *buffer, unsigned int length)
{
  // split the transfer into lead, aligned middle and end
  unsigned truncBytes = (address & 0x3);
  unsigned truncAddress = address & ~0x3;
  unsigned int leadBytes, leadAddr, midBytes, midAddr, endBytes, endAddr, leadIndex;
  unsigned toVerifyWords;
  unsigned char cptr[4];
  unsigned int i;
  int ret;

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
      ret = readPgmMemory(leadAddr, 1, (int *)cptr);
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
      ret = readPgmMemory(endAddr, 1, (int *)cptr);
      for(i=0; i< endBytes; i++)
	{
	  if (cptr[3-i] != (unsigned char) *buffer++)
	    return -1;
	}
    }

  return (leadBytes + midBytes + endBytes);
}

int
turnOff2wire(void)
{
  struct ubicom32command_packet packet;
  int ret = 0;

  if (ubipodControl2wire)
    {
      /* Turn off the 2 wire interface. */
      ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_2WIRE);
      ret = ubicom32command_packet_send(&packet);
    }
  return ret;
}

int
turnOn2wire(void)
{
  struct ubicom32command_packet packet;
  int ret = 0;

  if (ubipodControl2wire)
    {
      /* Turn on the 2 wire. */
      ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_2WIRE);
      ret = ubicom32command_packet_send(&packet);
    }
  return ret;
}

int
turnOffconsole(void)
{

  struct ubicom32command_packet packet;
  int ret = 0;

  if (ubipodControlconsole == 0)
    return ret;

  /* Turn off console interface. */
  ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_CONSOLE);

  ret = ubicom32command_packet_send(&packet);

  return ret;
}

int
turnOnconsole(void)
{
  struct ubicom32command_packet packet;
  int ret = 0;

  if (ubipodControlconsole == 0)
    return ret;

  /* Turn on console interface. */
  ubicom32command_packet_init_opcode(&packet, ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_CONSOLE);

  ret = ubicom32command_packet_send(&packet);
  return ret;
}
