/* Misc. entries in the ubicom32 description file.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 2000, 2001, 2002, 2003 Red Hat, Inc.

This file is part of the Red Hat simulators.


*/

#ifndef DESC_UBICOM32_H
#define DESC_UBICOM32_H

namespace ubicom32 {

// Enums.

/* Enum declaration for insn format enums for moveai.  */
typedef enum insn_opmoveai {
  OP_MOVEAI_MOVEAI = 14
} INSN_OPMOVEAI;

/* Enum declaration for insn format enums.  */
typedef enum insn_op1 {
  OP_X0, OP_UNUSED_01, OP_X2, OP_FPU
 , OP_BSET, OP_BCLR, OP_DSP, OP_UNUSED_07
 , OP_AND_2, OP_AND_4, OP_OR_2, OP_OR_4
 , OP_XOR_2, OP_XOR_4, OP_ADD_2, OP_ADD_4
 , OP_ADDC, OP_SUB_2, OP_SUB_4, OP_SUBC
 , OP_PXBLEND, OP_PXVI, OP_PXADDS, OP_UNUSED_17
 , OP_CMPI, OP_MOVEI, OP_JMP, OP_CALL
 , OP_MOVEAI, OP_MOVEAIH, OP_X1E, OP_UNUSED_1F
} INSN_OP1;

/* Enum declaration for insn x0 opcode ext enums.  */
typedef enum insn_op2 {
  OPX0_UNUSED_00, OPX0_SUSPEND, OPX0_FLUSH, OPX0_SYNC
 , OPX0_RET, OPX0_PREFETCH, OPX0_IREAD, OPX0_BKPT
 , OPX0_SYSRET, OPX0_SYSCALL, OPX0_NOT_4, OPX0_NOT_2
 , OPX0_MOVE_4, OPX0_MOVE_2, OPX0_MOVEA, OPX0_MOVE_1
 , OPX0_IWRITE, OPX0_FLUSHNI, OPX0_SETCSR, OPX0_TBSET
 , OPX0_TBCLR, OPX0_EXT_2, OPX0_INVAL, OPX0_EXT_1
 , OPX0_SWAPB_2, OPX0_SWAPB_4, OPX0_PXCNV, OPX0_PXCNV_T
 , OPX0_LEA_4, OPX0_LEA_2, OPX0_PDEC, OPX0_LEA_1
} INSN_OP2;

/* Enum declaration for insn x1E opcode ext enums.  */
typedef enum insn_x1e {
  OPX1E_CALLI, OPX1E_LEAI, OPX1E_CALLI_B, OPX1E_UNUSED_03
 , OPX1E_UNUSED_04, OPX1E_UNUSED_05, OPX1E_UNUSED_06, OPX1E_UNUSED_07
 , OPX1E_UNUSED_08, OPX1E_UNUSED_09, OPX1E_UNUSED_0A, OPX1E_UNUSED_0B
 , OPX1E_UNUSED_0C, OPX1E_UNUSED_0D, OPX1E_UNUSED_0E, OPX1E_UNUSED_0F
 , OPX1E_UNUSED_10, OPX1E_UNUSED_11, OPX1E_UNUSED_12, OPX1E_UNUSED_13
 , OPX1E_UNUSED_14, OPX1E_UNUSED_15, OPX1E_UNUSED_16, OPX1E_UNUSED_17
 , OPX1E_UNUSED_18, OPX1E_UNUSED_19, OPX1E_UNUSED_1A, OPX1E_UNUSED_1B
 , OPX1E_UNUSED_1C, OPX1E_UNUSED_1D, OPX1E_UNUSED_1E, OPX1E_UNUSED_1F
} INSN_X1E;

/* Enum declaration for insn x2 opcode ext enums.  */
typedef enum insn_opext {
  OPX2_PXHI, OPX2_MULS, OPX2_PXHI_S, OPX2_MULU
 , OPX2_UNUSED_04, OPX2_MULF, OPX2_BTST, OPX2_UNUSED_07
 , OPX2_CRCGEN, OPX2_MAC, OPX2_LSL_1, OPX2_LSR_1
 , OPX2_ASR_1, OPX2_UNUSED_0D, OPX2_UNUSED_0E, OPX2_UNUSED_0F
 , OPX2_LSL_4, OPX2_LSL_2, OPX2_LSR_4, OPX2_LSR_2
 , OPX2_ASR_4, OPX2_ASR_2, OPX2_BFEXTU, OPX2_UNUSED_17
 , OPX2_BFRVRS, OPX2_UNUSED_19, OPX2_SHFTD, OPX2_UNUSED_1B
 , OPX2_MERGE, OPX2_UNUSED_1D, OPX2_SHMRG_2, OPX2_SHMRG_1
} INSN_OPEXT;

/* Enum declaration for insn dsp opcode ext enums.  */
typedef enum insn_dsp_subop {
  OPDSP_MULS, OPDSP_MACS, OPDSP_MULU, OPDSP_MACU
 , OPDSP_MULF, OPDSP_MACF, OPDSP_MACS_4, OPDSP_MACUS
 , OPDSP_MULS_4, OPDSP_MSUF, OPDSP_MULU_4, OPDSP_MACU_4
 , OPDSP_UNUSED_0C, OPDSP_UNUSED_0D, OPDSP_UNUSED_0E, OPDSP_UNUSED_0F
 , OPDSP_MADD_4, OPDSP_MADD_2, OPDSP_MSUB_4, OPDSP_MSUB_2
 , OPDSP_UNUSED_14, OPDSP_UNUSED_15, OPDSP_UNUSED_16, OPDSP_UNUSED_17
 , OPDSP_UNUSED_18, OPDSP_UNUSED_19, OPDSP_UNUSED_1A, OPDSP_UNUSED_1B
 , OPDSP_UNUSED_1C, OPDSP_UNUSED_1D, OPDSP_UNUSED_1E, OPDSP_UNUSED_1F
} INSN_DSP_SUBOP;

/* Enum declaration for insn FPU opcode ext enums.  */
typedef enum insn_fpu_subop {
  OPFPU_FADDS, OPFPU_FSUBS, OPFPU_FMULS, OPFPU_FDIVS
 , OPFPU_FI2D, OPFPU_FS2D, OPFPU_FS2L, OPFPU_FSQRTS
 , OPFPU_FNEGS, OPFPU_FABSS, OPFPU_FI2S, OPFPU_FS2I
 , OPFPU_FCMPS, OPFPU_UNUSED_0D, OPFPU_UNUSED_0E, OPFPU_UNUSED_0F
 , OPFPU_FADDD, OPFPU_FSUBD, OPFPU_FMULD, OPFPU_FDIVD
 , OPFPU_FL2S, OPFPU_FD2S, OPFPU_FD2I, OPFPU_FSQRTD
 , OPFPU_FNEGD, OPFPU_FABSD, OPFPU_FL2D, OPFPU_FD2L
 , OPFPU_FCMPD, OPFPU_UNUSED_1D, OPFPU_UNUSED_1E, OPFPU_UNUSED_1F
} INSN_FPU_SUBOP;

/* Enum declaration for .  */
typedef enum data_names {
  H_DR_D0, H_DR_D1, H_DR_D2, H_DR_D3
 , H_DR_D4, H_DR_D5, H_DR_D6, H_DR_D7
 , H_DR_D8, H_DR_D9, H_DR_D10, H_DR_D11
 , H_DR_D12, H_DR_D13, H_DR_D14, H_DR_D15
} DATA_NAMES;

/* Enum declaration for .  */
typedef enum addr_names {
  H_AR_SP = 7, H_AR_A0 = 0, H_AR_A1 = 1, H_AR_A2 = 2
 , H_AR_A3 = 3, H_AR_A4 = 4, H_AR_A5 = 5, H_AR_A6 = 6
 , H_AR_A7 = 7
} ADDR_NAMES;

/* Enum declaration for .  */
typedef enum acc_names {
  ACCS_ACC0, ACCS_ACC1
} ACC_NAMES;

/* Enum declaration for .  */
typedef enum acc_lo_names {
  ACC_LOS_ACC0_LO, ACC_LOS_ACC0_HI, ACC_LOS_ACC1_LO, ACC_LOS_ACC1_HI
} ACC_LO_NAMES;

/* Enum declaration for .  */
typedef enum data_names_even {
  H_DR_EVEN_D0, H_DR_EVEN_D2, H_DR_EVEN_D4, H_DR_EVEN_D6
 , H_DR_EVEN_D8, H_DR_EVEN_D10, H_DR_EVEN_D12, H_DR_EVEN_D14
} DATA_NAMES_EVEN;

/* Enum declaration for .  */
typedef enum spad_names {
  H_SP_SCRATCHPAD0 = 0, H_SP_SCRATCHPAD1 = 0, H_SP_SCRATCHPAD2 = 0, H_SP_SCRATCHPAD3 = 0
 , H_SP_SCRATCHPAD4 = 0, H_SP_SCRATCHPAD5 = 0
} SPAD_NAMES;

// Insn attribute indices.

/* Enum declaration for cgen_insn attrs.  */
typedef enum cgen_insn_attr {
  CGEN_INSN_ALIAS, CGEN_INSN_VIRTUAL, CGEN_INSN_UNCOND_CTI, CGEN_INSN_COND_CTI
 , CGEN_INSN_SKIP_CTI, CGEN_INSN_DELAY_SLOT, CGEN_INSN_RELAXABLE, CGEN_INSN_RELAXED
 , CGEN_INSN_NO_DIS, CGEN_INSN_PBB, CGEN_INSN_END_BOOLS, CGEN_INSN_START_NBOOLS = 31
 , CGEN_INSN_MACH, CGEN_INSN_END_NBOOLS
} CGEN_INSN_ATTR;

/* Number of non-boolean elements in cgen_insn_attr.  */
#define CGEN_INSN_NBOOL_ATTRS (CGEN_INSN_END_NBOOLS - CGEN_INSN_START_NBOOLS - 1)

// Attributes.

/* Enum declaration for machine type selection.  */
typedef enum mach_attr {
  MACH_BASE, MACH_IP3035, MACH_UBICOM32DSP, MACH_IP3023COMPATIBILITY
 , MACH_UBICOM32_VER4, MACH_UBICOM32_VER5, MACH_UBICOM32_OLDMOVEAI, MACH_UBICOM32_NEWMOVEAI
 , MACH_UBICOM32_NEWMOVEAISIMULATE, MACH_MAX
} MACH_ATTR;

/* Enum declaration for instruction set selection.  */
typedef enum isa_attr {
  ISA_UBICOM32, ISA_MAX
} ISA_ATTR;

// Insn attributes.

struct ubicom32_insn_attr {
  unsigned int bools;
  unsigned int mach;
  inline unsigned int get_mach_attr () { return mach; }
  inline int get_alias_attr () { return (bools & (1<<CGEN_INSN_ALIAS)) != 0; }
  inline int get_virtual_attr () { return (bools & (1<<CGEN_INSN_VIRTUAL)) != 0; }
  inline int get_uncond_cti_attr () { return (bools & (1<<CGEN_INSN_UNCOND_CTI)) != 0; }
  inline int get_cond_cti_attr () { return (bools & (1<<CGEN_INSN_COND_CTI)) != 0; }
  inline int get_skip_cti_attr () { return (bools & (1<<CGEN_INSN_SKIP_CTI)) != 0; }
  inline int get_delay_slot_attr () { return (bools & (1<<CGEN_INSN_DELAY_SLOT)) != 0; }
  inline int get_relaxable_attr () { return (bools & (1<<CGEN_INSN_RELAXABLE)) != 0; }
  inline int get_relaxed_attr () { return (bools & (1<<CGEN_INSN_RELAXED)) != 0; }
  inline int get_no_dis_attr () { return (bools & (1<<CGEN_INSN_NO_DIS)) != 0; }
  inline int get_pbb_attr () { return (bools & (1<<CGEN_INSN_PBB)) != 0; }
};

#define MACH_IP3035_INSN_CHUNK_BITSIZE 0
#define MACH_UBICOM32DSP_INSN_CHUNK_BITSIZE 0
#define MACH_UBICOM32_VER4_INSN_CHUNK_BITSIZE 0
#define MACH_UBICOM32_VER5_INSN_CHUNK_BITSIZE 0
#define MACH_UBICOM32_OLDMOVEAI_INSN_CHUNK_BITSIZE 0

} // end ubicom32 namespace

#endif /* DESC_UBICOM32_H */
