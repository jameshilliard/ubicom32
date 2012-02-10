// ubicom32bf.h - Hand-written code for the Ubicom32 CPU. -*- C++ -*-

// Copyright (C) 1999, 2000, 2001, 2002 Red Hat Inc.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include <stdlib.h>
#include <time.h>
#include "cgen-cpu.h"
#include "ubicom32bf-desc.h"
#include "ubicom32bf-defs.h"
#include "ubicom32bf-decode.h"

namespace ubicom32bf
{
  class ubicom32bf_cpu_cgen
    {
      // Include cgen generated elements.
#include "ubicom32bf-cpu.h"

      friend class ubicom32bf_mapped_bus;

    public:
      inline void cgen_rtx_error(const char* msg) const
	{
	  cerr << "ubicom32bf-cpu rtx error: " << msg << endl;
	}

      inline void set_alignment_error() { /* nop for now */ }

      SI    get_h_sep(void);
      int CC_CSR_INDEX;// Condition code register index. This will point to UCSR for Version 5+

      static const int DR_ADDR = 0;
      static const int AR_ADDR = 0x80;
      static const int MAC_HI_ADDR = 0xA0;
      static const int MAC_LO_ADDR = 0xA4;
      static const int MAC_RC16_ADDR = 0xA8;
      static const int SRC_3_ADDR = 0xAC;
      static const int CONTEXT_COUNT_ADDR = 0xb0;
      static const int CSR_ADDR = 0xB4;
      static const int UCSR_ADDR = 0xE4;
      static const int ROSR_ADDR = 0xB8;
      static const int IREAD_ADDR = 0xBC;
      static const int IRQ_MASK_0_ADDR = 0xC0;
      static const int IRQ_MASK_1_ADDR = 0xC4;
      static const int PC_ADDR = 0xD0;
      static const int TRAP_CAUSE_ADDR = 0xD4;
      static const int ACC1_HI_ADDR = 0xD8;
      static const int ACC1_LO_ADDR = 0xDC;
      static const int PREVIOUS_PC_ADDR = 0xE0;
      static const int ID_REVISION_ADDR = 0x100;
      static const int IRQ_STATUS_0_ADDR = 0x104;
      static const int IRQ_STATUS_1_ADDR = 0x108;
      static const int INTSET_0_ADDR = 0x114;
      static const int INTSET_1_ADDR = 0x118;
      static const int INTCLR_0_ADDR = 0x124;
      static const int INTCLR_1_ADDR = 0x128;
      static const int GLBL_CTL_ADDR = 0x134;
      static const int MT_ACTIVE_ADDR = 0x138;
      static const int MT_ACTIVE_SET_ADDR = 0x13C;
      static const int MT_ACTIVE_CLR_ADDR = 0x140;
      static const int MT_DBG_ACTIVE_ADDR = 0x144;
      static const int MT_DBG_ACTIVE_SET_ADDR = 0x148;
      static const int MT_ENABLE_ADDR = 0x14C;
      static const int MT_PRIORITY_ADDR = 0x150;
      static const int MT_SCHEDULE_ADDR = 0x154;
      static const int MT_BREAK_ADDR = 0x158;
      static const int MT_BREAK_CLR_ADDR = 0x15C;
      static const int MT_SINGLESTEP_ADDR = 0x160;
      static const int MT_MIN_DELAY_ADDR = 0x164;
      static const int PERR_ADDR_ADDR = 0x16C;
      static const int DC_CAPTURE_ADDR = 0x170;
      static const int DC_CAPTURE_PC_ADDR = 0x174;
      static const int DC_CAPTURE_THREAD_ADDR = 0x178;
      static const int MT_DBG_ACTIVE_CLR_ADDR = 0x17C;
      static const int SCRATCHPAD0_ADDR = 0x180;

      static const int CFG_ADDR = 0x1A0;
      static const int  MT_TRAP_EN_ADDR = 0x1B8;
      static const int  MT_TRAP_ADDR = 0x1BC;
      static const int  MT_TRAP_SET_ADDR = 0x1C0;
      static const int  MT_TRAP_CLR_ADDR = 0x1C4;
      static const int  SEP_ADDR = 0x1C8;

      static const int HRT_TABLE_ADDR_1 = 0x800;
      static const int HRT_TABLE_ADDR_2 = 0x900;

