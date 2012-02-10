; Constraint definitions for Ubicom32

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

(define_register_constraint "a" "ALL_ADDRESS_REGS"
  "An An register.")

(define_register_constraint "d" "DATA_REGS"
  "A Dn register.")

(define_register_constraint "h" "ACC_REGS"
  "An accumulator register.")

(define_register_constraint "l" "ACC_LO_REGS"
  "An accn_lo register.")

(define_register_constraint "Z" "FDPIC_REG"
  "The FD-PIC GOT pointer: A0.")

(define_register_constraint "z" "LINK_REG"
  "The link register: A5.")

(define_register_constraint "W" "ADDRESS_REGS"
  "An An register.")

(define_register_constraint "w" "SIBCALL_ADDRESS_REGS"
  "An An register usable for a sibcall target.")

(define_constraint "I"
  "An 8-bit signed constant value."
  (and (match_code "const_int")
       (match_test "(ival >= -128) && (ival <= 127)")))

(define_constraint "Q"
  "An 8-bit signed constant value represented as unsigned."
  (and (match_code "const_int")
       (match_test "(ival >= 0x00) && (ival <= 0xff)")))

(define_constraint "R"
  "An 8-bit signed constant value represented as unsigned."
  (and (match_code "const_int")
       (match_test "((ival >= 0x0000) && (ival <= 0x007f)) || ((ival >= 0xff80) && (ival <= 0xffff))")))

(define_constraint "J"
  "A 7-bit unsigned constant value."
  (and (match_code "const_int")
       (match_test "(ival >= 0) && (ival <= 127)")))

(define_constraint "K"
  "A 7-bit unsigned constant value shifted << 1."
  (and (match_code "const_int")
       (match_test "(ival >= 0) && (ival <= 254) && ((ival & 1) == 0)")))

(define_constraint "L"
  "A 7-bit unsigned constant value shifted << 2."
  (and (match_code "const_int")
       (match_test "(ival >= 0) && (ival <= 508) && ((ival & 3) == 0)")))

(define_constraint "M"
  "A 5-bit unsigned constant value."
  (and (match_code "const_int")
       (match_test "(ival >= 0) && (ival <= 31)")))

(define_constraint "N1"
  "A signed 16 bit constant value."
  (and (match_code "const_int")
       (match_test "(ival >= -32768) && (ival <= 32767)")))

(define_constraint "N4"
  "A signed 16 bit constant value shifted << 2."
  (and (match_code "const_int")
       (match_test "(ival >= -131072) && (ival <= 131071) && ((ival & 3) == 0)")))

(define_constraint "O"
  "An exact bitmask of contiguous 1 bits starting at bit 0."
  (and (match_code "const_int")
       (match_test "exact_log2 (ival + 1) != -1")))

(define_constraint "P"
  "A 7-bit negative constant value shifted << 2."
  (and (match_code "const_int")
       (match_test "(ival >= -504) && (ival <= 0) && ((ival & 3) == 0)")))

(define_constraint "S"
  "A symbolic reference."
  (match_code "symbol_ref"))

(define_constraint "Y1"
  "An FD-PIC GOT index."
  (and (match_test "TARGET_FDPIC")
       (match_test "GET_CODE (op) == UNSPEC")
       (ior (match_test "XINT (op, 1) == UNSPEC_FDPIC_GOT_1")
	    (match_test "XINT (op, 1) == UNSPEC_FDPIC_FUNCDESC_GOT_1"))))

(define_constraint "Y4"
  "An FD-PIC GOT index."
  (and (match_test "TARGET_FDPIC")
       (match_test "GET_CODE (op) == UNSPEC")
       (ior (match_test "XINT (op, 1) == UNSPEC_FDPIC_GOT_4")
	    (match_test "XINT (op, 1) == UNSPEC_FDPIC_FUNCDESC_GOT_4"))))

; A set of offsettable memory operand constraints.  These aren't actually
; used in the compiler backend but are necessary to allow good inline
; assembler code generation.  The 1/2/4 variants enable the inline asm code
; to use the full extent of offsets for the various instruction sizes.
;
; Note that as these operand constraints do allow inc/dec/modify side
; effects then they should only be used where there is a single use of the
; memory operand in an instruction since two side effects are not allowed!

(define_memory_constraint "T1"
  "A memory operand that can be used for .1 instruction."
  (and (match_test "memory_operand (op, GET_MODE(op))")
       (match_test "GET_MODE (op) == QImode")))

(define_memory_constraint "T2"
  "A memory operand that can be used for .2 instruction."
  (and (match_test "memory_operand (op, GET_MODE(op))")
       (match_test "GET_MODE (op) == HImode")))

(define_memory_constraint "T4"
  "A memory operand that can be used for .4 instruction."
  (and (match_test "memory_operand (op, GET_MODE(op))")
       (ior (match_test "GET_MODE (op) == SImode")
	    (match_test "GET_MODE (op) == DImode")
	    (match_test "GET_MODE (op) == SFmode")
	    (match_test "GET_MODE (op) == DFmode"))))

; A set of offsettable memory operand constraints.  These aren't actually
; used in the compiler backend but are necessary to allow good inline
; assembler code generation.  The 1/2/4 variants enable the inline asm code
; to use the full extent of offsets for the various instruction sizes while
; the inability to use inc/dec/modify side effects ensures that the operands
; are safe in situations where 2 uses of the  memory operand are used in a
; single instruction.

(define_memory_constraint "U1"
  "An offsettable memory operand that can be used for .1 instruction."
  (and (match_test "memory_operand (op, GET_MODE(op))")
       (match_test "GET_MODE (op) == QImode")
       (match_test "GET_CODE (XEXP (op, 0)) != POST_INC")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_INC")
       (match_test "GET_CODE (XEXP (op, 0)) != POST_DEC")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_DEC")
       (match_test "GET_CODE (XEXP (op, 0)) != POST_MODIFY")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_MODIFY")))

(define_memory_constraint "U2"
  "An offsettable memory operand that can be used for .2 instruction."
  (and (match_test "memory_operand (op, GET_MODE(op))")
       (match_test "GET_MODE (op) == HImode")
       (match_test "GET_CODE (XEXP (op, 0)) != POST_INC")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_INC")
       (match_test "GET_CODE (XEXP (op, 0)) != POST_DEC")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_DEC")
       (match_test "GET_CODE (XEXP (op, 0)) != POST_MODIFY")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_MODIFY")))

(define_memory_constraint "U4"
  "An offsettable memory operand that can be used for .4 instruction."
  (and (match_test "memory_operand (op, GET_MODE(op))")
       (ior (match_test "GET_MODE (op) == SImode")
	    (match_test "GET_MODE (op) == DImode")
	    (match_test "GET_MODE (op) == SFmode")
	    (match_test "GET_MODE (op) == DFmode"))
       (match_test "GET_CODE (XEXP (op, 0)) != POST_INC")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_INC")
       (match_test "GET_CODE (XEXP (op, 0)) != POST_DEC")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_DEC")
       (match_test "GET_CODE (XEXP (op, 0)) != POST_MODIFY")
       (match_test "GET_CODE (XEXP (op, 0)) != PRE_MODIFY")))

