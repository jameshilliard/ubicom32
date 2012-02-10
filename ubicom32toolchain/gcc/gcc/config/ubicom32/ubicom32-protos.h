/* Function prototypes for Ubicom IP3000.

   Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
   2009 Free Software Foundation, Inc.
   Contributed by Ubicom, Inc.

   This file is part of GNU CC.

   GNU CC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   GNU CC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License along
   with GNU CC; see the file COPYING.  If not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifdef RTX_CODE

#ifdef TREE_CODE
extern void ubicom32_va_start (tree, rtx);
#endif /* TREE_CODE */

extern void ubicom32_print_operand (FILE *, rtx, int);
extern void ubicom32_print_operand_address (FILE *, rtx);

extern void ubicom32_conditional_register_usage (void);
extern enum reg_class ubicom32_preferred_reload_class (rtx, enum reg_class);
extern int ubicom32_regno_ok_for_index_p (int, int);
extern void ubicom32_expand_movsi (rtx *);
extern void ubicom32_expand_addsi3 (rtx *);
extern bool ubicom32_expand_scode (enum rtx_code, rtx, rtx, rtx *);
extern bool ubicom32_expand_addcc (enum rtx_code, rtx, rtx, rtx, rtx, rtx);
extern bool ubicom32_expand_mulsi3 (rtx *);
extern void ubicom32_split_sne (rtx *);
extern void ubicom32_split_seq (rtx *);
extern void ubicom32_split_slt (rtx *);
extern void ubicom32_split_sge (rtx *);
extern void ubicom32_split_sltu (rtx *);
extern void ubicom32_split_sgeu (rtx *);
extern void ubicom32_emit_move_const_int (rtx, rtx);
extern bool ubicom32_legitimate_constant_p (rtx);
extern bool ubicom32_legitimate_address_p (enum machine_mode, rtx, int);
extern rtx ubicom32_legitimize_address (rtx, rtx, enum machine_mode);
extern rtx ubicom32_legitimize_reload_address (rtx, enum machine_mode, int, int);
extern void ubicom32_canonicalize_comparison (enum rtx_code *code, rtx *op0, rtx *op1);
extern int ubicom32_mode_dependent_address_p (rtx);
extern void ubicom32_output_cond_jump (rtx, rtx, rtx);
extern void ubicom32_expand_eh_return (rtx *);
extern enum machine_mode ubicom32_select_cc_mode (RTX_CODE, rtx, rtx);
extern rtx ubicom32_gen_compare (RTX_CODE, rtx, rtx);
extern int ubicom32_shiftable_const_int (int);
#endif /* RTX_CODE */

#ifdef TREE_CODE
extern void ubicom32_init_cumulative_args (CUMULATIVE_ARGS *, tree, rtx);
extern struct rtx_def *function_arg (CUMULATIVE_ARGS *,
				     enum machine_mode, tree, int);
extern struct rtx_def *function_incoming_arg (CUMULATIVE_ARGS *,
					      enum machine_mode,
					      tree, int);
extern int function_arg_partial_nregs (CUMULATIVE_ARGS *,
				       enum machine_mode, tree, int);
extern struct rtx_def *ubicom32_va_arg (tree, tree);
extern int ubicom32_reg_parm_stack_space (tree);
#endif /* TREE_CODE */

extern void asm_file_start (FILE *);
extern void ubicom32_split_operand_pair (rtx *);
extern void ubicom32_expand_prologue (void);
extern void ubicom32_expand_epilogue (bool);
extern int ubicom32_initial_elimination_offset (int, int);
extern int ubicom32_regno_ok_for_base_p (int, int);
extern bool ubicom32_hard_regno_mode_ok (unsigned int, enum machine_mode);
extern rtx ubicom32_return_addr_rtx (int, rtx);
extern void ubicom32_optimization_options (int, int);
extern void ubicom32_override_options (void);
extern bool ubicom32_match_cc_mode (rtx, enum machine_mode);
extern int ubicom32_register_move_cost (enum machine_mode, enum reg_class,
					enum reg_class);
extern int ubicom32_memory_move_cost (enum machine_mode, enum reg_class, int);
extern int ubicom32_address_dep_p (rtx, rtx);
extern alias_set_type ubicom32_GOT_alias_set (void);
extern bool ubicom32_hard_regno_rename_ok (unsigned int, unsigned int);