      static const int DR_INDEX = DR_ADDR / 4;
      static const int AR_INDEX = AR_ADDR / 4;
      static const int MAC_HI_INDEX = MAC_HI_ADDR / 4;
      static const int MAC_LO_INDEX = MAC_LO_ADDR / 4;
      static const int TRAP_CAUSE_INDEX = TRAP_CAUSE_ADDR / 4;
      static const int ACC1_HI_INDEX = ACC1_HI_ADDR / 4;
      static const int ACC1_LO_INDEX = ACC1_LO_ADDR / 4;
      static const int PREVIOUS_PC_INDEX = PREVIOUS_PC_ADDR / 4;
      static const int MAC_RC16_INDEX = MAC_RC16_ADDR / 4;
      static const int SRC_3_INDEX = SRC_3_ADDR / 4;
      static const int CONTEXT_COUNT_INDEX = CONTEXT_COUNT_ADDR / 4;
      static const int CSR_INDEX = CSR_ADDR / 4;
      static const int UCSR_INDEX = UCSR_ADDR / 4;
      static const int ROSR_INDEX = ROSR_ADDR / 4;
      static const int IREAD_INDEX = IREAD_ADDR / 4;
      static const int IRQ_MASK_0_INDEX = IRQ_MASK_0_ADDR / 4;
      static const int IRQ_MASK_1_INDEX = IRQ_MASK_1_ADDR / 4;
      static const int PC_INDEX = PC_ADDR / 4;
      static const int ID_REVISION_INDEX = ID_REVISION_ADDR / 4;
      static const int IRQ_STATUS_0_INDEX = IRQ_STATUS_0_ADDR / 4;
      static const int IRQ_STATUS_1_INDEX = IRQ_STATUS_1_ADDR / 4;
      static const int INTSET_0_INDEX = INTSET_0_ADDR / 4;
      static const int INTSET_1_INDEX = INTSET_1_ADDR / 4;
      static const int INTCLR_0_INDEX = INTCLR_0_ADDR / 4;
      static const int INTCLR_1_INDEX = INTCLR_1_ADDR / 4;
      static const int GLBL_CTL_INDEX = GLBL_CTL_ADDR / 4;
      static const int MT_ACTIVE_INDEX = MT_ACTIVE_ADDR / 4;
      static const int MT_ACTIVE_SET_INDEX = MT_ACTIVE_SET_ADDR / 4;
      static const int MT_ACTIVE_CLR_INDEX = MT_ACTIVE_CLR_ADDR / 4;
      static const int MT_DBG_ACTIVE_INDEX = MT_DBG_ACTIVE_ADDR / 4;
      static const int MT_DBG_ACTIVE_SET_INDEX = MT_DBG_ACTIVE_SET_ADDR / 4;
      static const int MT_ENABLE_INDEX = MT_ENABLE_ADDR / 4;
      static const int MT_PRIORITY_INDEX = MT_PRIORITY_ADDR / 4;
      static const int MT_SCHEDULE_INDEX = MT_SCHEDULE_ADDR / 4;
      static const int MT_BREAK_INDEX = MT_BREAK_ADDR / 4;
      static const int MT_BREAK_CLR_INDEX = MT_BREAK_CLR_ADDR / 4;
      static const int MT_SINGLESTEP_INDEX = MT_SINGLESTEP_ADDR / 4;
      static const int MT_MIN_DELAY_INDEX = MT_MIN_DELAY_ADDR / 4;

      static const int PERR_ADDR_INDEX = PERR_ADDR_ADDR / 4;
      static const int DC_CAPTURE_INDEX = DC_CAPTURE_ADDR / 4;
      static const int DC_CAPTURE_PC_INDEX = DC_CAPTURE_PC_ADDR/ 4;
      static const int DC_CAPTURE_THREAD_INDEX = DC_CAPTURE_THREAD_ADDR / 4;
      static const int MT_DBG_ACTIVE_CLR_INDEX = MT_DBG_ACTIVE_CLR_ADDR / 4;
      static const int SCRATCHPAD0_INDEX = SCRATCHPAD0_ADDR/4;
      static const int CFG_INDEX = CFG_ADDR/4;
      static const int MT_TRAP_EN_INDEX = MT_TRAP_EN_ADDR/4;
      static const int MT_TRAP_INDEX = MT_TRAP_ADDR/4;
      static const int MT_TRAP_SET_INDEX = MT_TRAP_SET_ADDR/4;
      static const int MT_TRAP_CLR_INDEX = MT_TRAP_CLR_ADDR/4;
      static const int SEP_INDEX = SEP_ADDR/4;

    protected:
      // context support
      static const int NUM_CONTEXTS = 31;
      static const int SI_SIZE = 4;

      int   context;
      int   sidlibContext;
      int   direct_access;
      SI    register_file[NUM_CONTEXTS][64];
      SI    common_register_file[192];

      // condition codes for jmpcc instruction
      static const int CC_F  = 0;
      static const int CC_NC = 1;
      static const int CC_C  = 2;
      static const int CC_Z  = 3;
      static const int CC_GE = 4;
      static const int CC_GT = 5;
      static const int CC_HI = 6;
      static const int CC_LE = 7;
      static const int CC_LS = 8;
      static const int CC_LT = 9;
      static const int CC_MI = 10;
      static const int CC_NZ = 11;
      static const int CC_PL = 12;
      static const int CC_T  = 13;
      static const int CC_NV = 14;
      static const int CC_V  = 15;


      // register access routines
      SI   get_h_dr (UINT index) const;
      SI   get_h_s1_dr (UINT index) const;
      void set_h_dr (UINT index, SI value);
      void set_h_dr_internal (UINT index, SI value);

      SF get_acc_fpu_32(UINT reg_no) const;
      void set_acc_fpu_32(UINT reg_no, SF newval);
      DF get_acc_fpu_64(UINT reg_no) const;
      void set_acc_fpu_64(UINT reg_no, DF newval);
      SF get_fpu_32_from_data_reg(UINT reg_no) const;
      DF get_fpu_64_from_data_reg(UINT reg_no) const;

      USI  get_h_ar (UINT index) const;
      void set_h_ar (UINT index, USI value);

      SI   get_h_mac_hi () const;
      void set_h_mac_hi (SI value);

      USI  get_h_mac_lo () const;
      void set_h_mac_lo (USI value);

      USI  get_h_trap_cause () const;
      void set_h_trap_cause (USI value);

      SI   get_h_acc1_hi () const;
      void set_h_acc1_hi (SI value);

      USI  get_h_acc1_lo () const;
      void set_h_acc1_lo (USI value);

