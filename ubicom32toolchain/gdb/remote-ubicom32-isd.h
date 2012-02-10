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
/* Main processor kernel Debugger API defines */
#define BREAKPOINT_HIT		0x00000001

#define READ_STATUS 		0x00000002
#define READ_STATUS_DONE 	READ_STATUS

#define READ_DATA_MEMORY	0x00000003
#define READ_DATA_MEMORY_DONE 	READ_DATA_MEMORY

#define WRITE_DATA_MEMORY	0x00000004
#define WRITE_DATA_MEMORY_DONE 	WRITE_DATA_MEMORY

#define READ_REGISTERS 		0x00000005
#define READ_REGISTERS_DONE 	READ_REGISTERS

#define WRITE_REGISTERS 	0x00000006
#define WRITE_REGISTERS_DONE 	WRITE_REGISTERS

#define READ_PGM_MEMORY 	0x00000007
#define READ_PGM_MEMORY_DONE 	READ_PGM_MEMORY

#define WRITE_PGM_MEMORY 	0x00000008
#define WRITE_PGM_MEMORY_DONE 	WRITE_PGM_MEMORY

#define READ_FLASH_MEMORY 	0x00000009
#define READ_FLASH_MEMORY_DONE 	READ_FLASH_MEMORY

#define WRITE_FLASH_MEMORY 	0x0000000a
#define WRITE_FLASH_MEMORY_DONE WRITE_FLASH_MEMORY

#define ERASE_FLASH_MEMORY 	0x0000000b
#define ERASE_FLASH_MEMORY_DONE ERASE_FLASH_MEMORY

#define COPY_DATA_MEMORY 	0x0000000c
#define COPY_DATA_MEMORY_DONE 	COPY_DATA_MEMORY

#define STOP 			0x0000000d
#define STOP_DONE 		STOP

#define START 			0x0000000e
#define START_DONE 		START

#define SINGLE_STEP 		0x0000000f
#define SINGLE_STEP_DONE 	SINGLE_STEP

#define GET_COPRO_REG		0x00000010
#define GET_COPRO_REG_DONE	GET_COPRO_REG

#define SET_COPRO_REG		0x00000011
#define SET_COPRO_REG_DONE	SET_COPRO_REG

#define STOP_COPRO		0x00000012
#define STOP_COPRO_DONE		STOP_COPRO

#define START_COPRO		0x00000013
#define START_COPRO_DONE	START_COPRO

#define ATTACH_DEBUGGER		0x00000014
#define ATTACH_DEBUGGER_DONE	ATTACH_DEBUGGER

#define DETACH_DEBUGGER		0x00000015
#define DETACH_DEBUGGER_DONE	DETACH_DEBUGGER

#define GET_COPRO_TREGS		0x00000016
#define GET_COPRO_TREGS_DONE	GET_COPRO_TREGS

#define DEBUGGER_THREAD_NO	0x00000017
#define DEBUGGER_THREAD_NO_DONE DEBUGGER_THREAD_NO

#define DEBUGGER_DONT_DEBUG_MASK	0x00000018
#define DEBUGGER_DONT_DEBUG_MASK_DONE	DEBUGGER_DONT_DEBUG_MASK

#define GET_STATIC_BP_ADDRS	0x00000019
#define GET_STATIC_BP_ADDRS_DONE	GET_STATIC_BP_ADDRS

#define GET_THREAD_PCS	0x0000001a
#define GET_THREAD_PCS_DONE	GET_THREAD_PCS

#define ERASE_FLASH	0x0000001b
#define ERASE_FLASH_DONE	ERASE_FLASH

#define WHO_ARE_YOU	0x0000001c
#define WHO_ARE_YOU_DONE	WHO_ARE_YOU

#define PROTECT_SECTOR	0x0000001d
#define PROTECT_SECTOR_DONE	PROTECT_SECTOR

#define BAD_COMMAND 0x0000001e

#define GET_FLASH_DETAILS 0x0000001f
#define GET_FLASH_DETAILS_DONE GET_FLASH_DETAILS

#define DCAPT_ERROR	0x00000020
#define PARITY_ERROR	0x00000021

#define GET_VERSION 	0x00000022
#define GET_VERSION_DONE	GET_VERSION

#define UNPROTECT_SECTOR	0x00000023
#define UNPROTECT_SECTOR_DONE	UNPROTECT_SECTOR

