MACHINE=
SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-ubicom32fdpic"
TEXT_START_ADDR=0x000000
MAXPAGESIZE=0x4000
TARGET_PAGE_SIZE=0x1000
NONPAGED_TEXT_START_ADDR=${TEXT_START_ADDR}
ARCH=ubicom32
TEMPLATE_NAME=elf32
ENTRY=_start
EMBEDDED=yes
GENERATE_SHLIB_SCRIPT=yes
EMBEDDED= # This gets us program headers mapped as part of the text segment.
OTHER_GOT_SYMBOLS=
OTHER_READONLY_SECTIONS="
  .rofixup        : {
    ${RELOCATING+__ROFIXUP_LIST__ = .;}
    *(.rofixup)
    ${RELOCATING+__ROFIXUP_END__ = .;}
  }
"
ELFSIZE=32
DATA_START_SYMBOLS=
CTOR_START='___ctors = .;'
CTOR_END='___ctors_end = .;'
DTOR_START='___dtors = .;'
DTOR_END='___dtors_end = .;'
# This code gets inserted into the generic elf32.sc linker script
# and allows us to define our own command line switches.
PARSE_AND_LIST_PROLOGUE='
#define OPTION_SET_STACK_SIZE		300
'

PARSE_AND_LIST_LONGOPTS='
  {"stack-size", required_argument, NULL, OPTION_SET_STACK_SIZE},
'

PARSE_AND_LIST_OPTIONS='
  fprintf (file, _("  --stack-size=<stacksize>    Set stack size to value specified.\n"));
'

PARSE_AND_LIST_ARGS_CASES='
    case OPTION_SET_STACK_SIZE:
      {
        char *end;
        extern long ubicom32fdpic_stack_size;
        long tmp_val = strtoul (optarg, &end, 0);
        if (end == optarg)
           einfo (_("%P%F: invalid hex number for stack size parameter '%s'\n"), optarg);
	else
	   ubicom32fdpic_stack_size = tmp_val;

      }
      break;
'