      USI  get_acc_lo(UINT index) const;
      DI  get_acc(UINT index) const;
  
      SI   get_h_src_3 () const;
      void set_h_src_3 (SI value);

      USI  get_h_previous_pc () const;
      void set_h_previous_pc (USI value);


      static const long CSR_DEST_THREAD_SHIFT = 15;
      static const long CSR_DEST_THREAD_MASK = (0x1f << CSR_DEST_THREAD_SHIFT);
      static const long CSR_DEST_THREAD_SELECT_BIT_MASK = 1 << 14;
      static const long CSR_SOURCE_THREAD_SHIFT = 9;
      static const long CSR_SOURCE_THREAD_MASK = (0x1f << CSR_SOURCE_THREAD_SHIFT);
      static const long CSR_SOURCE_THREAD_SELECT_BIT_MASK = 1 << 8;
      static const long CSR_CLEAR_CC_MASK = 0xffefff00;
      static const long CSR_NBIT_32_MASK = 1 << 7;
      static const long CSR_ZBIT_32_MASK = 1 << 6;
      static const long CSR_VBIT_32_MASK = 1 << 5;
      static const long CSR_CBIT_32_MASK = 1 << 4;
      static const long CSR_NBIT_16_MASK = 1 << 3;
      static const long CSR_ZBIT_16_MASK = 1 << 2;
      static const long CSR_VBIT_16_MASK = 1 << 1;
      static const long CSR_CBIT_16_MASK = 1 << 0;

      static const long CSR_DSP_OBIT_MASK = 1 << 20;
      static const long CSR_PRIVILEGE_MASK = 1 << 21;
      static const long CSR_PREV_PRIVILEGE_MASK = 1 << 22;
	    
      // FPU Rounding modes
      static const long UCSR_FPRM_ROUND_NEAREST = 0;
      static const long UCSR_FPRM_ROUND_ZERO = 1;
      static const long UCSR_FPRM_ROUND_POSITIVE = 2;
      static const long UCSR_FPRM_ROUND_NEGATIVE = 3;
      static const long UCSR_FPRM_SHIFT = 23;

      // FPU Exception sticky bits
      static const long UCSR_FPESB_INVALID = 1 << 15;
      static const long UCSR_FPESB_DIV0 = 1 << 16;
      static const long UCSR_FPESB_OVERFLOW = 1 << 17;
      static const long UCSR_FPESB_UNDERFLOW = 1 << 18;
      static const long UCSR_FPESB_INEXACT = 1 << 19;

      // FPU Exception trap enable bits
      static const long UCSR_FPTE_INVALID = 1 << 25;
      static const long UCSR_FPTE_DIV0 = 1 << 26;
      static const long UCSR_FPTE_OVERFLOW = 1 << 27;
      static const long UCSR_FPTE_UNDERFLOW = 1 << 28;
      static const long UCSR_FPTE_INEXACT = 1 << 29;
      static const long UCSR_FPTE_SHIFT = 25;

      static const long CSR_READ_ONLY_MASK = 0x077dfff;
      USI  get_h_csr () const;
      USI  get_h_csr_c (int context) const;
      void set_h_csr (USI value);
      void set_h_csr_c (USI value, int context);
      USI  get_h_cc_csr () const;
      USI  get_h_cc_csr_c (int context) const;
      void set_h_cc_csr (USI value);
      void set_h_cc_csr_c (USI value, int context);

      void set_h_nbit_16 (BI value);
      void set_h_zbit_16 (BI value);
      void set_h_vbit_16 (BI value);
      void set_h_cbit_16 (BI value);
      void set_h_nbit_32 (BI value);
      void set_h_zbit_32 (BI value);
      void set_h_vbit_32 (BI value);
      void set_h_cbit_32 (BI value);
      void set_h_dsp_obit(BI value);
      void set_h_privbit(BI value);
      void set_h_prev_privbit(BI value);
      
      inline BI get_h_nbit_16 () const
      {
	return (get_h_cc_csr () & CSR_NBIT_16_MASK) != 0;
      }
      inline BI get_h_zbit_16 () const
      {
	return (get_h_cc_csr () & CSR_ZBIT_16_MASK) != 0;
      }
      inline BI get_h_vbit_16 () const
      {
	return (get_h_cc_csr () & CSR_VBIT_16_MASK) != 0;
      }
      inline BI get_h_cbit_16 () const
      {
	return (get_h_cc_csr () & CSR_CBIT_16_MASK) != 0;
      }
      inline BI get_h_nbit_32 () const
      {
	return (get_h_cc_csr () & CSR_NBIT_32_MASK) != 0;
      }
      inline BI get_h_zbit_32 () const
      {
	return (get_h_cc_csr () & CSR_ZBIT_32_MASK) != 0;
      }
      inline BI get_h_vbit_32 () const
      {
	return (get_h_cc_csr () & CSR_VBIT_32_MASK) != 0;
      }
      inline BI get_h_cbit_32 () const
      {
	return (get_h_cc_csr () & CSR_CBIT_32_MASK) != 0;
      }
      inline BI get_h_dsp_obit () const
      {
	return (get_h_cc_csr () & CSR_DSP_OBIT_MASK) != 0;
      }

      inline BI get_h_privbit () const
      {
	return (get_h_csr () & CSR_PRIVILEGE_MASK) != 0;
      }

      inline BI get_h_prev_privbit () const
      {
	return (get_h_csr () & CSR_PREV_PRIVILEGE_MASK) != 0;
      }

