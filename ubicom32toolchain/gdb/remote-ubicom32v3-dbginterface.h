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
#ifdef __cplusplus
extern "C" {
#endif
extern int ubicom32v3currentThread;
extern int ubicom32v3_current_thread;
extern int ubicom32DebuggerAssistantThread;
extern int ubicom32v3singleStep(unsigned int threadNo);
extern int ubicom32v3stopProcessor(void);
extern int ubicom32v3restartProcessor(unsigned int scratch3value);
extern int ubicom32v3writeRegisters( int threadNo, int regIndex, 
					 int *inBuf, unsigned int length);
extern int ubicom32v3readRegisters( int threadNo, int regIndex, int *dest, 
			  unsigned int length);
extern int ubicom32v3createTpacketMP(unsigned int threadNo);
extern int ubicom32v3createGpacket(unsigned char *buffer, unsigned int cpuType);
extern int ubicom32v3waitForBP(bpReason_t *reason);
extern int ubicom32v3waitForBPNoTrap(bpReason_t *reason);
extern int ubicom32v3writeMemory(unsigned int destAddr, unsigned int len, int *data);
extern int ubicom32v3readMemory(unsigned int destAddr, unsigned int len, int *data);
extern int ubicom32v3fastWriteMemory(unsigned int destAddr, unsigned int len, int *data);
extern int ubicom32v3UltrafastWriteMemory(unsigned int destAddr, unsigned int len, int *data);
extern int ubicom32v3fastReadMemory(unsigned int destAddr, unsigned int len, int *data);
extern int ubicom32v3UltraFastReadMemory(unsigned int destAddr, unsigned int len, int *data);
extern int ubicom32v3UltraFastReadMemoryRaw(unsigned int destAddr, unsigned int len, int *data);
//extern int crcPgmMemory(unsigned int destAddr, unsigned int len, int *data);

extern int ubicom32v3isp_connect(char *remote);
extern int ubicom32v3isp_attach(void);
extern int ubicom32v3isp_close(void);
extern int ubicom32v3isp_reset(void);
extern int ubicom32v3gdbGetallRegs(int threadNo, ubicom32v3Regs_t *mainRegs);
extern int ubicom32v3isp_detach(void);
extern int ubicom32v3waitForBP(bpReason_t *reason);
extern int ubicom32v3singleStep(unsigned int threadNo);
extern int ubicom32v3cacheflushinvalidate(unsigned int startAddr, unsigned int length);
extern int ubicom32v3cacheinvalidate(unsigned int startAddr, unsigned int length);
extern int ubicom32v3setupDebuggerThread(unsigned int threadNo);
extern int ubicom32v3restoreDebuggerThread(unsigned int);
extern int ubicom32v3isp_bist(void);
extern int ubicom32v3isp_pll_init(void);
extern int ubicom32v3isp_download_fast_transfer_code(unsigned int address);
extern int ubicom32v3isp_force_run_cpu(unsigned int address);
extern int ubicom32v3isp_wakeup_downloader(void);
extern int ubicom32v3isp_make_thread0_alive(void);
extern int ubicom32v3hw_monitor_setup(unsigned int addr, unsigned int needEvent,
				unsigned int triggerEvent, unsigned int eventPattern, 
				unsigned int eventMask, unsigned int runCounter);
extern int ubicom32v3isp_hw_monitor_status(unsigned int *status, 
				     unsigned int *resBuf, unsigned int runCounter);
extern int ubicom32v3isp_initBackendToBootKernel( unsigned int downloader_len, int *downloader_data, unsigned int vma);

extern int ubicom32v3_write_flash_bytes (unsigned int address, char *buffer, int length);
extern int ubicom32v3_verify_bytes (unsigned int address, char *buffer, unsigned int length);
extern int ubicom32v3StopWatchdog(void);
extern int ubicom32v3DisableLeakageEnable(void);
extern int ubicom32v3RestartWatchdog(void);
extern int ubicom32v3RestoreLeakageEnable(void);
extern void ubicom32PatchDebugCode(void);
extern int ubicom32v5ReadMMU (void);
extern unsigned int *ubicom32v5ReadPGD_1 (unsigned int pgd_addr, unsigned int *pgd);
extern unsigned int *ubicom32v5ReadPGD (unsigned int threadNo);
extern unsigned int ubicom32v5pgd_table[PGD_TABLE_SIZE];
extern ubicom32v5PTE_t *ubicom32v5ReadPTE (unsigned int pmd_addr);
extern unsigned int ubicom32v5translate_virtmem (unsigned int addr);

#ifdef __cplusplus
}
#endif