#define CRC_PGM_MEMORY		0x00000024
#define CRC_PGM_MEMORY_DONE	CRC_PGM_MEMORY

#define JUMP_TO_PC		0x00000025
#define JUMP_TO_PC_DONE		JUMP_TO_PC

#define GET_SGL_STP_BUF		0x00000026
#define GET_SGL_STP_BUF_DONE	GET_SGL_STP_BUF

#define TRAP			0x00000027
#define HALT			0x00000028

#define START_SENDING		0xffccdde2

/* Mail Box Queue Status bit defines XXXX Probably subject to change*/
#define M_BOX_IN_Q_EMPTY	0x40000000
#define M_BOX_IN_Q_FULL		0x80000000
#define M_BOX_OUT_Q_EMPTY	0x10000000
#define M_BOX_OUT_Q_FULL	0x20000000


#define ISPISD_PACKET_MAGIC 0x123455AA
#define ISPISD_OPCODE_UBICOM32COMMAND 0x00000002
#define ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK 0x00000003
#define ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX 0x00000004
#define ISPISD_OPCODE_UBICOM32COMMAND_WRITE_MAILBOX 0x00000005
#define ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK_MARS 0x00000006
#define ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX_UBICOM32V3 0x00000007
#define ISPISD_OPCODE_UBICOM32COMMAND_READ_MAILBOX_UBICOM32 0x00000008
#define ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_CHECK_UBIPOD 0x00000009
#define ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_2WIRE 0x0000000A
#define ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_2WIRE 0x0000000B
#define ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_STOP_CONSOLE 0x0000000C
#define ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_START_CONSOLE 0x0000000D
#define ISPISD_OPCODE_UBICOM32COMMAND_DONGLE_UNCLOG_MAILBOX 0x0000000E


struct ubicom32ispisd_packet {
	unsigned int magic;
	unsigned int length;
	unsigned int opcode;
	unsigned int operand;
};

/*
 * ubicom32
 */
#define UBICOM32COMMAND_MAX_COMMANDS_PER_PACKET 128
#define UBICOM32COMMAND_TIMEOUT_NORMAL 100

struct ubicom32command {
	unsigned char reserved0;
	unsigned char reserved1;
	unsigned char options;
	unsigned char opcode;
	unsigned char operand0;
	unsigned char operand1;
	unsigned char operand2;
	unsigned char operand3;
};

struct ubicom32command_packet {
	struct ubicom32ispisd_packet header;
	struct ubicom32command commands[UBICOM32COMMAND_MAX_COMMANDS_PER_PACKET];
};

#define WRITE_CHECK	0x08
#define READ_CHECK	0x10
#define IBUF_CHECK	0x20
//#define IBUF_CHECK	0x0
#define OPTION_FORCE 0x01
#define OPTION_IGNORE_RESPONSE 0x04
#define OPTION_EXTENDED_TIMEOUT 0x2

extern void retrieve_pending_packet(void);
extern int ubicom32ispisd_packet_send (struct ubicom32ispisd_packet *packet);
extern void ubicom32command_packet_init (struct ubicom32command_packet *packet);
extern void ubicom32command_packet_init_opcode (struct ubicom32command_packet *packet, unsigned int opcode);
extern int ubicom32command_packet_send (struct ubicom32command_packet *packet);
extern int ubicom32command_packet_append (struct ubicom32command_packet *packet,
				      unsigned char options, unsigned char opcode,
				      unsigned int operand);
extern int ubicom32command_packet_get_next (struct ubicom32command_packet *packet);
extern int ubicom32command_packet_get_free (struct ubicom32command_packet *packet);
extern int debugOpen(void);
extern int debugReset(void);
extern int debugClose(void);
extern int readStatus(int *status); /* Isses ISD_MAIL_STATUS command */
extern int readMailbox(int *mboxData);
extern void ubicom32_erase_progress_callback(int total_length, int current_length, int block_size, void *param);
extern int turnOff2wire(void);
extern int turnOn2wire(void);
extern int turnOffconsole(void);
extern int turnOnconsole(void);
enum threadState {
  OK,
  UNCONTROLLED,
  COPRODEAD,
  DISABLED,
  SUSPENDED,
};

enum connection_status {
  OPEN,
  RESET_ISSUED,
  CONN_BOOT_LOADER,
  CONN_APP,
  CONN_BOOT_LOADER2,
};