      static const long ROSR_CONTEXT_SHIFT = 2;
      static const long ROSR_MEM_BUSY_BIT_MASK = 1 << 1;
      USI  get_h_rosr () const;
      USI  get_h_rosr_c (int context) const;
      void  set_h_rosr_reset(int context);
      void  set_h_rosr_c (USI value, int context);
      void set_h_rosr(USI value);

      USI  get_h_iread () const;
      void set_h_iread (USI value);

      USI  get_h_irq_mask_0 () const;
      USI  get_h_irq_mask_0_c (int context) const;
      void set_h_irq_mask_0 (USI value);

      USI  get_h_irq_mask_1 () const;
      USI  get_h_irq_mask_1_c (int context) const;
      void set_h_irq_mask_1 (USI value);

      int s1_context[NUM_CONTEXTS]; // { 0, 1, 2, 3, 4, 5, 6, 7 };
      int d_context[NUM_CONTEXTS]; // { 0, 1, 2, 3, 4, 5, 6, 7};

      USI  get_h_pc () const;
      void set_h_pc (USI value);
      void set_h_pc_user (USI value);  // direct mem write to pc
      void set_h_pc_c (USI value, int context);

      USI  get_h_irq_status_0 () const;
      USI syscall_trap_num;
      void set_h_irq_status_0 (USI value);

      USI  get_h_irq_status_1 () const;
      static const USI IRQ_1_BKPT_MASK                        = 0x80000000;
      static const USI IRQ_1_MBOX_MASK                        = 0x40000000;
      static const USI IRQ_1_DCAPT_MASK                       = 0x20000000;
      static const USI IRQ_1_MATH_COPROCESSOR_COMPLETION_MASK = 0x10000000;
      static const USI IRQ_1_MATH_COPROCESSOR_EXCEPTION_MASK  = 0x10000000;
      static const USI IRQ_1_RT_TIMER_MASK  		      = 0x08000000;
      static const USI IRQ_1_MEM_PARITY_ERROR_MASK	      = 0x04000000;

      void set_h_irq_status_1 (USI value);

      void set_h_intset_0 (USI value);
      void set_h_intset_1 (USI value);
      void set_h_intclr_0 (USI value);
      void set_h_intclr_1 (USI value);

      SI   get_h_mac_rc16 () const;
      void set_h_mac_rc16 (SI value);

      // global register access routines
      // static USI REVISION_ID_VALUE;
      USI  get_h_id_revision () const;

      static const long GLBL_CTL_INTERRUPT_ENABLE_BIT_MASK = 1 << 0;
      static const long GLBL_CTL_IERASE_ENABLE_BIT_MASK = 1 << 1;
      static const long GLBL_CTL_HRT_TABLE_SELECT_BIT_MASK = 1 << 2;
      static const long GLBL_CTL_MIN_DELAY_SHIFT = 3;
      static const long GLBL_CTL_MIN_DELAY_MASK = 0xf << GLBL_CTL_MIN_DELAY_SHIFT;

      USI  get_h_glbl_ctl () const;
      void set_h_glbl_ctl (USI value);
      int  current_delay;
      USI  get_h_glbl_ctl_interrupt_enable_bit () const;
      USI  get_h_glbl_ctl_ierase_enable_bit () const;
      USI  get_h_glbl_ctl_hrt_table_select_bit () const;

      /* where do these go ? */
      virtual void signal_watchdog_enable_change (bool newval) = 0;

      USI  get_h_mt_active () const;
      void set_h_mt_active (USI value);
      void set_h_mt_active_set (USI mask);
      void set_h_mt_active_clr (USI mask);

      USI  get_h_mt_dbg_active () const;
      void set_h_mt_dbg_active (USI value);
      void set_h_mt_dbg_active_set (USI mask);
      void set_h_mt_dbg_active_clr (USI mask);

      USI  get_h_mt_enable () const;
      void set_h_mt_enable (USI value);
      void suspend_context (int context_num);

      USI  get_h_mt_priority () const;
      void set_h_mt_priority (USI value);

      USI  get_h_mt_schedule () const;
      void set_h_mt_schedule (USI value);

      USI  get_h_mt_break () const;
      void set_h_mt_break (USI value);
      void set_h_mt_break_clr (USI mask);

      USI  get_h_mt_single_step () const;
      void set_h_mt_single_step (USI value);

      USI  get_h_mt_min_delay_enable () const;
      void set_h_mt_min_delay_enable (USI value);

      USI  get_h_dcapt () const;
      void set_h_dcapt (USI value);
      static const USI DCAPT_MISALIGNED_DEST =	0x00000200;
      static const USI DCAPT_MISALIGNED_SRC =	0x00000100;
      static const USI DCAPT_ILLEGAL_DEST =	0x00000080;
      static const USI DCAPT_ILLEGAL_SRC =	0x00000040;
      static const USI DCAPT_ADDRESS_MATCH =	0x00000020;

      mutable USI  set_dcapt_interrupt;
      inline USI get_set_dcapt_interrupt(void) const {return set_dcapt_interrupt;}
      inline void set_set_dcapt_interrupt(USI value)const
        {
	  if(value)
	    set_dcapt_interrupt |= value;
	  else
	    set_dcapt_interrupt = value;
	}

      USI  get_h_dcapt_pc () const;
      void set_h_dcapt_pc (USI value);

      USI  get_h_dcapt_thread () const;
      void set_h_dcapt_thread (USI value);

