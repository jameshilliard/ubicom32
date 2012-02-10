; Predicate definitions for Ubicom32.

; Copyright (C) 2009 Free Software Foundation, Inc.
; Contributed by Ubicom, Inc.

; This file is part of GCC.

; GCC is free software; you can redistribute it and/or modify it
; under the terms of the GNU General Public License as published
; by the Free Software Foundation; either version 3, or (at your
; option) any later version.

; GCC is distributed in the hope that it will be useful, but WITHOUT
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
; License for more details.

; You should have received a copy of the GNU General Public License
; along with GCC; see the file COPYING3.  If not see
; <http://www.gnu.org/licenses/>.

(define_predicate "ubicom32_move_operand"
  (match_code "const_int, const_double, const, mem, subreg, reg, lo_sum")
{
  if (CONST_INT_P (op))
    return true;

  if (GET_CODE (op) == CONST_DOUBLE)
    return true;
  
  if (GET_CODE (op) == CONST)
    return memory_address_p (mode, op);

  if (GET_MODE (op) != mode)
    return false;

  if (MEM_P (op))
    return memory_address_p (mode, XEXP (op, 0));
  
  if (GET_CODE (op) == SUBREG) {
      op = SUBREG_REG (op);

      if (REG_P (op))
	return true;
  
      if (! MEM_P (op))
	return false;

      /* Paradoxical SUBREG.  */
      if (GET_MODE_SIZE (mode) > GET_MODE_SIZE (GET_MODE (op)))
	return false;

      return memory_address_p (GET_MODE (op), XEXP (op, 0));
    }

  return register_operand (op, mode);
})

;; Returns true if OP is either a symbol reference or a sum of a
;; symbol reference and a constant.

(define_predicate "ubicom32_symbolic_address_operand"
  (match_code "symbol_ref, label_ref, const")
{
  switch (GET_CODE (op))
    {
    case SYMBOL_REF:
    case LABEL_REF:
      return true;

    case CONST:
      op = XEXP (op, 0);
      return ((GET_CODE (XEXP (op, 0)) == SYMBOL_REF
	       || GET_CODE (XEXP (op, 0)) == LABEL_REF)
	      && CONST_INT_P (XEXP (op, 1)));

    default:
      return false;
    }
})

;; Returns true if OP is either a symbol reference, a sum of a
;; symbol reference and a constant or a constant integer.

(define_predicate "ubicom32_moveai_operand"
  (match_code "symbol_ref, label_ref, const, const_int")
{
  switch (GET_CODE (op))
    {
    case SYMBOL_REF:
    case LABEL_REF:
      return true;

    case CONST_INT:
      /* For v4 and earlier we can only handle 31 bits but
         we can handle the full 32 bits for v5.  */
      if (ubicom32_v5)
	return true;

      return (INTVAL (op) >= 0
	      && INTVAL (op) <= 0x7fffffff);

    case CONST:
      op = XEXP (op, 0);
      return ((GET_CODE (XEXP (op, 0)) == SYMBOL_REF
	       || GET_CODE (XEXP (op, 0)) == LABEL_REF)
	      && CONST_INT_P (XEXP (op, 1)));

    default:
      return false;
    }
})

;; Return true if operand is the FD-PIC register.

(define_predicate "ubicom32_fdpic_register_operand"
  (match_code "reg")
{
  unsigned int regno;

  if (! TARGET_FDPIC)
    return false;

  if (!REG_P (op))
    return false;

  regno = REGNO (op);

  if (GET_MODE (op) != mode && mode != VOIDmode)
    return false;

  return ((regno >= FIRST_PSEUDO_REGISTER 
	   && regno != REGNO (virtual_stack_vars_rtx))
	  || REGNO_REG_CLASS (regno) == FDPIC_REG);
})

(define_predicate "ubicom32_fdpic_got_1_offset_operand"
  (match_code "unspec")
{
  if (! TARGET_FDPIC)
    return false;

  if (XINT (op, 1) != UNSPEC_FDPIC_GOT_1
      && XINT (op, 1) != UNSPEC_FDPIC_FUNCDESC_GOT_1)
    return false;

  return true;
})

(define_predicate "ubicom32_fdpic_got_4_offset_operand"
  (match_code "unspec")
{
  if (! TARGET_FDPIC)
    return false;

  if (XINT (op, 1) != UNSPEC_FDPIC_GOT_4
      && XINT (op, 1) != UNSPEC_FDPIC_FUNCDESC_GOT_4)
    return false;

  return true;
})

