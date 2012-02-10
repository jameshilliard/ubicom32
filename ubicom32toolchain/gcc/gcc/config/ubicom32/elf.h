#undef STARTFILE_SPEC
#define STARTFILE_SPEC \
  "crti%O%s crtbegin%O%s"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC \
  "crtend%O%s crtn%O%s"

#ifdef __UBICOM32_FDPIC__
#define CRT_CALL_STATIC_FUNCTION(SECTION_OP, FUNC)			\
  asm (SECTION_OP);							\
  asm ("move.4 a0, 0(sp);\n\t"						\
       "call a5," USER_LABEL_PREFIX #FUNC ";");				\
  asm (TEXT_SECTION_ASM_OP);
#endif

#define NO_IMPLICIT_EXTERN_C