      USI  get_h_perr_addr () const;
      void set_h_perr_addr (USI value);

      USI   get_h_scratchpad (UINT index) const;
      void set_h_scratchpad (UINT index, USI value);

      USI  get_h_count () const;
      void inc_h_count ();
      void set_h_count (int context_num, USI value);

      // syscalls
      virtual void do_syscall () = 0;

      // low-level virtual memory access routines
      virtual USI read_real_data_word (host_int_4 pc, host_int_4 address) const = 0;
      virtual void write_real_data_word (host_int_4 pc, host_int_4 address, big_int_4 value) const = 0;

      // scheduling support
      static const int HRT_MAX_ENTRIES = 64;
      static const int HRT_BITS_PER_ENTRY = 8;
      static const int HRT_BIT_MASK = (1 << HRT_BITS_PER_ENTRY) - 1;
      static const int HRT_LAST_ENTRY_BIT = 7;
      static const int HRT_LAST_ENTRY_MASK = (1 << HRT_LAST_ENTRY_BIT);
      static const int HRT_OCCUPIED_BIT = 6;
      static const int HRT_OCCUPIED_MASK = (1 << HRT_OCCUPIED_BIT);
      static const int HRT_CONTEXT_MASK = NUM_CONTEXTS - 1;

      static const int MAX_DELAY = HRT_MAX_ENTRIES;

#if 0
      int  hrt_index;
      int  hrt_count;
      char hrt_table[HRT_MAX_ENTRIES];
#endif

      USI  currentHrtBase;	// Base address of the current HRT table either 0x800 or 0x900
      USI  currentHrtSlot;	// HRT Slot that will be scheduled next

      int  srt_index;
      char context_table[NUM_CONTEXTS];
      int  delay_table[NUM_CONTEXTS];
      int  srt_have_hp_threads;
      int  no_active;

#if 0
      virtual void build_hrt_table () = 0;
#endif

      int  is_enabled_m (int mask);
      int  is_active_m (int mask);
      int  is_active (int context);
      int  is_active_srt (int context);
      int  is_active_hp_srt (int context);
      int  is_past_delay (int context);
      void inc_delay_table ();
      inline int  is_empty (char hrt_entry) const 
      { 
	return (hrt_entry & HRT_OCCUPIED_MASK); 
      }
      inline int  is_last (char hrt_entry) const 
      { 
	return (hrt_entry & HRT_LAST_ENTRY_MASK); 
      }
      inline int  get_entry_context (char hrt_entry) const
      {
	return (hrt_entry & HRT_CONTEXT_MASK);
      }
      void try_to_activate (int context);
      int  have_srt_hp_threads ();

      USI gdbAccess;		// This is set when gdb is changing registers Don't Dcapt

    };

  class ubicom32bf_mapped_bus: public sidutil::passthrough_word_bus<sid::big_int_4>
    {
    public:
      ubicom32bf_mapped_bus (sid::bus **t, ubicom32bf_cpu_cgen *cpu) : 
	sidutil::passthrough_word_bus<sid::big_int_4> (t), cpu (cpu), mask (0xffffffff) {}
      ~ubicom32bf_mapped_bus () throw () {}

    private:
      ubicom32bf_cpu_cgen *cpu;

      sid::bus::status word_write(sid::host_int_4 addr,
				  sid::big_int_4 mask,
				  sid::big_int_4 data);
      
      sid::bus::status word_read(sid::host_int_4 addr,
				 sid::big_int_4 mask,
				 sid::big_int_4& data);
      
      int do_direct_passthrough (sid::host_int_4& addr);

      static const int WORD_ALIGN_MASK = ~0x3;
      big_int_4 mask;

      sid::bus::status write(host_int_4 addr, big_int_4 data) throw () \
	  { if (do_direct_passthrough (addr)) \
               return (*target)->write(addr, data); \
               return this->word_write(addr >> 2, mask, data); }
    
      sid::bus::status read(host_int_4 addr, big_int_4& data) throw () \
	  { if (do_direct_passthrough (addr)) \
               return (*target)->read(addr, data); \
	       return this->word_read(addr >> 2 , mask, data); }
    
 };

  class ubicom32bf_cpu: public ubicom32bf_cpu_cgen, public cgen_bi_endian_cpu
    {
    private:
      scache_engine<ubicom32_scache> engine;

      // special buses that handle mapping memory to registers, access protection,
      // and the special virtual memory mapping the debugger uses
      ubicom32bf_mapped_bus mapped_data_bus;
      sid::bus *real_data_bus;

#if 0
      // harvard architecture constants used for mapping single space virtual addresses to harvard
      static const int FIRST_DATA_SECTION = 0x01;
      static const int FIRST_INSN_SECTION = 0x02;
      sidutil::harvard_bus harvard_debugger_bus;
#endif

      // program memory privilege support
      static const int        PAGE_SIZE     = 2048;
      static const int        NUM_PAGE_BITS = 11;
      USI leapdec;		// Flag to know if we are in a lea/pdec instruction

      /* cpu_type is 0 for ip3035, 1 for mars */
      USI cpu_type;
      // global control enable bit change notification
#if 0
      void signal_timer_enable_change (bool newval) { this->timer_enable_pin.drive (newval); }
#endif

      void signal_watchdog_enable_change (bool newval) { this->watchdog_enable_pin.drive (newval); }

      void step_insns ();
      int  scheduler ();
      void execute();
      void flush_icache ();
      void reset ();