(define_predicate "ubicom32_arith_operand"
  (match_code "subreg, reg, const_int, lo_sum, mem")
{
  return (ubicom32_move_operand (op, mode)
	  && ! ubicom32_symbolic_address_operand (op, mode)
	  && (! CONST_INT_P (op)
	      || satisfies_constraint_I (op)));
})

(define_predicate "ubicom32_arith_operand_dot1"
  (match_code "subreg, reg, const_int, lo_sum, mem")
{
  return (ubicom32_move_operand (op, mode)
	  && ! ubicom32_symbolic_address_operand (op, mode)
	  && (! CONST_INT_P (op)
	      || satisfies_constraint_Q (op)));
})

(define_predicate "ubicom32_arith_operand_dot2"
  (match_code "subreg, reg, const_int, lo_sum, mem")
{
  return (ubicom32_move_operand (op, mode)
	  && ! ubicom32_symbolic_address_operand (op, mode)
	  && (! CONST_INT_P (op)
	      || satisfies_constraint_R (op)));
})

(define_predicate "ubicom32_compare_operand"
  (match_code "subreg, reg, const_int, lo_sum, mem")
{
 return (ubicom32_move_operand (op, mode)
	  && ! ubicom32_symbolic_address_operand (op, mode)
	  && (! CONST_INT_P (op)
	      || satisfies_constraint_N1 (op)));
})

(define_predicate "ubicom32_compare_operator"
  (match_code "compare"))

(define_predicate "ubicom32_and_or_si3_operand"
  (match_code "subreg, reg, const_int, lo_sum, mem")
{
  return (ubicom32_arith_operand (op, mode)
	  || (CONST_INT_P (op)
	      && ((exact_log2 (INTVAL (op) + 1) != -1
		   && exact_log2 (INTVAL (op) + 1) <= 31)
		  || (exact_log2 (INTVAL (op)) != -1
		      && exact_log2 (INTVAL (op)) <= 31)
		  || (exact_log2 (~INTVAL (op)) != -1
		      && exact_log2 (~INTVAL (op)) <= 31))));
})

(define_predicate "ubicom32_and_or_hi3_operand"
  (match_code "subreg, reg, const_int, lo_sum, mem")
{
  return (ubicom32_arith_operand (op, mode)
	  || (CONST_INT_P (op)
	      && exact_log2 (INTVAL (op) + 1) != -1
	      && exact_log2 (INTVAL (op) + 1) <= 15));
})

(define_predicate "ubicom32_mem_or_address_register_operand"
  (match_code "subreg, reg, mem")
{
  unsigned int regno;

  if (MEM_P (op)
      && memory_operand (op, mode))
    return true;

  if (REG_P (op))
    regno = REGNO (op);
  else if (GET_CODE (op) == SUBREG && REG_P (SUBREG_REG (op)))
    {
      int offset;
      if (REGNO (SUBREG_REG (op)) >= FIRST_PSEUDO_REGISTER)
	offset = SUBREG_BYTE (op) / (GET_MODE_SIZE (GET_MODE (op)));
      else
	offset = subreg_regno_offset (REGNO (SUBREG_REG (op)),
				      GET_MODE (SUBREG_REG (op)),
				      SUBREG_BYTE (op),
				      GET_MODE (op));
      regno = REGNO (SUBREG_REG (op)) + offset;
    }
  else
    return false;

  return (regno >= FIRST_PSEUDO_REGISTER 
	  || REGNO_REG_CLASS (regno) == FDPIC_REG
	  || REGNO_REG_CLASS (regno) == LINK_REG
	  || REGNO_REG_CLASS (regno) == SIBCALL_ADDRESS_REGS
	  || REGNO_REG_CLASS (regno) == ADDRESS_REGS);
})

(define_predicate "ubicom32_data_register_operand"
  (match_code "subreg, reg")
{
  unsigned int regno;

  if (REG_P (op))
    regno = REGNO (op);
  else if (GET_CODE (op) == SUBREG && REG_P (SUBREG_REG (op)))
    {
      int offset;
      if (REGNO (SUBREG_REG (op)) >= FIRST_PSEUDO_REGISTER)
	offset = SUBREG_BYTE (op) / (GET_MODE_SIZE (GET_MODE (op)));
      else
	offset = subreg_regno_offset (REGNO (SUBREG_REG (op)),
				      GET_MODE (SUBREG_REG (op)),
				      SUBREG_BYTE (op),
				      GET_MODE (op));
      regno = REGNO (SUBREG_REG (op)) + offset;
    }
  else
    return false;

  return ((regno >= FIRST_PSEUDO_REGISTER 
	   && regno != REGNO (virtual_stack_vars_rtx))
	  || REGNO_REG_CLASS (regno) == DATA_REGS);
})

