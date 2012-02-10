/* CPU class elements for ubicom32bf.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 2000, 2001, 2002, 2003 Red Hat, Inc.

This file is part of the Red Hat simulators.


*/

// This file is included in the middle of the cpu class struct.

public:

// CPU state information.

  // Hardware elements.
  struct {
  /* address register pre/post increment values */
  USI h_ar_inc[8];
  /* address register pre/post increment flag */
  BI h_ar_inc_flag;
  /* program counter */
  USI h_pc;
  } hardware;

  // C++ register access function templates
#define current_cpu this

  inline USI h_global_control_get () const { return current_cpu->get_h_glbl_ctl (); }
  inline void h_global_control_set (USI newval) { current_cpu->set_h_glbl_ctl (newval);
 }

  inline USI h_mt_break_get () const { return current_cpu->get_h_mt_break (); }
  inline void h_mt_break_set (USI newval) { current_cpu->set_h_mt_break (newval);
 }

  inline USI h_mt_active_get () const { return current_cpu->get_h_mt_active (); }
  inline void h_mt_active_set (USI newval) { current_cpu->set_h_mt_active (newval);
 }

  inline USI h_mt_enable_get () const { return current_cpu->get_h_mt_enable (); }
  inline void h_mt_enable_set (USI newval) { current_cpu->set_h_mt_enable (newval);
 }

  inline USI h_mt_priority_get () const { return current_cpu->get_h_mt_priority (); }
  inline void h_mt_priority_set (USI newval) { current_cpu->set_h_mt_priority (newval);
 }

  inline USI h_mt_schedule_get () const { return current_cpu->get_h_mt_schedule (); }
  inline void h_mt_schedule_set (USI newval) { current_cpu->set_h_mt_schedule (newval);
 }

  inline USI h_irq_status_0_get () const { return current_cpu->get_h_irq_status_0 (); }
  inline void h_irq_status_0_set (USI newval) { current_cpu->set_h_irq_status_0 (newval);
 }

  inline USI h_irq_status_1_get () const { return current_cpu->get_h_irq_status_1 (); }
  inline void h_irq_status_1_set (USI newval) { current_cpu->set_h_irq_status_1 (newval);
 }

  inline SI h_dr_get (UINT regno) const { return current_cpu->get_h_dr (regno); }
  inline void h_dr_set (UINT regno, SI newval) { current_cpu->set_h_dr (regno, newval);
 }

  inline SI h_s1_dr_get (UINT regno) const { return current_cpu->get_h_s1_dr (regno); }
  inline void h_s1_dr_set (UINT regno, SI newval) { ((void) 0); /*nop*/
 }

  inline USI h_ar_get (UINT regno) const { return current_cpu->get_h_ar (regno); }
  inline void h_ar_set (UINT regno, USI newval) { current_cpu->set_h_ar (regno, newval);
 }

  inline USI h_ar_inc_get (UINT regno) const { return this->hardware.h_ar_inc[regno]; }
  inline void h_ar_inc_set (UINT regno, USI newval) { this->hardware.h_ar_inc[regno] = newval; }

  inline BI h_ar_inc_flag_get () const { return this->hardware.h_ar_inc_flag; }
  inline void h_ar_inc_flag_set (BI newval) { this->hardware.h_ar_inc_flag = newval; }

  inline SI h_mac_hi_get () const { return current_cpu->get_h_mac_hi (); }
  inline void h_mac_hi_set (SI newval) { current_cpu->set_h_mac_hi (newval);
 }

  inline USI h_mac_lo_get () const { return current_cpu->get_h_mac_lo (); }
  inline void h_mac_lo_set (USI newval) { current_cpu->set_h_mac_lo (newval);
 }

  inline SI h_src_3_get () const { return current_cpu->get_h_src_3 (); }
  inline void h_src_3_set (SI newval) { current_cpu->set_h_src_3 (newval);
 }

  inline USI h_csr_get () const { return current_cpu->get_h_csr (); }
  inline void h_csr_set (USI newval) { current_cpu->set_h_csr (newval);
 }

  inline USI h_iread_get () const { return current_cpu->get_h_iread (); }
  inline void h_iread_set (USI newval) { current_cpu->set_h_iread (newval);
 }

  inline SI h_acc1_hi_get () const { return current_cpu->get_h_acc1_hi (); }
  inline void h_acc1_hi_set (SI newval) { current_cpu->set_h_acc1_hi (newval);
 }

  inline USI h_acc1_lo_get () const { return current_cpu->get_h_acc1_lo (); }
  inline void h_acc1_lo_set (USI newval) { current_cpu->set_h_acc1_lo (newval);
 }

  inline USI h_pc_get () const { return this->hardware.h_pc; }
  inline void h_pc_set (USI newval) { this->hardware.h_pc = newval; }

  inline BI h_nbit_16_get () const { return current_cpu->get_h_nbit_16 (); }
  inline void h_nbit_16_set (BI newval) { current_cpu->set_h_nbit_16 (newval);
 }

  inline BI h_zbit_16_get () const { return current_cpu->get_h_zbit_16 (); }
  inline void h_zbit_16_set (BI newval) { current_cpu->set_h_zbit_16 (newval);
 }

  inline BI h_vbit_16_get () const { return current_cpu->get_h_vbit_16 (); }
  inline void h_vbit_16_set (BI newval) { current_cpu->set_h_vbit_16 (newval);
 }

  inline BI h_cbit_16_get () const { return current_cpu->get_h_cbit_16 (); }
  inline void h_cbit_16_set (BI newval) { current_cpu->set_h_cbit_16 (newval);
 }

  inline BI h_nbit_32_get () const { return current_cpu->get_h_nbit_32 (); }
  inline void h_nbit_32_set (BI newval) { current_cpu->set_h_nbit_32 (newval);
 }

  inline BI h_zbit_32_get () const { return current_cpu->get_h_zbit_32 (); }
  inline void h_zbit_32_set (BI newval) { current_cpu->set_h_zbit_32 (newval);
 }

  inline BI h_vbit_32_get () const { return current_cpu->get_h_vbit_32 (); }
  inline void h_vbit_32_set (BI newval) { current_cpu->set_h_vbit_32 (newval);
 }

  inline BI h_cbit_32_get () const { return current_cpu->get_h_cbit_32 (); }
  inline void h_cbit_32_set (BI newval) { current_cpu->set_h_cbit_32 (newval);
 }

  inline BI h_prev_privbit_get () const { return current_cpu->get_h_prev_privbit (); }
  inline void h_prev_privbit_set (BI newval) { current_cpu->set_h_prev_privbit (newval);
 }

  inline BI h_privbit_get () const { return current_cpu->get_h_privbit (); }
  inline void h_privbit_set (BI newval) { current_cpu->set_h_privbit (newval);
 }

  inline USI h_DSP_S2_Acc_reg_mul_get (UINT regno) const { return current_cpu->get_acc_lo (regno); }
  inline void h_DSP_S2_Acc_reg_mul_set (UINT regno, USI newval) { ((void) 0); /*nop*/
 }

  inline DI h_DSP_S2_Acc_reg_addsub_get (UINT regno) const { return current_cpu->get_acc (regno); }
  inline void h_DSP_S2_Acc_reg_addsub_set (UINT regno, DI newval) { ((void) 0); /*nop*/
 }

  inline DF h_FPU_acc_64_get (UINT regno) const { return current_cpu->get_acc_fpu_64 (regno); }
  inline void h_FPU_acc_64_set (UINT regno, DF newval) { current_cpu->set_acc_fpu_64 (regno, newval);
 }

  inline SF h_FPU_acc_32_get (UINT regno) const { return current_cpu->get_acc_fpu_32 (regno); }
  inline void h_FPU_acc_32_set (UINT regno, SF newval) { current_cpu->set_acc_fpu_32 (regno, newval);
 }

  inline SF h_FPU_data_reg_32_get (UINT regno) const { return current_cpu->get_fpu_32_from_data_reg (regno); }
  inline void h_FPU_data_reg_32_set (UINT regno, SF newval) { ((void) 0); /*nop*/
 }

  inline DF h_FPU_data_reg_64_get (UINT regno) const { return current_cpu->get_fpu_64_from_data_reg (regno); }
  inline void h_FPU_data_reg_64_set (UINT regno, DF newval) { ((void) 0); /*nop*/
 }

  inline SI h_sp_get (UINT regno) const { return current_cpu->get_h_scratchpad (regno); }
  inline void h_sp_set (UINT regno, SI newval) { current_cpu->set_h_scratchpad (regno, newval);
 }

#undef current_cpu