      USI read_real_data_word (host_int_4 pc, host_int_4 address) const;
      void write_real_data_word (host_int_4 pc, host_int_4 address, big_int_4 value) const;
      
    public:
      // direct memory access for s1 and d operands
      SI   get_s1_direct_mem (USI addr);
      void set_d_direct_mem (USI addr, SI value);
      void set_d_direct_mem_hi (USI addr, HI value);
      inline void is_lea_pdec(USI value) {this->leapdec = value;}

      // pins
      callback_pin<ubicom32bf_cpu> irq_0_pin;
      void do_irq_0_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_1_pin;
      void do_irq_1_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_2_pin;
      void do_irq_2_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_3_pin;
      void do_irq_3_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_4_pin;
      void do_irq_4_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_5_pin;
      void do_irq_5_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_6_pin;
      void do_irq_6_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_7_pin;
      void do_irq_7_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_8_pin;
      void do_irq_8_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_9_pin;
      void do_irq_9_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_10_pin;
      void do_irq_10_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_11_pin;
      void do_irq_11_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_12_pin;
      void do_irq_12_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_13_pin;
      void do_irq_13_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_14_pin;
      void do_irq_14_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_15_pin;
      void do_irq_15_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_16_pin;
      void do_irq_16_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_17_pin;
      void do_irq_17_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_18_pin;
      void do_irq_18_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_19_pin;
      void do_irq_19_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_20_pin;
      void do_irq_20_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_21_pin;
      void do_irq_21_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_22_pin;
      void do_irq_22_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_23_pin;
      void do_irq_23_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_24_pin;
      void do_irq_24_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_25_pin;
      void do_irq_25_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_26_pin;
      void do_irq_26_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_27_pin;
      void do_irq_27_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_28_pin;
      void do_irq_28_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_29_pin;
      void do_irq_29_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_30_pin;
      void do_irq_30_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_31_pin;
      void do_irq_31_pin (host_int_4 value);
      void do_irq_pin (host_int_4 value, host_int_4 pin_number);
      callback_pin<ubicom32bf_cpu> irq_math_coprocessor_completion_pin;
      void do_irq_math_coprocessor_completion_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> irq_math_coprocessor_exception_pin;
      void do_irq_math_coprocessor_exception_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> mbox_intr_pin;
      void do_mbox_intr_pin (host_int_4 value);
      callback_pin<ubicom32bf_cpu> rttmr_intr_pin;
      void do_rttmr_intr_pin (host_int_4 value);

      /* The watchdog timer is not kept with the cpu so it
	 externally tells the cpu via the watchdog_trigger_pin
	 if rollover has occurred.  If watchdog resets are
	 enabled, then a watchdog reset is indicated via
	 the watchdog_reset_pin. */
      callback_pin<ubicom32bf_cpu> watchdog_trigger_pin;
      void do_watchdog_trigger_pin (host_int_4 value);

      callback_pin<ubicom32bf_cpu> scheduler_pin;
      void do_scheduler_pin (host_int_4 value);

      callback_pin<ubicom32bf_cpu> execute_pin;
      void do_execute_pin (host_int_4 value);


      output_pin watchdog_reset_pin; 
      output_pin watchdog_enable_pin;

#if 0
      output_pin timer_enable_pin;
      output_pin tctrl_t1rst_pin;
      output_pin tctrl_t2rst_pin;
      output_pin tctrl_t3rst_pin;
      output_pin tctrl_t4rst_pin;
      output_pin tctrl_t1ie_pin;
      output_pin tctrl_t2ie_pin;
      output_pin tctrl_t3ie_pin;
      output_pin tctrl_t4ie_pin;
#endif
      // This pin is driven with a non-zero value when the cpu is
      // executing insns. Used by the coprocessor to correctly detect
      // data memory access conflicts.
      output_pin running_pin;

     // debug support routines
      string dbg_get_reg (host_int_4 n);
      component::status dbg_set_reg (host_int_4 n, const string& s);

    protected:
      void stepped (sid::host_int_4 n);

    public:
      ubicom32bf_cpu (const USI cpu=0);
      ~ubicom32bf_cpu () throw () {}

      virtual host_int_4 get_pc () { return (host_int_4)this->get_h_pc ();}
      int  check_mask (UINT flag_type, UINT cc);
      void finish_multiply ();
      void side_effect(USI d_field);
      SI finish_lshift_2(SI tmp_result, USI tmp_uresult);
      UHI pixel_convert(SI truncate, SI data);
      USI pixel_blend(SI truncate, SI data, SI s2, SI src3);
      USI vertical_interpolation(SI order, SI data, SI s2, SI src3);
      USI horizontal_interpolation(SI order, SI data, SI s2, SI src3);
      UHI add_and_saturate(SI upper, SI data, SI s2, SI src3);
      SI   bfrvrs (SI s1, SI s2);
      void suspend ();
      void bkpt (UINT context_mask);
      void iwrite (USI pea, SI s1);
      USI  iread (USI pea);
      static const int IERASE_DATA = 0xffffffff;  // erasing page sets all bits on
      void ierase (USI pea);
      void setcsr (SI s1);
      USI  crcgen (USI B, USI P);
      DI dsp_clip16(DI input);
      DI dsp_clip32(DI input);
      void dsp_addsub(DI s1, DI s2, USI dest_acc, DI addsub, USI mode, USI clip);
      DI my_get_acc(UINT index);
      void my_set_h_dsp_obit(BI value);
      void unused_insn ();