(define_predicate "ubicom32_address_register_operand"
  (match_code "subreg, reg")
{
  unsigned int regno;

  if (REG_P (op))
    regno = REGNO (op);
  else if (GET_CODE (op) == SUBREG && REG_P (SUBREG_REG (op)))
    {
      int offset;
      if (REGNO (SUBREG_REG (op)) >= FIRST_PSEUDO_REGISTER)
	offset = SUBREG_BYTE (op) / (GET_MODE_SIZE (GET_MODE (op)));
      else
	offset = subreg_regno_offset (REGNO (SUBREG_REG (op)),
				      GET_MODE (SUBREG_REG (op)),
				      SUBREG_BYTE (op),
				      GET_MODE (op));
      regno = REGNO (SUBREG_REG (op)) + offset;
    }
  else
    return false;

  return (regno >= FIRST_PSEUDO_REGISTER 
	  || REGNO_REG_CLASS (regno) == FDPIC_REG
	  || REGNO_REG_CLASS (regno) == LINK_REG
	  || REGNO_REG_CLASS (regno) == SIBCALL_ADDRESS_REGS
	  || REGNO_REG_CLASS (regno) == ADDRESS_REGS);
})

(define_predicate "ubicom32_acc_lo_register_operand"
  (match_code "subreg, reg")
{
  unsigned int regno;

  if (REG_P (op))
    regno = REGNO (op);
  else if (GET_CODE (op) == SUBREG && REG_P (SUBREG_REG (op)))
    {
      int offset;
      if (REGNO (SUBREG_REG (op)) >= FIRST_PSEUDO_REGISTER)
	offset = SUBREG_BYTE (op) / (GET_MODE_SIZE (GET_MODE (op)));
      else
	offset = subreg_regno_offset (REGNO (SUBREG_REG (op)),
				      GET_MODE (SUBREG_REG (op)),
				      SUBREG_BYTE (op),
				      GET_MODE (op));
      regno = REGNO (SUBREG_REG (op)) + offset;
    }
  else
    return false;

  return ((regno >= FIRST_PSEUDO_REGISTER 
	   && regno != REGNO (virtual_stack_vars_rtx))
	  || REGNO_REG_CLASS (regno) == ACC_LO_REGS);
})

(define_predicate "ubicom32_acc_hi_register_operand"
  (match_code "subreg, reg")
{
  unsigned int regno;

  if (REG_P (op))
    regno = REGNO (op);
  else if (GET_CODE (op) == SUBREG && REG_P (SUBREG_REG (op)))
    {
      int offset;
      if (REGNO (SUBREG_REG (op)) >= FIRST_PSEUDO_REGISTER)
	offset = SUBREG_BYTE (op) / (GET_MODE_SIZE (GET_MODE (op)));
      else
	offset = subreg_regno_offset (REGNO (SUBREG_REG (op)),
				      GET_MODE (SUBREG_REG (op)),
				      SUBREG_BYTE (op),
				      GET_MODE (op));
      regno = REGNO (SUBREG_REG (op)) + offset;
    }
  else
    return false;

  return ((regno >= FIRST_PSEUDO_REGISTER 
	   && regno != REGNO (virtual_stack_vars_rtx))
	  || REGNO_REG_CLASS (regno) == ACC_REGS);
})

;; Any ACC register (acc0_hi, acc0_lo, acc1_hi, acc1_lo)
(define_predicate "ubicom32_acc_register_operand"
  (and (match_operand 0 "register_operand")
       (match_test "REGNO(op) >= FIRST_PSEUDO_REGISTER || IS_ACC_REG(op)")))

(define_predicate "ubicom32_call_address_operand"
  (match_code "symbol_ref, subreg, reg")
{
  return (GET_CODE (op) == SYMBOL_REF || REG_P (op));
})

(define_special_predicate "ubicom32_cc_register_operand"
  (and (match_code "reg")
       (match_test "REGNO (op) == CC_REGNUM")))

;; Even data register or acc0_hi/acc1_hi
(define_predicate "ubicom32_dbl_acc_register_operand"
  (match_operand 0 "register_operand")
{
  return (REGNO(op) >= FIRST_PSEUDO_REGISTER)
	  || IS_DOUBLE_REG(op)
	  || IS_ACC_HI_REG(op);
})