typedef struct addrBounds {
  unsigned int lower;
  unsigned int upper;
  unsigned int type;
} addrBounds_t;

typedef struct rangeHiInfo {
  unsigned int num_i_ranges;
  unsigned int num_d_ranges;
} rangeHiInfo_t;

typedef struct FlashBlock_s {
  unsigned int blockNumber;	/* Flash block number 0 - 0xff */
  unsigned int *code;		/* Pointer to the code block */
  unsigned short numBreakpoints; /* Number of breakpoints in this block */
  unsigned short bpListSpace;	/* How much have we allocated for the breakpointList */
  unsigned int *breakpointList; /* Pointer to the breakpoint list. Holds the BP address */
  unsigned short numRemoveBPs;	/* Number of removed BPS */
  unsigned short bpListRemoveSpace; /* Space allocated for removed bps */
  unsigned int *breakpointRemoveList;	/* Pointer to removed BP list */
  struct FlashBlock_s *next;	/* Pointer to the next flash block */
  unsigned char flashBlockGDBState; /* 0 = new 1= Insert 2= Remove */
  unsigned char flashState;	/* 0 = new Bps 1= BPs Installed 2= No BPs */
  unsigned int numWrites;	/* Keep track of number of times this block got written */
} FBlock_t;

extern FBlock_t *firstFlashBlock, *lastFlashBlock;
extern unsigned int ubicom32WriteFlashBlock(FBlock_t * );

enum ubicom32v3_state {
  UBICOM32V3_RUNNING,
  UBICOM32V3_STOPPED,
  UBICOM32V3_DETACHED,
  UBICOM32V3_STEPPING,
  UBICOM32V3_STEPPING_PROBLEMS,
  UBICOM32V3_TRAP_ERROR,
  UBICOM32V3_CORE_STOPPED,
};

extern enum ubicom32v3_state remote_ubicom32v3_state;

struct ubicom32_cpu 
{
  void *registers;		/* Pointer to the register structure. */
  int reg_area_size;		/* Size of the Register save area */
  int num_threads;		/* # of hardware threads. */
  int num_per_thread_regs;	/* # of per thread registers */
  int num_global_regs;		/* # of global registers */
  int core_magic;		/* Core Magic number */
  int *register_map_table;	/* GDB index to Register Address Mapping table. */
  int (*fetch_registers_fn)(struct ubicom32_cpu *cpu, int threadNo); /* Function to retrieve all registers from hardware */
  int (*fetch_tpacket_regs_fn)(struct ubicom32_cpu *cpu, int threadNo); /* Function to retrieve T Packet registers. */
  void (*supply_register_fn)(struct ubicom32_cpu *cpu, int threadNo, void *regcache); /* Function to supply register data to GDB front end. */
  void (*supply_tpacket_regs_fn)(struct ubicom32_cpu *cpu, int threadNo); /* Function to supply T Packet register data to GDB front end. */
  int (*write_register_fn)(int threadNo, int regIndex, int *inBuf, unsigned int length); /* Function to write to a cpu register real/core */
  void (*read_core_fn)(struct ubicom32_cpu *cpu);	/* Function to retrive core file data */
  void (*write_core_fn)(struct ubicom32_cpu *cpu); /* Function to write state to core. */
  void (*create_register_note_data_fn)(struct ubicom32_cpu *cpu, int threadNo, void *buffer); /* Create note section entry from register data */
  addrBounds_t *address_map;	/* CPU address map. */
  int num_address_map_entries;		/* Number of address map_entries */
  unsigned int dcache_control_status_reg;
  unsigned int hrt_start, hrt_end;
  unsigned int flash_start, flash_end;
  unsigned int ocm_start, ocm_end;
  unsigned int ddr_start, ddr_end;
  void *register_info;
  unsigned int mailbox_base, dcache_control_base, icache_control_base, timer_base, ocp_base;
  unsigned int debug_pc;
  int (*setupDebuggerThread_fn)(unsigned int threadNo);
  int (*restoreDebuggerThread_fn)(unsigned int threadNo);
  unsigned int pc_regindex;
  unsigned int trap_cause_index;
  rangeHiInfo_t *range_info;
  unsigned int (*get_sdram_limit_fn)(void);
};

extern struct ubicom32_cpu *current_cpu;
extern struct ubicom32_cpu ubicom32_cpus[];