      int  on_chip_flash () { return 0; }
      void set_pc (host_int_4 v) 
	{ 
	  this->hardware.h_pc = (PCADDR) v; 
	}
      void set_h_tctrl (UHI value);
      void set_h_tctrl_w (USI value);
      
      void invalid_insn (PCADDR pc);
      void memory_trap (const cpu_memory_fault& t);

      // syscall temporary registers
      SI syscall_arg0;
      SI syscall_arg1;
      SI syscall_arg2;
      SI syscall_arg3;
      SI syscall_error;
      SI syscall_result;

      inline USI thread_get_self(void)
      {
	return context;
      }

      void setup_rounding(void);
      void set_fpu_exception_flags(void);

      SF fadds(SI source1, SF source2);
      DF faddd(DF source1, DF source2);
      SF fsubs(SI source1, SF source2);
      DF fsubd(DF source1, DF source2);
      SF fmuls(SI source1, SF source2);
      DF fmuld(DF source1, DF source2);
      SF fdivs(SI source1, SF source2);
      DF fdivd(DF source1, DF source2);
      SF fsqrts(SI source1);
      DF fsqrtd(DF source1);
      SF fnegs(SI source1);
      DF fnegd(DF source1);
      SF fabss(SI source1);
      DF fabsd(DF source1);
      SF fi2s(SI source1);
      DF fi2d(SI source1);
      SF fl2s(DF source1);
      DF fl2d(DF source1);
      DF fs2d(SI source1);
      SF fd2s(DF source1);
      SF fs2i(SI source1);
      SF fd2i(DF source1);
      DF fs2l(SI source1);
      DF fd2l(DF source1);
      SF fcmps(SI source1, SF sourcce2);
      DF fcmpd(DF source1, DF source2);

      void do_syscall ();
#if 0
      void build_hrt_table ();
#endif

      // stream / destream state
      void stream_state (ostream& o) const;
      void destream_state (istream& i);

      // attribute handlers
      string get_csr_attr (unsigned context);
      component::status set_csr_attr (unsigned context, const string& state);
      string get_rosr_attr (unsigned context);
      component::status set_rosr_attr (unsigned context, const string& state);
      string get_generic_register_attr (SI addr);
      component::status set_generic_register_attr (SI addr, const string& state);
      string get_current_thread_attr (int *);
      component::status set_current_thread_attr (int *, const string& state);
      component::status set_stat_register_attr (SI addr, const string& state);
      string get_harvard_pc_attr ();
      component::status set_harvard_pc_attr (const string& state);

      // Called by the semantic code at the end of a non-cti insn.
      inline void
      done_insn (PCADDR npc, sem_status& status)
        {
          this->h_pc_set (npc);
        }

      // Called by the semantic code at the end of a cti insn.
      inline void
      done_cti_insn (PCADDR npc, sem_status& status)
        {
          this->h_pc_set (npc);
        }

      // Called by the semantic code for branches
      inline void
      branch (PCADDR new_pc, PCADDR& npc, sem_status& status)
        {
          npc = new_pc;
        }

