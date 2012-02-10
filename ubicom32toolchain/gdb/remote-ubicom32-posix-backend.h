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
typedef unsigned int u32_t;

struct posix_context {
  /*
   * Data Registers
   */
  u32_t dn[16];

  /*
   * Address Registers
   */
  u32_t an[8];

  /*
   * Per thread misc registers.
   */
  u32_t acc0_hi;
  u32_t acc0_lo;
  u32_t mac_rc16;
  u32_t acc1_hi;
  u32_t acc1_lo;
  u32_t source3;
  u32_t inst_cnt;
  u32_t csr;
  u32_t iread_data;
  u32_t int_mask0;
  u32_t int_mask1;
  u32_t trap_cause;
  u32_t pc;
};

struct ubicom32_posix_process {
  unsigned int proc;
  unsigned int num_pthreads;
  unsigned int pthreads[1024];
  unsigned int current_thread;
  struct posix_context current_thread_context;
  unsigned int target_signal;
  unsigned int target_state;
  unsigned int gdb_event_thread;
};

struct posix_syscall_frame {
	u32_t frame_begin;
	u32_t frame_end;
	u32_t reg_d0;
	u32_t reg_d1;
	u32_t reg_d2;
	u32_t reg_d3;
	u32_t reg_d4;
	u32_t reg_d5;
	u32_t reg_d6;
	u32_t reg_d7;
	u32_t reg_a1;
	u32_t reg_a2;
	u32_t reg_a3;
	u32_t reg_a4;
	u32_t reg_a5;
	u32_t reg_a6;
};

extern int ubicom32_remote_posix_detach(void);
extern int ubicom32_remote_posix_connect(char *remote);
extern int ubicom32_remote_posix_close(void);
extern int ubicom32_remote_posix_attach(unsigned int text_start, unsigned int text_size, unsigned int data_start, 
					unsigned int data_size, struct ubicom32_posix_process *gdb_proc);
extern int ubicom32_remote_fetch_registers(struct ubicom32_posix_process *gdb_proc);
extern int ubicom32_remote_posix_read_memory(unsigned int leadaddr, unsigned int toreadwords, int *recvbuffer);
extern int ubicom32_remote_posix_write_memory(unsigned int leadaddr, unsigned int towritewords, int *sendbuffer);
extern int ubicom32_remote_posix_restart(unsigned int signal);
extern int ubicom32_remote_posix_restart_thread(unsigned int signal, struct ubicom32_posix_process *gdb_proc);
extern int ubicom32_remote_posix_stop(void);
extern int ubicom32_remote_posix_get_gdb_event(struct ubicom32_posix_process *gdb_proc);
extern int ubicom32_remote_posix_get_status(struct ubicom32_posix_process *gdb_proc);
extern int ubicom32_remote_posix_reset(struct ubicom32_posix_process *gdb_proc);