      // memory routines
      inline QI
      GETMEMQI(PCADDR pc, ADDR addr) 
      {
	return this->read_data_memory_1 (pc, addr);
      }
      inline void
      SETMEMQI(PCADDR pc, ADDR addr, QI value) 
      {
	// see if we need to dcapt checking for indirects
	if (!(this->get_h_dcapt () & 0x1))
	  {
	    // check for debug capture address being written to
	    if ((addr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	      {
		USI tvalue = context|DCAPT_ADDRESS_MATCH;
		set_h_dcapt_pc (pc);
		set_set_dcapt_interrupt (tvalue);
	      }
	  }
	
	return this->write_data_memory_1 (pc, addr, value);
      }
      inline UQI
      GETMEMUQI(PCADDR pc, ADDR addr)
      {
	return this->read_data_memory_1 (pc, addr);
      }
      inline void
      SETMEMUQI(PCADDR pc, ADDR addr, UQI value)
      {
	// see if we need to dcapt checking for indirects
	if (!(this->get_h_dcapt () & 0x1))
	  {
	    // check for debug capture address being written to
	    if ((addr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	      {

		USI value = context|DCAPT_ADDRESS_MATCH;
		set_h_dcapt_pc (pc);
		set_set_dcapt_interrupt (value);
	      }
	  }

	return this->write_data_memory_1 (pc, addr, value);
      }
      inline HI
      GETMEMHI(PCADDR pc, ADDR addr)
      {
	if (UNLIKELY(addr & 0x1))
	  {
            USI tvalue = context | DCAPT_MISALIGNED_SRC; 
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 1) << 1;
	return this->read_data_memory_2 (pc, addr);
      }
      inline void
      SETMEMHI(PCADDR pc, ADDR addr, HI value)
      {
	if (UNLIKELY(addr & 0x1))
	  {
	    USI tvalue = context | DCAPT_MISALIGNED_DEST;
	    set_h_dcapt_pc (pc);
	    set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 1) << 1;
	
	// see if we need to dcapt checking for indirects
	if (!(this->get_h_dcapt () & 0x1))
	  {
	    // check for debug capture address being written to
	    if ((addr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	      {
		USI tvalue = context|DCAPT_ADDRESS_MATCH;
		set_h_dcapt_pc (pc);
		set_set_dcapt_interrupt (tvalue);
	      }
	  }
	
	return this->write_data_memory_2 (pc, addr, value);
      }
      inline UHI
      GETMEMUHI(PCADDR pc, ADDR addr)
      {
	if (UNLIKELY(addr & 0x1))
	  {
            USI tvalue = context | DCAPT_MISALIGNED_SRC;
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 1) << 1;
	return this->read_data_memory_2 (pc, addr);
      }
      inline void
      SETMEMUHI(PCADDR pc, ADDR addr, UHI value)
      {
	if (UNLIKELY(addr & 0x1))
	  {
            USI tvalue = context | DCAPT_MISALIGNED_DEST;
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 1) << 1;

	// see if we need to dcapt checking for indirects
	if (!(this->get_h_dcapt () & 0x1))
	  {
	    // check for debug capture address being written to
	    if ((addr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	      {
		USI tvalue = context|DCAPT_ADDRESS_MATCH;
		set_h_dcapt_pc (pc);
		set_set_dcapt_interrupt (tvalue);
	      }
	  }
	
	return this->write_data_memory_2 (pc, addr, value);
      }
      inline SI
      GETMEMSI(PCADDR pc, ADDR addr)
      {
	if (UNLIKELY(addr & 0x3))
	  {
            USI tvalue = context | DCAPT_MISALIGNED_SRC; 
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 2) << 2;
	return this->read_data_memory_4 (pc, addr);
      }
      inline void
      SETMEMSI(PCADDR pc, ADDR addr, SI value)
      {
	if (UNLIKELY(addr & 0x3))
	  {
            USI tvalue = context | DCAPT_MISALIGNED_DEST;
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 2) << 2;

	// see if we need to dcapt checking for indirects
	if (!(this->get_h_dcapt () & 0x1))
	  {
	    // check for debug capture address being written to
	    if ((addr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	      {
		USI value = context|DCAPT_ADDRESS_MATCH;
		set_h_dcapt_pc (pc);
		set_set_dcapt_interrupt (value);
	      }
	  }
	
	return this->write_data_memory_4 (pc, addr, value);
      }
      inline void
      SETMEMSI_DIRECT(PCADDR pc, ADDR addr, SI value) 
      {
	if (UNLIKELY(addr & 0x3))
	  {
            USI tvalue = context | DCAPT_MISALIGNED_DEST;
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 2) << 2;

	// do not check direct addresses for debug capture address
	
	return this->write_data_memory_4 (pc, addr, value);
      }
      inline USI
      GETMEMUSI(PCADDR pc, ADDR addr) 
      {
	if UNLIKELY(addr & 0x3)
	  {
            USI tvalue = context | DCAPT_MISALIGNED_SRC; 
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 2) << 2;
	return this->read_data_memory_4 (pc, addr);
      }
      inline void
      SETMEMUSI(PCADDR pc, ADDR addr, USI value) 
      {
	if UNLIKELY(addr & 0x3)
	  {
            USI tvalue = context | DCAPT_MISALIGNED_DEST;
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }

	addr = (addr >> 2) << 2;
	
	// see if we need to dcapt checking for indirects
	if (!(this->get_h_dcapt () & 0x1))
	  {
	    // check for debug capture address being written to
	    if ((addr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	      {
		USI tvalue = context|DCAPT_ADDRESS_MATCH;
		set_h_dcapt_pc (pc);
		set_set_dcapt_interrupt (tvalue);
	      }
	  }

	return this->write_data_memory_4 (pc, addr, value);
      }
      inline DI
      GETMEMDI(PCADDR pc, ADDR addr) 
      {
	if UNLIKELY(addr & 0x3)
	  {
            USI tvalue = context | DCAPT_MISALIGNED_SRC; 
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 3) << 3;
	return this->read_data_memory_8 (pc, addr);
      }
      inline void
      SETMEMDI(PCADDR pc, ADDR addr, DI value) 
      {
	if UNLIKELY(addr & 0x3)
	  {
            USI tvalue = context | DCAPT_MISALIGNED_DEST;
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	// see if we need to dcapt checking for indirects
	if (!(this->get_h_dcapt () & 0x1))
	  {
	    // check for debug capture address being written to
	    if ((addr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	      {
		USI value = context|DCAPT_ADDRESS_MATCH;
		set_h_dcapt_pc (pc);
		set_set_dcapt_interrupt (value);
	      }
	  }

	addr = (addr >> 3) << 3;

	return this->write_data_memory_8 (pc, addr, value);
      }
      inline UDI
      GETMEMUDI(PCADDR pc, ADDR addr) 
      {
	if UNLIKELY(addr & 0x3)
	  {
            USI tvalue = context | DCAPT_MISALIGNED_SRC; 
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	addr = (addr >> 3) << 3;
	return this->read_data_memory_8 (pc, addr);
      }
      inline void
      SETMEMUDI(PCADDR pc, ADDR addr, UDI value) 
      {
	if UNLIKELY(addr & 0x3)
	  {
            USI tvalue = context | DCAPT_MISALIGNED_DEST;
            set_h_dcapt_pc (pc);                         
            set_set_dcapt_interrupt (tvalue);                 
	  }
	
	// see if we need to dcapt checking for indirects
	if (!(this->get_h_dcapt () & 0x1))
	  {
	    // check for debug capture address being written to
	    if ((addr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	      {
		USI value = context|DCAPT_ADDRESS_MATCH;
		set_h_dcapt_pc (pc);
		set_set_dcapt_interrupt (value);
	      }
	  }

	addr = (addr >> 3) << 3;

	return this->write_data_memory_8 (pc, addr, value);
      }
    };

} // namespace ubicom32bf







