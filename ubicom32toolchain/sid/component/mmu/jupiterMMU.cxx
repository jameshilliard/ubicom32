/*
** jupiterMMU.cxx - An implementation of the Ubicom jupiter MMU
** by Toshiyasu Morita
** Copyright (C) 2009 Ubicom, Inc.
**
**
** Quick intro to the MMU design:
**
** This code was originally derived from the armRemap.cxx file, but has been
** extensively modified.
**
** The MMU has seven buses:
**
** data-access-port		(used by the CPU for data access)
** insn-access-port		(used by the CPU for instruction access)
** data-tlb-access-port		(used by sidrefcompare to update the data TLB)
** insn-tlb-access-port		(used by sidrefcompare to update the instruction TLB)
** thread-num-access-port	(used by sidrefcompare to set the thread number)
** privilege-level-access-port	(used by sidrefcompare to set the supervisor bit)
** log-mmu-read-port		(used by sidrefcompare to inform sid of MMU read accesses)
**
** All instruction access should be done through the insn-access-port
** and all data access should be performed through the data-acess-port.
**
** All data TLB accesses should be performed through the data-tlb-access-port
** and all instruction TLB accesses through the insn-tlb-access-port.
** All TLB accesses must be performed as 64-bit reads and writes.
**
** Note:
**
** This MMU only implements the features required for proper operation by
** sidrefcompare. This MMU is NOT usable in a standalone configuration
** without sidrefcompare.
**
** The following features are intentionally not implemented:
**
** 1) TLB replacement policy
**
**    The Jupiter processor performs a TLB lookup before the instruction
**    is determined to be either cancelled or graduated. Practically speaking,
**    this means that instructions which are never executed (such as those
**    immediately after a branch which is taken) affect the state of the
**    pseudo-LRU algorithm.
**
**    The TLB accesses are contained in a separate trace file and 
**    there is a separate utility (rpcheck) which implements the TLB 
**    replacement policy and runs in lockstep with the trace file.
**
**    Therefore, the TLB replacement policy is not implemented in this file.
**
** 2) Hardware TLB miss handler and PTEC
**
**    This file does not implement the TLB miss handler, but it does check
**    the behavior of the hardware MMU.
**
**    Specifically, we monitor:
**
**    1) The "MMU Read" lines in the trace file to see which OCM memory
**       locations are accessed by the MMU. The info from these lines is
**       stored in a TLB preload buffer.
**
**    2) The "TLB Update" lines in the trace files to see which values
**       are loaded into the TLB entries. The values which are loaded into
**       each TLB entry are checked by calculating the attributes of the
**       PTEC way 0 and way 1 entries and ensuring the TLB attributes matches
**       one of the two entries.
**
** 3) MMU exception queue
**
**    The data read from the queue is provided in the RTL trace file so
**    this is not simulated.
**
** 4) TLB miss handler thread wakeup
**
**    The information about which thread is currently executing is provided
**    by the RTL trace file. Therfore, the simulator does not "choose" which
**    thread to execute, so it is not necessary to simulate TLB miss handler
**    thread wakeup.
**
** Basically, we assume there is never a TLB miss because the RTL trace file
** should have information to preload the TLB entry before the instruction
** which uses the TLB entry is executed. If a TLB miss does actually occur,
** this is treated as an error and an error message is printed and abort()
** is called.
**
** NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
**
** The primary purpose of this code is twofold:
**
** 1) Implement the hardware MMU algorithms in a straightforward and
**    easily-debuggable format
** 
** 2) Provide a large amount of useful debugging information (when enabled)
**    to facilitate debugging of hardware features
**
** Therefore this code is NOT designed for speed.
**
** NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
**
** Common abbreviations used in this file:
**
** vaddr = virtal address
** paddr = physical address
** ASID  = Address Space IDentifier
** TLB   = Translation Lookaside Buffer
** DTLB  = Data TLB
** ITLB  = Instruction TLB
** PTE   = Process Table Entry
** PPN   = Physical Page Number
** VPN   = Virtual Page Number
*/

/* General configuration macros */
#define TRANSLATED_ADDRESS(x)		(x < 0xb0000000)
#define MMU_REGISTER_START_ADDRESS	0xb9000e00
#define MMU_REGISTER_END_ADDRESS	0xb9001000
#define MMU_REGISTERS_NUM		((MMU_REGISTER_END_ADDRESS - MMU_REGISTER_START_ADDRESS) / 4)

// Number of hardware threads
#define HW_THREADS_NUM		12

// The number of hardware TLB entries
#define TLB_ENTRIES_NUM		64
#define TLB_ENTRIES_SHIFTS_NUM	6

#define USE_NONCACHEABLE_BIT

/*************************************
* Start MMU configuration macros 
*************************************/

/* MMU page size - 1 = 8kbytes, 3 = 16kbytes */

#define PAGE_MASK			(MMU_register[0] & 3)

#ifdef USE_NONCACHEABLE_BIT
#define RTAG_MASK			((MMU_register[4 >> 2] & ((1 << 7) - 1)) >> 1)
#else
#define RTAG_MASK			(MMU_register[4 >> 2] & ((1 << 7) - 1))
#endif

#define IS_OCM_ADDRESS(x) ((x >= 0xbffc0000) && (x < 0xc0000000))

/*************************************
* End MMU configuration macros 
*************************************/
/*************************************
* Start TLB entry macros 
*************************************/

#ifdef USE_NONCACHEABLE_BIT

/*
** The Jupiter hardware (and software) TLB value is in this format:
**
** |vpn|asid|ppn|r|w|x|s|v|
**
** where:
**
** field    description          # of  bits   bit positions
** --------------------------------------------------------
** vpn     virtual page number    19 bits       bits 53:35
** asid    address space ID       10 bits       bits 34:25
** ppn     physical page number   19 bits       bits 24:6
** nocache noncacheable bit        1 bit	bit  5  (unused by sidrefcompare)
** r       read permission         1 bit        bit  4 
** w       write permission        1 bit        bit  3 
** x       execute permission      1 bit        bit  2 
** s       supervisor permission   1 bit        bit  1
** v       valid qualifier         1 bit        bit  0
**
** IMPORTANT NOTE IMPORTANT NOTE IMPORTANT NOTE IMPORTANT NOTE
**
** The "supervisor permission" bit is inverted!
**
** 0 = supervisor mode
** 1 = user mode
**
** IMPORTANT NOTE IMPORTANT NOTE IMPORTANT NOTE IMPORTANT NOTE
*/

/*
** Note: "tlb" is a long long * (pointer to a 64-bit value) macros should return int
** in case the code prints the value with %d or %x.
*/

/* The upper 19 bits of the virtual page of the TLB entry (19 bits) */
#define TLB_ENTRY_VPN(tlb_value)		((int)(tlb_value >> 35) & 0x7ffff)

/* The ASID of the TLB entry (10 bits) */
#define TLB_ENTRY_ASID(tlb_value)		((int)(tlb_value >> 25) & 0x3ff)

/* The physical page page number of the TLB entry (19 bits) */
#define TLB_ENTRY_PPN(tlb_value)		((int)(tlb_value >> 6) & 0x7ffff)

/* The read permission bit for the TLB entry (1 bit) */
#define TLB_ENTRY_READ_BIT(tlb_value)		((int)(tlb_value >> 4) & 1)

/* The write permission bit for the TLB entry (1 bit) */
#define TLB_ENTRY_WRITE_BIT(tlb_value)		((int)(tlb_value >> 3) & 1)

/* The execute permission bit for the TLB entry (1 bit) */
#define TLB_ENTRY_EXECUTE_BIT(tlb_value)	((int)(tlb_value >> 2) & 1)

/* The supervisor permission bit for the TLB entry (1 bit) */
/* 0 = supervisor mode, 1 = user mode - this is the way the hardware works! */
#define TLB_ENTRY_SUPERVISOR_BIT(tlb_value)	((int)(tlb_value >> 1) & 1)

/* The valid bit for the TLB entry (1 bit) */
#define TLB_ENTRY_VALID_BIT(tlb_value)		((int)(tlb_value >> 0) & 1)

#else

/*
** The Jupiter hardware (and software) TLB value is in this format:
**
** |vpn|asid|ppn|r|w|x|s|v|
**
** where:
**
** field    description          # of  bits   bit positions
** --------------------------------------------------------
** vpn    virtual page number    19 bits       bits 52:34
** asid   address space ID       10 bits       bits 33:24
** ppn    physical page number   19 bits       bits 23:5
** r      read permission         1 bit        bit  4 
** w      write permission        1 bit        bit  3 
** x      execute permission      1 bit        bit  2 
** s      supervisor permission   1 bit        bit  1
** v      valid qualifier         1 bit        bit  0
**
** IMPORTANT NOTE IMPORTANT NOTE IMPORTANT NOTE IMPORTANT NOTE
**
** The "supervisor permission" bit is inverted!
**
** 0 = supervisor mode
** 1 = user mode
**
** IMPORTANT NOTE IMPORTANT NOTE IMPORTANT NOTE IMPORTANT NOTE
*/

/*
** Note: "tlb" is a long long * (pointer to a 64-bit value) macros should return int
** in case the code prints the value with %d or %x.
*/

/* The upper 19 bits of the virtual page of the TLB entry (19 bits) */
#define TLB_ENTRY_VPN(tlb_value)		((int)(tlb_value >> 34) & 0x7ffff)

/* The ASID of the TLB entry (10 bits) */
#define TLB_ENTRY_ASID(tlb_value)		((int)(tlb_value >> 24) & 0x3ff)

/* The physical page page number of the TLB entry (19 bits) */
#define TLB_ENTRY_PPN(tlb_value)		((int)(tlb_value >> 5) & 0x7ffff)

/* The read permission bit for the TLB entry (1 bit)*/
#define TLB_ENTRY_READ_BIT(tlb_value)		((int)(tlb_value >> 4) & 1)

/* The write permission bit for the TLB entry (1 bit) */
#define TLB_ENTRY_WRITE_BIT(tlb_value)		((int)(tlb_value >> 3) & 1)

/* The execute permission bit for the TLB entry (1 bit) */
#define TLB_ENTRY_EXECUTE_BIT(tlb_value)	((int)(tlb_value >> 2) & 1)

/* The supervisor permission bit for the TLB entry (1 bit) */
/* 0 = supervisor mode, 1 = user mode - this is the way the hardware works! */
#define TLB_ENTRY_SUPERVISOR_BIT(tlb_value)	((int)(tlb_value >> 1) & 1)

/* The valid bit for the TLB entry (1 bit) */
#define TLB_ENTRY_VALID_BIT(tlb_value)		((int)(tlb_value >> 0) & 1)

#endif

/*************************************
* End TLB entry macros 
*************************************/
/*************************************
* MMU register offsets
*************************************/

/*
** Note: The values in the Jupiter Chip Specification section 9.1: MMU registers are BYTE indexes.
**       These are INT indexes (byte index / 4) 
*/

#define CONFIG		0
#define PTEC_CFG0	1
#define PTEC_CFG1	2
#define PTEC_CFG2	3
#define MISSQW0		4
#define MISSQW1		5
#define RESTART		6
#define INSERTW0	7
#define INSERTW1	8
#define I_PURGE		9
#define D_PURGE		10
#define PTEC_ERR	11
#define TLB_IDX		12
#define ENTRYW0		13
#define ENTRYW21	14
#define STATSU		15

/*************************************
* MMU register offsets
*************************************/
/*************************************
* Start ASID access macros 
*************************************/

#define ASID0(thread_num) 	((MMU_register[(0x104 + (thread_num << 4)) >> 2] >>  0) & 0x3ff)
#define ASID1(thread_num) 	((MMU_register[(0x104 + (thread_num << 4)) >> 2] >> 10) & 0x3ff)
#define ASID2(thread_num) 	((MMU_register[(0x104 + (thread_num << 4)) >> 2] >> 20) & 0x3ff)

#define CMP_0_EN(thread_num)	((MMU_register[(0x108 + (thread_num << 4)) >> 2] >> 24) & 1)
#define CMP_1_EN(thread_num)	((MMU_register[(0x108 + (thread_num << 4)) >> 2] >> 25) & 1)
#define CMP_2_EN(thread_num)	((MMU_register[(0x108 + (thread_num << 4)) >> 2] >> 26) & 1)

#define ASID_CMP0(thread_num)	((MMU_register[(0x108 + (thread_num << 4)) >> 2] >>  0) & 0xf)
#define ASID_CMP1(thread_num)	((MMU_register[(0x108 + (thread_num << 4)) >> 2] >>  8) & 0xf)
#define ASID_CMP2(thread_num)	((MMU_register[(0x108 + (thread_num << 4)) >> 2] >> 16) & 0xf)

#define ASID_MASK0(thread_num)	((MMU_register[(0x10c + (thread_num << 4)) >> 2] >>   0) & 0xf)
#define ASID_MASK1(thread_num)	((MMU_register[(0x10c + (thread_num << 4)) >> 2] >>   8) & 0xf)
#define ASID_MASK2(thread_num)	((MMU_register[(0x10c + (thread_num << 4)) >> 2] >>  16) & 0xf)

/*************************************
* End ASID access macros 
*************************************/
/*************************************
* Start PTEC access macros 
*************************************/

#define PTEC_ENABLED		(MMU_register[0] & (1 << 4))

#define ATAG_BASE		(MMU_register[0x8 >> 2] & (((1 << 25) - 1) << 7))
#define ENTRY_BASE		(MMU_register[0xc >> 2] & (((1 << 25) - 1) << 7))

/* This is the equivalent of "ENTRY_BITS" in the manual, but renamed to be less confusing */

#define PTEC_ENTRIES_LOG2	((MMU_register[0x4 >> 2] >> 16) & ((1 << 5) - 1))

#define PTEC_ENTRIES_MASK 	((1 << PTEC_ENTRIES_LOG2) - 1)

#define EXTRACT_BITS_FROM_INT32(value, right_shift, bits_num)		(((unsigned int)value >> right_shift) & ((1 << bits_num) - 1))

#define PTEC_TAG0(atag_value)	EXTRACT_BITS_FROM_INT32(atag_value, 26,  6)
#define PTEC_TAG1(atag_value)	EXTRACT_BITS_FROM_INT32(atag_value, 10,  6)
#define PTEC_ASID0(atag_value)	EXTRACT_BITS_FROM_INT32(atag_value, 16, 10)
#define PTEC_ASID1(atag_value)	EXTRACT_BITS_FROM_INT32(atag_value,  0, 10)

typedef enum {

	EXPECTING_PTEC_ATAG_READ,
	EXPECTING_PTEC_ATAG_OR_WAY0_OR_WAY1_READ,
	EXPECTING_PTEC_WAY1_READ,
	EXPECTING_PTEC_WAY1_OR_ATAG_READ

} MMU_PTEC_STATE;

/*************************************
* End PTEC access macros 
*************************************/
/*************************************
* Start MMU read buffer definitions
*************************************/

/* When the CPU starts large number of threads before the DDR controller has initialized, the CPU
   will generate a large number of consecutive MMU reads (empiricaly observed over 90).
   The maximum theoretical number of consecutive MMU reads is 32 * 2 * 3 = 192, and the next
   larger power of two is 256. */

#define MMU_READ_BUFFER_ENTRIES 256

typedef struct {

	int paddr, value;

} MMU_READ_BUFFER_ENTRY;

/*************************************
* Start MMU read buffer definitions
*************************************/

#include "config.h"
#include "tconfig.h"
 
#include <sidcomp.h>
#include <sidso.h>
#include <sidcomputil.h>
#include <sidattrutil.h>
#include <sidmiscutil.h>
#include <sidpinutil.h>
#include <sidbusutil.h>
#include <sidtypes.h>
#include <sidwatchutil.h>
#include <stdint.h>

using std::map;
using std::vector;
using std::string;
using std::ostream;
using std::istream;
using std::pair;
using std::ios;

using sid::component;
using sid::bus;
using sid::host_int_1;
using sid::host_int_4;
using sid::little_int_1;
using sid::little_int_2;
using sid::little_int_4;
using sid::little_int_8;
using sid::big_int_1;
using sid::big_int_2;
using sid::big_int_4;
using sid::big_int_8;
using sid::component_library;
using sid::COMPONENT_LIBRARY_MAGIC;

using sidutil::attribute_coder_base;
using sidutil::attribute_coder_virtual_parameterized;
using sidutil::fixed_attribute_map_component;
using sidutil::fixed_bus_map_component;
using sidutil::fixed_pin_map_component;
using sidutil::fixed_accessor_map_component;
using sidutil::make_attribute;
using sidutil::parse_attribute;
using sidutil::no_relation_component;
using sidutil::callback_pin;
using sidutil::output_pin;
using sidutil::word_bus;
using sidutil::self_watcher;
using sidutil::tokenize;

#include <map>
#include <vector>
#include <string>

typedef long long int TLB_ENTRY;

/* These globals are ugly kludges because it's so difficult to communicate to the MMU from sidrefcompare */

extern int running_flag;
int debug_mmu_flag = 0;
int check_ptec_flag = 1;

static void jupiterMMU_abort()
{
	printf("Rerun sidrefcompare with -d flag to enable debug output\n");
	printf("** FAIL **\n");
	abort();
}

class jupiterMMU:

	public virtual component,
	protected fixed_attribute_map_component,
	protected fixed_bus_map_component,
	protected fixed_pin_map_component,
	protected fixed_accessor_map_component,
	protected no_relation_component
{
	public:
		jupiterMMU();
		~jupiterMMU () throw () {}

		/* These methods are called by the methods in the insn_access and data_access classes */

		bus::status write(host_int_4 address, little_int_1 data, int insn_flag) throw();
		bus::status write(host_int_4 address, little_int_2 data, int insn_flag) throw();
		bus::status write(host_int_4 address, little_int_4 data, int insn_flag) throw();
		bus::status write(host_int_4 address, little_int_8 data, int insn_flag) throw();
		bus::status write(host_int_4 address, big_int_1 data, int insn_flag) throw();
		bus::status write(host_int_4 address, big_int_2 data, int insn_flag) throw();
		bus::status write(host_int_4 address, big_int_4 data, int insn_flag) throw();
		bus::status write(host_int_4 address, big_int_8 data, int insn_flag) throw();

		bus::status read(host_int_4 address, little_int_1 &data, int insn_flag) throw();
		bus::status read(host_int_4 address, little_int_2 &data, int insn_flag) throw();
		bus::status read(host_int_4 address, little_int_4 &data, int insn_flag) throw();
		bus::status read(host_int_4 address, little_int_8 &data, int insn_flag) throw();
		bus::status read(host_int_4 address, big_int_1 &data, int insn_flag) throw();
		bus::status read(host_int_4 address, big_int_2 &data, int insn_flag) throw();
		bus::status read(host_int_4 address, big_int_4 &data, int insn_flag) throw();
		bus::status read(host_int_4 address, big_int_8 &data, int insn_flag) throw();

		long long int reformat_PTEC_to_TLB_format(int tag, int entry, int index);
		void check_TLB_load_against_PTEC(TLB_ENTRY *tlb, host_int_4 address, long long int data);

		bus::status TLB_write(host_int_4 address, little_int_1 data, int insn_flag) throw();
		bus::status TLB_write(host_int_4 address, little_int_2 data, int insn_flag) throw();
		bus::status TLB_write(host_int_4 address, little_int_4 data, int insn_flag) throw();
		bus::status TLB_write(host_int_4 address, little_int_8 data, int insn_flag) throw();
		bus::status TLB_write(host_int_4 address, big_int_1 data, int insn_flag) throw();
		bus::status TLB_write(host_int_4 address, big_int_2 data, int insn_flag) throw();
		bus::status TLB_write(host_int_4 address, big_int_4 data, int insn_flag) throw();
		bus::status TLB_write(host_int_4 address, big_int_8 data, int insn_flag) throw();

		bus::status TLB_read(host_int_4 address, little_int_1 &data, int insn_flag) throw();
		bus::status TLB_read(host_int_4 address, little_int_2 &data, int insn_flag) throw();
		bus::status TLB_read(host_int_4 address, little_int_4 &data, int insn_flag) throw();
		bus::status TLB_read(host_int_4 address, little_int_8 &data, int insn_flag) throw();
		bus::status TLB_read(host_int_4 address, big_int_1 &data, int insn_flag) throw();
		bus::status TLB_read(host_int_4 address, big_int_2 &data, int insn_flag) throw();
		bus::status TLB_read(host_int_4 address, big_int_4 &data, int insn_flag) throw();
		bus::status TLB_read(host_int_4 address, big_int_8 &data, int insn_flag) throw();

		bus::status snoop_MMU_read(int address) throw();

	private:

		class data_access: public bus
		{
			class jupiterMMU *mmu;

			public:

				data_access(jupiterMMU *mmu) : mmu(mmu) { };

				bus::status write(host_int_4 address, little_int_1 data) throw() {
					return mmu->write(address, data, 0);
				}
				bus::status write(host_int_4 address, little_int_2 data) throw() {
					return mmu->write(address, data, 0);
				}
				bus::status write(host_int_4 address, little_int_4 data) throw() {
					return mmu->write(address, data, 0);
				}
				bus::status write(host_int_4 address, little_int_8 data) throw() {
					return mmu->write(address, data, 0);
				}
				bus::status write(host_int_4 address, big_int_1 data) throw() {
					return mmu->write(address, data, 0);
				}
				bus::status write(host_int_4 address, big_int_2 data) throw() {
					return mmu->write(address, data, 0);
				}
				bus::status write(host_int_4 address, big_int_4 data) throw() {
					return mmu->write(address, data, 0);
				}
				bus::status write(host_int_4 address, big_int_8 data) throw() {
					return mmu->write(address, data, 0);
				}
				bus::status read(host_int_4 address, little_int_1 &data) throw() {
					return mmu->read(address, data, 0);
				}
				bus::status read(host_int_4 address, little_int_2 &data) throw() {
					return mmu->read(address, data, 0);
				}
				bus::status read(host_int_4 address, little_int_4 &data) throw() {
					return mmu->read(address, data, 0);
				}
				bus::status read(host_int_4 address, little_int_8 &data) throw() {
					return mmu->read(address, data, 0);
				}
				bus::status read(host_int_4 address, big_int_1 &data) throw() {
					return mmu->read(address, data, 0);
				}
				bus::status read(host_int_4 address, big_int_2 &data) throw() {
					return mmu->read(address, data, 0);
				}
				bus::status read(host_int_4 address, big_int_4 &data) throw() {
					return mmu->read(address, data, 0);
				}
				bus::status read(host_int_4 address, big_int_8 &data) throw() {
					return mmu->read(address, data, 0);
				}
			};

		class insn_access: public bus
		{
			class jupiterMMU *mmu;

			public:

				insn_access(jupiterMMU *mmu) : mmu(mmu) { };

				bus::status write(host_int_4 address, little_int_1 data) throw() {
					return mmu->write(address, data, 1);
				}
				bus::status write(host_int_4 address, little_int_2 data) throw() {
					return mmu->write(address, data, 1);
				}
				bus::status write(host_int_4 address, little_int_4 data) throw() {
					return mmu->write(address, data, 1);
				}
				bus::status write(host_int_4 address, little_int_8 data) throw() {
					return mmu->write(address, data, 1);
				}
				bus::status write(host_int_4 address, big_int_1 data) throw() {
					return mmu->write(address, data, 1);
				}
				bus::status write(host_int_4 address, big_int_2 data) throw() {
					return mmu->write(address, data, 1);
				}
				bus::status write(host_int_4 address, big_int_4 data) throw() {
					return mmu->write(address, data, 1);
				}
				bus::status write(host_int_4 address, big_int_8 data) throw() {
					return mmu->write(address, data, 1);
				}
				bus::status read(host_int_4 address, little_int_1 &data) throw() {
					return mmu->read(address, data, 1);
				}
				bus::status read(host_int_4 address, little_int_2 &data) throw() {
					return mmu->read(address, data, 1);
				}
				bus::status read(host_int_4 address, little_int_4 &data) throw() {
					return mmu->read(address, data, 1);
				}
				bus::status read(host_int_4 address, little_int_8 &data) throw() {
					return mmu->read(address, data, 1);
				}
				bus::status read(host_int_4 address, big_int_1 &data) throw() {
					return mmu->read(address, data, 1);
				}
				bus::status read(host_int_4 address, big_int_2 &data) throw() {
					return mmu->read(address, data, 1);
				}
				bus::status read(host_int_4 address, big_int_4 &data) throw() {
					return mmu->read(address, data, 1);
				}
				bus::status read(host_int_4 address, big_int_8 &data) throw() {
					return mmu->read(address, data, 1);
				}
		};

		class data_TLB_access: public bus
		{
			class jupiterMMU *mmu;

			public:

				data_TLB_access(jupiterMMU *mmu) : mmu(mmu) { };

				bus::status write(host_int_4 address, little_int_1 data) throw() {
					mmu->TLB_write(address, data, 0);
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_2 data) throw() {
					mmu->TLB_write(address, data, 0);
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_4 data) throw() {
					mmu->TLB_write(address, data, 0);
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_8 data) throw() {
					mmu->TLB_write(address, data, 0);
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_1 data) throw() {
					mmu->TLB_write(address, data, 0);
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_2 data) throw() {
					mmu->TLB_write(address, data, 0);
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_4 data) throw() {
					mmu->TLB_write(address, data, 0);
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_8 data) throw() {
					mmu->TLB_write(address, data, 0);
					return bus::ok;
				}

				bus::status read(host_int_4 address, little_int_1 &data) throw() {
					mmu->TLB_read(address, data, 0);
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_2 &data) throw() {
					mmu->TLB_read(address, data, 0);
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_4 &data) throw() {
					mmu->TLB_read(address, data, 0);
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_8 &data) throw() {
					mmu->TLB_read(address, data, 0);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_1 &data) throw() {
					mmu->TLB_read(address, data, 0);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_2 &data) throw() {
					mmu->TLB_read(address, data, 0);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_4 &data) throw() {
					mmu->TLB_read(address, data, 0);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_8 &data) throw() {
					mmu->TLB_read(address, data, 0);
					return bus::ok;
				}
		};

		class insn_TLB_access: public bus
		{
			class jupiterMMU *mmu;

			public:

				insn_TLB_access(jupiterMMU *mmu) : mmu(mmu) { };

				bus::status write(host_int_4 address, little_int_1 data) throw() {
					mmu->TLB_write(address, data, 1);
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_2 data) throw() {
					mmu->TLB_write(address, data, 1);
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_4 data) throw() {
					mmu->TLB_write(address, data, 1);
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_8 data) throw() {
					mmu->TLB_write(address, data, 1);
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_1 data) throw() {
					mmu->TLB_write(address, data, 1);
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_2 data) throw() {
					mmu->TLB_write(address, data, 1);
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_4 data) throw() {
					mmu->TLB_write(address, data, 1);
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_8 data) throw() {
					mmu->TLB_write(address, data, 1);
					return bus::ok;
				}

				bus::status read(host_int_4 address, little_int_1 &data) throw() {
					mmu->TLB_read(address, data, 1);
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_2 &data) throw() {
					mmu->TLB_read(address, data, 1);
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_4 &data) throw() {
					mmu->TLB_read(address, data, 1);
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_8 &data) throw() {
					mmu->TLB_read(address, data, 1);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_1 &data) throw() {
					mmu->TLB_read(address, data, 1);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_2 &data) throw() {
					mmu->TLB_read(address, data, 1);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_4 &data) throw() {
					mmu->TLB_read(address, data, 1);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_8 &data) throw() {
					mmu->TLB_read(address, data, 1);
					return bus::ok;
				}
		};

		/* Bus for setting the thread_number */

		class thread_num_access: public bus
		{
			class jupiterMMU *mmu;

			public:

				thread_num_access(jupiterMMU *mmu) : mmu(mmu) { };

				bus::status write(host_int_4 address, little_int_1 data) throw() {

					if (data >= HW_THREADS_NUM) {
						printf("Tried to set illegal thread number %d (max thread num is %d)\n", (int)data, HW_THREADS_NUM-1);
						jupiterMMU_abort();
					}

					mmu->current_thread_num = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_2 data) throw() {

					if (data >= HW_THREADS_NUM) {
						printf("Tried to set illegal thread number %d (max thread num is %d)\n", (int)data, HW_THREADS_NUM-1);
						jupiterMMU_abort();
					}

					mmu->current_thread_num = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_4 data) throw() {

					if (data >= HW_THREADS_NUM) {
						printf("Tried to set illegal thread number %d (max thread num is %d)\n", (int)data, HW_THREADS_NUM-1);
						jupiterMMU_abort();
					}

					mmu->current_thread_num = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_8 data) throw() {

					if (data >= HW_THREADS_NUM) {
						printf("Tried to set illegal thread number %d (max thread num is %d)\n", (int)data, HW_THREADS_NUM-1);
						jupiterMMU_abort();
					}

					mmu->current_thread_num = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_1 data) throw() {

					if (data >= HW_THREADS_NUM) {
						printf("Tried to set illegal thread number %d (max thread num is %d)\n", (int)data, HW_THREADS_NUM-1);
						jupiterMMU_abort();
					}

					mmu->current_thread_num = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_2 data) throw() {

					if (data >= HW_THREADS_NUM) {
						printf("Tried to set illegal thread number %d (max thread num is %d)\n", (int)data, HW_THREADS_NUM-1);
						jupiterMMU_abort();
					}

					mmu->current_thread_num = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_4 data) throw() {

					if (data >= HW_THREADS_NUM) {
						printf("Tried to set illegal thread number %d (max thread num is %d)\n", (int)data, HW_THREADS_NUM-1);
						jupiterMMU_abort();
					}

					if (debug_mmu_flag)
						printf("jupiterMMU: current_thread_num set to %d\n", (int)data);

					mmu->current_thread_num = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_8 data) throw() {

					if (data >= HW_THREADS_NUM) {
						printf("Tried to set illegal thread number %d (max thread num is %d)\n", (int)data, HW_THREADS_NUM-1);
						jupiterMMU_abort();
					}

					mmu->current_thread_num = data;
					return bus::ok;
				}

				bus::status read(host_int_4 address, little_int_1 &data) throw() {
					data = mmu->current_thread_num;
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_2 &data) throw() {
					data = mmu->current_thread_num;
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_4 &data) throw() {
					data = mmu->current_thread_num;
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_8 &data) throw() {
					data = mmu->current_thread_num;
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_1 &data) throw() {
					data = mmu->current_thread_num;
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_2 &data) throw() {
					data = mmu->current_thread_num;
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_4 &data) throw() {
					data = mmu->current_thread_num;
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_8 &data) throw() {
					data = mmu->current_thread_num;
					return bus::ok;
				}
		};

		/* Bus for setting the privilege level */

		class privilege_level_access: public bus
		{
			class jupiterMMU *mmu;

			public:

				privilege_level_access(jupiterMMU *mmu) : mmu(mmu) { };

				bus::status write(host_int_4 address, little_int_1 data) throw() {

					if (data & ~1) {
						printf("Privilege level must be 0 or 1, but was set to %d\n", (int)data);
						jupiterMMU_abort();
					}

					mmu->privilege_level = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_2 data) throw() {

					if (data & ~1) {
						printf("Privilege level must be 0 or 1, but was set to %d\n", (int)data);
						jupiterMMU_abort();
					}

					mmu->privilege_level = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_4 data) throw() {

					if (data & ~1) {
						printf("Privilege level must be 0 or 1, but was set to %d\n", (int)data);
						jupiterMMU_abort();
					}

					mmu->privilege_level = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, little_int_8 data) throw() {

					if (data & ~1) {
						printf("Privilege level must be 0 or 1, but was set to %d\n", (int)data);
						jupiterMMU_abort();
					}

					mmu->privilege_level = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_1 data) throw() {

					if (data & ~1) {
						printf("Privilege level must be 0 or 1, but was set to %d\n", (int)data);
						jupiterMMU_abort();
					}

					mmu->privilege_level = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_2 data) throw() {

					if (data & ~1) {
						printf("Privilege level must be 0 or 1, but was set to %d\n", (int)data);
						jupiterMMU_abort();
					}

					mmu->privilege_level = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_4 data) throw() {

					if (data & ~1) {
						printf("Privilege level must be 0 or 1, but was set to %d\n", (int)data);
						jupiterMMU_abort();
					}

					if (debug_mmu_flag)
						printf("jupiterMMU: privilege_level set to %d\n", (int)data);

					mmu->privilege_level = data;
					return bus::ok;
				}
				bus::status write(host_int_4 address, big_int_8 data) throw() {

					if (data & ~1) {
						printf("Privilege level must be 0 or 1, but was set to %d\n", (int)data);
						jupiterMMU_abort();
					}

					mmu->privilege_level = data;
					return bus::ok;
				}

				bus::status read(host_int_4 address, little_int_1 &data) throw() {
					data = mmu->privilege_level;
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_2 &data) throw() {
					data = mmu->privilege_level;
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_4 &data) throw() {
					data = mmu->privilege_level;
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_8 &data) throw() {
					data = mmu->privilege_level;
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_1 &data) throw() {
					data = mmu->privilege_level;
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_2 &data) throw() {
					data = mmu->privilege_level;
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_4 &data) throw() {
					data = mmu->privilege_level;
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_8 &data) throw() {
					data = mmu->privilege_level;
					return bus::ok;
				}
		};

		class snoop_MMU_read_access: public bus
		{
			class jupiterMMU *mmu;

			public:

				snoop_MMU_read_access(jupiterMMU *mmu) : mmu(mmu) { };

				bus::status write(host_int_4 address, little_int_1 data) throw() {
					return bus::unmapped;
				}
				bus::status write(host_int_4 address, little_int_2 data) throw() {
					return bus::unmapped;
				}
				bus::status write(host_int_4 address, little_int_4 data) throw() {
					return bus::unmapped;
				}
				bus::status write(host_int_4 address, little_int_8 data) throw() {
					return bus::unmapped;
				}
				bus::status write(host_int_4 address, big_int_1 data) throw() {
					return bus::unmapped;
				}
				bus::status write(host_int_4 address, big_int_2 data) throw() {
					return bus::unmapped;
				}
				bus::status write(host_int_4 address, big_int_4 data) throw() {
					return bus::unmapped;
				}
				bus::status write(host_int_4 address, big_int_8 data) throw() {
					return bus::unmapped;
				}
				bus::status read(host_int_4 address, little_int_1 &data) throw() {
					return bus::unmapped;
				}
				bus::status read(host_int_4 address, little_int_2 &data) throw() {
					return bus::unmapped;
				}
				bus::status read(host_int_4 address, little_int_4 &data) throw() {
					mmu->snoop_MMU_read((int)address);
					return bus::ok;
				}
				bus::status read(host_int_4 address, little_int_8 &data) throw() {
					return bus::unmapped;
				}
				bus::status read(host_int_4 address, big_int_1 &data) throw() {
					return bus::unmapped;
				}
				bus::status read(host_int_4 address, big_int_2 &data) throw() {
					return bus::unmapped;
				}
				bus::status read(host_int_4 address, big_int_4 &data) throw() {
					mmu->snoop_MMU_read((int)address);
					return bus::ok;
				}
				bus::status read(host_int_4 address, big_int_8 &data) throw() {
					return bus::unmapped;
				}
		};

		void debug_TLB_entry(int insn_flag, int tlb_index, TLB_ENTRY tlb_value);

		/* Buses for incoming instruction fetch/data access to virtual addresses */
		data_access data_bus;
		insn_access insn_bus;

		/* Backdoor bus for TLB access */
		data_TLB_access data_TLB_bus;
		insn_TLB_access insn_TLB_bus;

		/* Backdoor bus for accessing the current thread number */
		thread_num_access thread_num_bus;

		/* Backdoor bus for setting the privilege_level (supervisor bit) */	
		privilege_level_access privilege_level_bus;

		/* Backdoor bus for logging the MMU reads (to deduce the PTEC index) */	
		snoop_MMU_read_access snoop_MMU_read_bus;

		// MMU registers - see the Jupiter manual section 9.1.16
		// for a description of the hardware registers.
		host_int_4 MMU_register[MMU_REGISTERS_NUM];

		/* TLB entries */
		TLB_ENTRY data_TLB[TLB_ENTRIES_NUM];
		TLB_ENTRY insn_TLB[TLB_ENTRIES_NUM];

		/* Bus for accessing physical memory */
		bus *downstream_bus;

		// Are we currently applying the translation map?
		bool enabled_flag;

		/* Current privilege level. (0 = supervisor mode, 1 = user mode - yes, this is intentional) */
		int privilege_level;

		/* The MMU must know which hardware thread is currently executing */
		int current_thread_num;

		/* The last MMU PTEC index accessed (deduced from MMU Read lines in mp.trace) */
		int last_ptec_index;

		/* The current state of the PTEC-tracking state machine */
		MMU_PTEC_STATE mmu_ptec_state;

		/* Methods for PTEC load value checking */
		int get_PTEC_values(int way, int asid, int vpn, int &ptec_asid, int &ptec_vpn, int &vpn_mask, int &ptec_ppn,
				int &ptec_read_bit, int &ptec_write_bit, int &ptec_execute_bit, int &ptec_supervisor_bit);
		void Check_TLB_Value(TLB_ENTRY tlb_value);

		/* Methods for MMU read ring buffer buffering */

		MMU_READ_BUFFER_ENTRY MMU_read_buffer[MMU_READ_BUFFER_ENTRIES];
		int MMU_read_buffer_index;

		void add_entry_to_ring_buffer(int paddr);
		int find_entry_in_ring_buffer(int paddr, int &value);

		/* Main address translation routine */
		int protection_check(TLB_ENTRY *tlb, int index, int read_bit, int write_bit, int execute_bit);
		int select_ASID(host_int_4 vaddr, int &asid);
		int find_TLB_entry(TLB_ENTRY *tlb, int asid, unsigned int upper_vaddr_bits, int read_bit, int write_bit, int execute_bit, host_int_4 &paddr);
		bus::status translate_address(host_int_4 vaddr, host_int_4 &paddr, int read_bit, int write_bit, int execute_bit);
		void snoop_write(host_int_4 vaddr, host_int_4 data);

		void reset(host_int_4 value);
		void enable(host_int_4 value);
		host_int_4 resetStatus;

		callback_pin<jupiterMMU> resetPin, enable_pin;
		friend class callback_pin<jupiterMMU>;

		output_pin halt;

		// State save/restore support.
		string save_state();
		sid::component::status restore_state(const string& state);
		friend ostream& operator<< (ostream& op, const jupiterMMU& obj);
		friend istream& operator>> (istream& ip, jupiterMMU& obj);

		// Triggerpoint support.
		friend class self_watcher<jupiterMMU>;
		self_watcher<jupiterMMU> triggerpoint_manager;
};

jupiterMMU::jupiterMMU()
	: data_bus(this), insn_bus(this), data_TLB_bus(this), insn_TLB_bus(this), privilege_level_bus(this), thread_num_bus(this), snoop_MMU_read_bus(this),
	downstream_bus(0), resetStatus(0), enabled_flag(false), resetPin(this, &jupiterMMU::reset), enable_pin(this, &jupiterMMU::enable), triggerpoint_manager(this)
{
	/* Reset registers values current as of 10/8/2009 - TM */

	memset(this->MMU_register, 0, MMU_REGISTERS_NUM * sizeof(host_int_4));
	MMU_register[CONFIG]    = 0x00000f03;
	MMU_register[PTEC_CFG0] = 0x00050000;

	mmu_ptec_state = EXPECTING_PTEC_ATAG_READ;

	add_accessor("all", &downstream_bus);
	add_attribute_virtual("state-snapshot", this, &jupiterMMU::restore_state, &jupiterMMU::save_state);
	add_bus("data-access-port", dynamic_cast<sid::bus *>(&data_bus));
	add_bus("insn-access-port", dynamic_cast<sid::bus *>(&insn_bus));
	add_bus("data-tlb-access-port",  dynamic_cast<sid::bus *>(&data_TLB_bus));
	add_bus("insn-tlb-access-port",  dynamic_cast<sid::bus *>(&insn_TLB_bus));
	add_bus("thread-num-access-port",       dynamic_cast<sid::bus *>(&thread_num_bus));
	add_bus("privilege-level-access-port", dynamic_cast<sid::bus *>(&privilege_level_bus));
	add_bus("log-mmu-read-port", dynamic_cast<sid::bus *>(&snoop_MMU_read_bus));
	add_pin("reset", &resetPin);
	add_pin("enable", &enable_pin);
	add_pin("halt!", &halt);
}

void jupiterMMU::debug_TLB_entry(int insn_flag, int tlb_index, TLB_ENTRY tlb_value)

{
	int page_shifts, vaddr, paddr;

	if (!debug_mmu_flag)
		return;

	page_shifts = (PAGE_MASK == 3) ? 14 : 13;

	vaddr = TLB_ENTRY_VPN(tlb_value) << page_shifts;
	paddr = TLB_ENTRY_PPN(tlb_value) << page_shifts;

	printf(insn_flag ? "insn" : "data");

	printf(" TLB entry: %d: ASID: 0x%03x, VPN: 0x%05x (vaddr: 0x%08x), PPN: 0x%05x (paddr: 0x%08x), R: %d, W: %d, X: %d, S: %d, V: %d\n",
		tlb_index, TLB_ENTRY_ASID(tlb_value), TLB_ENTRY_VPN(tlb_value), vaddr, TLB_ENTRY_PPN(tlb_value), paddr,
		TLB_ENTRY_READ_BIT(tlb_value), TLB_ENTRY_WRITE_BIT(tlb_value), TLB_ENTRY_EXECUTE_BIT(tlb_value),
		TLB_ENTRY_SUPERVISOR_BIT(tlb_value), TLB_ENTRY_VALID_BIT(tlb_value));
}

/*************************************
* Start translation read/write methods
*************************************/

int jupiterMMU::protection_check(TLB_ENTRY *tlb, int index, int read_bit, int write_bit, int execute_bit)

{
	TLB_ENTRY tlb_value;

	tlb_value = tlb[index];

	if (read_bit && !TLB_ENTRY_READ_BIT(tlb_value)) {
		printf("jupiterMMU: read protection violation\n");
		return 0;
	}

	if (write_bit && !TLB_ENTRY_WRITE_BIT(tlb_value)) {
		printf("jupiterMMU: write protection violation\n");
		return 0;
	}

	if (execute_bit && !TLB_ENTRY_EXECUTE_BIT(tlb_value)) {
		printf("jupiterMMU: execute protection violation\n");
		return 0;
	}

	if (privilege_level && !TLB_ENTRY_SUPERVISOR_BIT(tlb_value)) {
		printf("jupiterMMU: supervisor protection violation: CPU in user mode accessed TLB requiring supervisor mode\n");
		return 0;
	}

	return 1;
}

int jupiterMMU::select_ASID(host_int_4 vaddr, int &asid)

{
	int upper_vaddr_bits, matches;

	matches = 0;

	upper_vaddr_bits = (vaddr >> 28) & 0x0f;

	if (CMP_0_EN(current_thread_num)) {
		if ((upper_vaddr_bits & ASID_MASK0(current_thread_num)) == ASID_CMP0(current_thread_num)) {
			asid = ASID0(current_thread_num);
			matches++;
		}
	}

	if (CMP_1_EN(current_thread_num)) {
		if ((upper_vaddr_bits & ASID_MASK1(current_thread_num)) == ASID_CMP1(current_thread_num)) {
			asid = ASID1(current_thread_num);
			matches++;
		}
	}

	if (CMP_2_EN(current_thread_num)) {
		if ((upper_vaddr_bits & ASID_MASK2(current_thread_num)) == ASID_CMP2(current_thread_num)) {
			asid = ASID2(current_thread_num);
			matches++;
		}
	}

	return matches;
}

/*
** This routine requires an asid, vaddr, and permission bits, and returns the number of matches.
** If the number of matches is 1, then the translated physical address is returned.
*/

int jupiterMMU::find_TLB_entry(TLB_ENTRY *tlb, int asid, unsigned int vaddr, int read_bit, int write_bit, int execute_bit, host_int_4 &paddr)

{
	int i, matches, upper_vaddr_bits;
	TLB_ENTRY tlb_value;

	/* Calculate the upper vaddr bits. */

	upper_vaddr_bits = vaddr >> 13;

	/* The upper_vaddr_bits is always the highest 13 bits, so the LSB must be cleared in 16k page mode. */

	if (PAGE_MASK == 3)
		upper_vaddr_bits &= ~1;

	for (matches=i=0; i<TLB_ENTRIES_NUM; i++) {

		tlb_value = tlb[i];

		if (!TLB_ENTRY_VALID_BIT(tlb_value))
			continue;

		if (debug_mmu_flag)
			printf("checking ");

		debug_TLB_entry(execute_bit, i, tlb_value);

		if (TLB_ENTRY_ASID(tlb_value) != asid) {

			if (debug_mmu_flag)
				printf("...this TLB entry does not match (reason: TLB ASID: 0x%x != ASID: 0x%x)\n", TLB_ENTRY_ASID(tlb_value), asid);

			continue;

		} else if (TLB_ENTRY_VPN(tlb_value) != upper_vaddr_bits) {

			if (debug_mmu_flag)
				printf("...this TLB entry does not match (reason: TLB VPN: 0x%x != VPN: 0x%x)\n", TLB_ENTRY_VPN(tlb_value), upper_vaddr_bits);

			continue;

		} else {

			if (debug_mmu_flag)
				printf("...this TLB entry matches ASID & VPN\n");
		}

		if (!protection_check(tlb, i, read_bit, write_bit, execute_bit)) {
			printf("MMU protection violation\n");
			printf("Should never happen; probably bug in trace file or sidrefcompare\n");
			jupiterMMU_abort();
		}

		switch (PAGE_MASK) {

			case 1:

				/* The TLB_ENTRY_PPN is always 19 bits regardless of the page size.
				   For 8k mode, we need to OR in the lower 13 bits of the virtual address. */

				paddr = (TLB_ENTRY_PPN(tlb_value) << 13) | (vaddr & ((1 << 13) - 1));
				matches++;
				break;

			case 3:

				/* The TLB_ENTRY_PPN is always 19 bits regardless of the page size.
				   For 16k mode, we need to OR in the lower 14 bits of the virtual address. */

				paddr = (TLB_ENTRY_PPN(tlb_value) << 13) | (vaddr & ((1 << 14) - 1));
				matches++;
				break;

			default:

				/* Page size is reserved (Jupiter manual section 9.1.16) */
				assert(0);
		}
	}

	return matches;
}

/*
** This routine is the "core" of the MMU and does the main work of translating an address from a virtual address to a physical address.
** It checks the per-thread ASID registers to find a match, then check TLB entries for an ASID/vpn match, and checks permissions.
** If a single valid translation is found, then the address is translated.
*/

bus::status jupiterMMU::translate_address(host_int_4 vaddr, host_int_4 &paddr, int read_bit, int write_bit, int execute_bit)

{
	int upper_vaddr_bits, asid, i, matches, tlb_index;
	TLB_ENTRY *tlb, tlb_value;

	if (!enabled_flag || !TRANSLATED_ADDRESS(vaddr)) {
		paddr = vaddr;
		return bus::ok;
	}

	/* Determine which ASID matches the virtual address. */

	upper_vaddr_bits = (vaddr >> 28) & 0x0f;

	if (debug_mmu_flag) {
		printf("Start of translation: vaddr: 0x%08x, R: %d, W: %d, X: %d\n", (int)vaddr, read_bit, write_bit, execute_bit);
		printf("upper_vaddr_bits: 0x%x\n", upper_vaddr_bits);
		printf("CMP_0_EN: %d, ASID_MASK0: 0x%x, ASID_CMP0: 0x%x, ASID0: 0x%x\n", CMP_0_EN(current_thread_num), ASID_MASK0(current_thread_num),
			ASID_CMP0(current_thread_num), ASID0(current_thread_num));
		printf("CMP_1_EN: %d, ASID_MASK1: 0x%x, ASID_CMP1: 0x%x, ASID1: 0x%x\n", CMP_1_EN(current_thread_num), ASID_MASK1(current_thread_num),
			ASID_CMP1(current_thread_num), ASID1(current_thread_num));
		printf("CMP_2_EN: %d, ASID_MASK2: 0x%x, ASID_CMP2: 0x%x, ASID2; 0x%x\n", CMP_2_EN(current_thread_num), ASID_MASK2(current_thread_num),
			ASID_CMP2(current_thread_num), ASID2(current_thread_num));
	}

	matches = select_ASID(vaddr, asid);

	/* Ensure that an ASID matched the virtual address */

	if (!matches) {
		printf("No matching ASID found for memory access ASID: 0x%x, vaddr: 0x%08x\n", asid, vaddr);
		jupiterMMU_abort();
	} else if (matches > 1) {
		printf("%d matching ASID found for memory access ASID: 0x%x, vaddr: 0x%08x\n", matches, asid, vaddr);
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("matched ASID: 0x%x\n", asid);

	/* Select either data TLB or instruction TLB depending on access type */

	tlb = execute_bit ? insn_TLB : data_TLB;

	/* Check if there is a matching TLB entry for this access. */

	matches = find_TLB_entry(tlb, asid, vaddr, read_bit, write_bit, execute_bit, paddr);

	if (matches != 1) {
		printf("%d matching TLB entries found for memory access ASID: 0x%x, vaddr: 0x%08x\n", matches, asid, (int)vaddr);
		printf("Should never happen; probably bug in trace file or sidrefcompare\n");
		jupiterMMU_abort();
	} else if (paddr < 0xb0000000) {
		printf("MMU translated vaddr 0x%08x to paddr 0x%08x (virtual address translated to another virtual address)\n", (int)vaddr, (int)paddr);
		printf("Should never happen; probably bug in testcase or sidrefcompare\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("End of translation; vaddr: 0x%08x => paddr: 0x%08x\n", vaddr, paddr);

	return bus::ok;
}

/*
** This routine "snoops" (checks) all the addresses which are written through the MMU.
*/

void jupiterMMU::snoop_write(host_int_4 vaddr, host_int_4 data)
{
	int offset;

	/* We translate all addresses below 0xb0000000. For addresses above 0xb0000000, we only intercept
	   MMU accesses and pass through all other addresses. */

	if ((vaddr >= MMU_REGISTER_START_ADDRESS) && (vaddr < MMU_REGISTER_END_ADDRESS)) {

		offset = (vaddr - MMU_REGISTER_START_ADDRESS) >> 2;

		MMU_register[offset] = data;

		switch (offset) {

			case PTEC_CFG0:
	
				printf("jupiterMMU: ATAG_BASE set to value 0x%x\n", ATAG_BASE);

				if (!IS_OCM_ADDRESS(ATAG_BASE)) 
					printf("jupiterMMU: Warning - ATAG_BASE is set outside of OCM\n");

				break;

			case PTEC_CFG1:
	
				printf("jupiterMMU: ENTRY_BASE set to value 0x%x\n", ENTRY_BASE);

				if (!IS_OCM_ADDRESS(ENTRY_BASE))
					printf("jupiterMMU: Warning - ENTRY_BASE is set outside of OCM\n");

				break;

			case INSERTW1:

				/* If the CPU writes to INSERTW1, it is loading TLB entries manually, so disable TLB value checking. */

				if (check_ptec_flag) {
					printf("jupiterMMU: write to INSERTW1 detected; automatically disabling TLB value checking\n");
					check_ptec_flag = 0;
				}

				break;
		}
	}
}

/*************************************
* End translation read/write methods
*************************************/
/*************************************
* Start read/write memory methods
*************************************/

bus::status jupiterMMU::write(host_int_4 address, little_int_1 data, int insn_flag) throw ()
{
	host_int_4 paddr;
	bus::status retval;

	snoop_write(address, data);

	if (translate_address(address, paddr, 0, 1, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->write(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU wrote 0x%02x to address 0x%08x which is not mapped to a device!\n", (int)data, (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	return retval;
}

bus::status jupiterMMU::write(host_int_4 address, little_int_2 data, int insn_flag) throw ()
{
	host_int_4 paddr;
	bus::status retval;

	snoop_write(address, data);

	if (translate_address(address, paddr, 0, 1, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->write(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU wrote 0x%04x to address 0x%08x which is not mapped to a device!\n", (int)data, (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	return retval;
}

bus::status jupiterMMU::write(host_int_4 address, little_int_4 data, int insn_flag) throw ()
{
	host_int_4 paddr;
	bus::status retval;

	snoop_write(address, data);

	if (translate_address(address, paddr, 0, 1, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->write(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU wrote 0x%08x to address 0x%08x which is not mapped to a device!\n", (int)data, (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	return retval;
}

bus::status jupiterMMU::write(host_int_4 address, little_int_8 data, int insn_flag) throw ()
{
	host_int_4 paddr;
	bus::status retval;

	snoop_write(address, data);

	if (translate_address(address, paddr, 0, 1, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->write(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU wrote 0x%016llx to address %x which is not mapped to a device!\n", (long long int)data, (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	return retval;
}

bus::status jupiterMMU::write(host_int_4 address, big_int_1 data, int insn_flag) throw ()
{
	host_int_4 paddr;
	bus::status retval;

	snoop_write(address, data);

	if (translate_address(address, paddr, 0, 1, insn_flag) == bus::unmapped)
		return bus::unmapped;

	if (debug_mmu_flag)
		printf("jupiterMMU: write address: 0x%08x = 0x%02x\n", paddr, (int)data);

	retval = downstream_bus->write(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU wrote 0x%02x to address 0x%08x which is not mapped to a device!\n", (int)data, (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	return retval;
}

bus::status jupiterMMU::write(host_int_4 address, big_int_2 data, int insn_flag) throw ()
{
	host_int_4 paddr;
	bus::status retval;

	snoop_write(address, data);

	if (translate_address(address, paddr, 0, 1, insn_flag) == bus::unmapped)
		return bus::unmapped;

	if (debug_mmu_flag)
		printf("jupiterMMU: write address: 0x%08x = 0x%04x\n", paddr, (int)data);

	retval = downstream_bus->write(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU wrote 0x%04x to address 0x%08x which is not mapped to a device!\n", (int)data, (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	return retval;
}

bus::status jupiterMMU::write(host_int_4 address, big_int_4 data, int insn_flag) throw ()
{
	host_int_4 paddr;
	bus::status retval;

	snoop_write(address, data);

	if (translate_address(address, paddr, 0, 1, insn_flag) == bus::unmapped)
		return bus::unmapped;

	if (debug_mmu_flag)
		printf("jupiterMMU: write address: 0x%08x = 0x%08x\n", paddr, (int)data);

	retval = downstream_bus->write(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU wrote 0x%08x to address 0x%08x which is not mapped to a device!\n", (int)data, (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	return retval;
}

bus::status jupiterMMU::write(host_int_4 address, big_int_8 data, int insn_flag) throw ()
{
	host_int_4 paddr;
	bus::status retval;

	snoop_write(address, data);

	if (translate_address(address, paddr, 0, 1, insn_flag) == bus::unmapped)
		return bus::unmapped;

	if (debug_mmu_flag)
		printf("jupiterMMU: write address: 0x%08x = 0x%08llx\n", paddr, (long long int)data);

	retval = downstream_bus->write(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU wrote 0x%016x to address 0x%08x which is not mapped to a device!\n", (long long int)data, (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	return retval;
}

bus::status jupiterMMU::read(host_int_4 address, little_int_1 &data, int insn_flag) throw()
{
	host_int_4 paddr;
	bus::status retval;

	if (translate_address(address, paddr, 1, 0, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->read(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU read from address 0x%08x which is not mapped to a device!\n", (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("jupiterMMU: read address 0x%08x = 0x%02x\n", paddr, (int)data);

	return retval;
}

bus::status jupiterMMU::read(host_int_4 address, little_int_2 &data, int insn_flag) throw()
{
	host_int_4 paddr;
	bus::status retval;

	if (translate_address(address, paddr, 1, 0, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->read(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU read from address 0x%08x which is not mapped to a device!\n", (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("jupiterMMU: read address 0x%08x = 0x%04x\n", paddr, (int)data);

	return retval;
}

bus::status jupiterMMU::read(host_int_4 address, little_int_4 &data, int insn_flag) throw()
{
	host_int_4 paddr;
	bus::status retval;

	if (translate_address(address, paddr, 1, 0, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->read(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU read from address %x which is not mapped to a device!\n", (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("jupiterMMU: read address 0x%08x = 0x%08x\n", paddr, (int)data);

	return retval;
}

bus::status jupiterMMU::read(host_int_4 address, little_int_8 &data, int insn_flag) throw()
{
	host_int_4 paddr;
	bus::status retval;

	if (translate_address(address, paddr, 1, 0, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->read(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU read from address 0x%08x which is not mapped to a device!\n", (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("jupiterMMU: read address 0x%08x = 0x%016x\n", paddr, (long long int)data);

	return retval;
}

bus::status jupiterMMU::read(host_int_4 address, big_int_1 &data, int insn_flag) throw()
{
	host_int_4 paddr;
	bus::status retval;

	if (translate_address(address, paddr, 1, 0, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->read(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU read from address 0x%08x which is not mapped to a device!\n", (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("jupiterMMU: read address 0x%08x = 0x%02x\n", paddr, (int)data);

	return retval;
}

bus::status jupiterMMU::read(host_int_4 address, big_int_2 &data, int insn_flag) throw()
{
	host_int_4 paddr;
	bus::status retval;

	if (translate_address(address, paddr, 1, 0, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->read(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU read from address 0x%08x which is not mapped to a device!\n", (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("jupiterMMU: read address 0x%08x = 0x%04x\n", paddr, (int)data);

	return retval;
}

bus::status jupiterMMU::read(host_int_4 address, big_int_4 &data, int insn_flag) throw()
{
	host_int_4 paddr;
	bus::status retval;

	if (translate_address(address, paddr, 1, 0, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->read(paddr, data);

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU read from address 0x%08x which is not mapped to a device!\n", (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("jupiterMMU: read address 0x%08x = 0x%08x\n", paddr, (int)data);

	return retval;
}

bus::status jupiterMMU::read(host_int_4 address, big_int_8 &data, int insn_flag) throw()
{
	host_int_4 paddr;
	bus::status retval;

	if (translate_address(address, paddr, 1, 0, insn_flag) == bus::unmapped)
		return bus::unmapped;

	retval = downstream_bus->read(paddr, data); 

	if (retval == bus::unmapped && running_flag) {
		printf("jupiterMMU: CPU read from address 0x%08x which is not mapped to a device!\n", (int)address);
		printf("Possible bug in either test code or the SID configuration file.\n");
		jupiterMMU_abort();
	}

	if (debug_mmu_flag)
		printf("jupiterMMU: read address 0x%08x = 0x%016x\n", paddr, (long long int)data);

	return retval;
}

/*************************************
* End read/write memory methods
*************************************/
/*************************************
* Start MMU read buffering code
*************************************/

/*
** These routines buffer the data when the "MMU read" occurs. This is required because the jupiterMMU code checks the TLB value
** on a TLB update, and the PTEC value could have been modified between the MMU Read and the TLB Update lines.
*/

void jupiterMMU::add_entry_to_ring_buffer(int paddr)
{
	big_int_4 data;
	bus::status retval;

	retval = downstream_bus->read(paddr, data);

	if (retval != bus::ok) {
		printf("Error: couldn't read paddr 0x%08x in jupiterMMU::add_entry_to_ring_buffer()\n");
		jupiterMMU_abort();
	}

	MMU_read_buffer[MMU_read_buffer_index].paddr = paddr;
	MMU_read_buffer[MMU_read_buffer_index].value = (int)data;

	if (debug_mmu_flag)
		printf("    MMU read (paddr: 0x%08x, value: 0x%08x) stored at index %d in ring buffer\n", paddr, (int)data, MMU_read_buffer_index);

	MMU_read_buffer_index = (++MMU_read_buffer_index) & (MMU_READ_BUFFER_ENTRIES - 1);
}

/*
** Given a paddr, this routine searches through the ring buffer of previously processed "MMU Read" lines, and returns the value which
** was previously read by the MMU from this paddr.
*/

int jupiterMMU::find_entry_in_ring_buffer(int paddr, int &value)
{
	int i, temp_index;
	bus::status retval;
	big_int_4 data;

	temp_index = (MMU_read_buffer_index - 1) & (MMU_READ_BUFFER_ENTRIES - 1);

	for (i=0; i<MMU_READ_BUFFER_ENTRIES; i++) {

		if (MMU_read_buffer[temp_index].paddr == paddr) {

			value = MMU_read_buffer[temp_index].value;

			retval = downstream_bus->read(paddr, data);

			if (retval != bus::ok) {
				printf("Error: couldn't read paddr 0x%08x in jupiterMMU::find_entry_in_ring_buffer()\n");
				jupiterMMU_abort();
			}

			if (value != data) {
				printf("    Note: PTEC data (atag or entry) was modified between \"MMU Read\" and \"TLB Update\" lines\n");
				printf("          paddr: 0x%08x: value at MMU read: 0x%08x, value at TLB Update: 0x%08x\n", paddr, value, (int)data);
			}

			if (debug_mmu_flag)
				printf("        Previous MMU Read (0x%x = 0x%x) found in ring buffer at index %d\n", paddr, value, temp_index);

			return 1;
		}

		temp_index = --temp_index & (MMU_READ_BUFFER_ENTRIES - 1);
	}

	if (debug_mmu_flag)
		printf("        Note: PTEC address (0x%08x) was never read by hardware MMU (does not match previous %d MMU Reads in trace file)\n",
			paddr, MMU_READ_BUFFER_ENTRIES);

	return 0;
}

/*************************************
* End MMU read buffering code
*************************************/
/*************************************
* Start check PTEC check code
*************************************/

/*
** The PTEC checking code herein performs two primary functions:
**
** o Performs a sanity check on the order of MMU read accesses.
**   It ensures the MMU reads the atag, way0, and way1 arrays in the correct order.
**
** o It keeps track of the current PTEC index accesse by the MMU for later use by other routines.
**/

/*
** Print the current state of the MMU state machine.
*/

char *print_MMU_PTEC_state(MMU_PTEC_STATE mmu_ptec_state)

{
	switch (mmu_ptec_state) {

		case EXPECTING_PTEC_ATAG_READ:

			return "PTEC atag read";
			break;

		case EXPECTING_PTEC_ATAG_OR_WAY0_OR_WAY1_READ:

			return "PTEC atag or entry way0 or entry way 1 read";
			break;

		case EXPECTING_PTEC_WAY1_READ:

			return "PTEC way 1 read";
			break;

		case EXPECTING_PTEC_WAY1_OR_ATAG_READ:

			return "PTEC way 1 or atag read";
			break;
	}
}

/*
** When an "MMU Read" line is processed in sidrefcompare, this function is called. It decodes the access
** (determines whether it's an atag, way 0, or way 1 access) and advances the state machine which tracks the MMU state.
**/

bus::status jupiterMMU::snoop_MMU_read(int address) throw()
{
	int ptec_index, ptec_size;

	add_entry_to_ring_buffer(address);

	ptec_size = 1 << (PTEC_ENTRIES_LOG2 + 2);

	/* Check if the MMU read is an atag read */

	if ((address >= ATAG_BASE) && (address < (ATAG_BASE + ptec_size))) {

		ptec_index  = (address - ENTRY_BASE) >> 2;
		ptec_index &= PTEC_ENTRIES_MASK;
		last_ptec_index = ptec_index;

		if (debug_mmu_flag)
			printf("    Note: MMU read is PTEC atag at index: 0x%x\n", ptec_index);

		if ((mmu_ptec_state != EXPECTING_PTEC_ATAG_READ) && (mmu_ptec_state != EXPECTING_PTEC_ATAG_OR_WAY0_OR_WAY1_READ) &&
		    (mmu_ptec_state != EXPECTING_PTEC_WAY1_OR_ATAG_READ)) {
			printf("jupiterMMU: Unexpected PTEC atag read; was expecting a %s\n", print_MMU_PTEC_state(mmu_ptec_state));
			printf("Possible bug in either sidrefcompare or trace file, or test may require PTEC checking disabled (-p option)\n");
			jupiterMMU_abort();
		}

		mmu_ptec_state = EXPECTING_PTEC_ATAG_OR_WAY0_OR_WAY1_READ;
	}

	/* Check if the MMU read is an entry way 0 read */

	if ((address >= ENTRY_BASE) && (address < (ENTRY_BASE + ptec_size))) {
		ptec_index  = (address - ENTRY_BASE) >> 2;
		ptec_index &= PTEC_ENTRIES_MASK;

		if (debug_mmu_flag)
			printf("    Note: MMU read is PTEC entry way 0 at index: 0x%x\n", ptec_index);
		
		if (mmu_ptec_state != EXPECTING_PTEC_ATAG_OR_WAY0_OR_WAY1_READ) {
			printf("jupiterMMU: Unexpected PTEC way 0 read; was expecting a %s\n", print_MMU_PTEC_state(mmu_ptec_state));
			printf("Possible bug in either sidrefcompare or trace file, or test may require PTEC checking disabled (-p option)\n");
			jupiterMMU_abort();
		}

		if (ptec_index != last_ptec_index) {
			printf("jupiterMMU: PTEC way 0 read index (0x%x) does not match previous PTEC atag read index (0x%x)\n", ptec_index, last_ptec_index);
			jupiterMMU_abort();
		}

		mmu_ptec_state = EXPECTING_PTEC_WAY1_OR_ATAG_READ;
	}

	/* Check if the MMU read is an entry way 1 read */

	if ((address >= (ENTRY_BASE + ptec_size)) && (address < (ENTRY_BASE + (ptec_size * 2)))) {
		ptec_index  = (address - ENTRY_BASE) >> 2;
		ptec_index &= PTEC_ENTRIES_MASK;

		if (debug_mmu_flag)
			printf("    Note: MMU read is PTEC entry way 1 at index: 0x%x\n", ptec_index);

		if ((mmu_ptec_state != EXPECTING_PTEC_ATAG_OR_WAY0_OR_WAY1_READ) && (mmu_ptec_state != EXPECTING_PTEC_WAY1_OR_ATAG_READ)) {
			printf("jupiterMMU: Unexpected PTEC way 1 read; was expecting a %s\n", print_MMU_PTEC_state(mmu_ptec_state));
			printf("Possible bug in either sidrefcompare or trace file, or test may require PTEC checking disabled (-p option)\n");
			jupiterMMU_abort();
		}

		if (ptec_index != last_ptec_index) {
			printf("jupiterMMU: PTEC way 1 read index (0x%x) does not match previous PTEC atag read index (0x%x)\n", ptec_index, last_ptec_index);
			jupiterMMU_abort();
		}

		mmu_ptec_state = EXPECTING_PTEC_ATAG_READ;
	}
}

/*
** Given a way, ASID, and VPN, this routine reads the PTEC and returns the attributes of the PTEC entry.
*/

int jupiterMMU::get_PTEC_values(int way, int asid, int vpn, int &ptec_asid, int &ptec_vpn, int &vpn_mask, int &ptec_ppn,
				 int &ptec_read_bit, int &ptec_write_bit, int &ptec_execute_bit, int &ptec_supervisor_bit)

{
	int vpn_bits, asid_shifts, page_shift, ptec_index, atag_paddr, entry_paddr, ptec_atag, ptec_entry, calc_vpn;
	int vpn_atag_bits, vpn_entry_bits, vpn_index_bits, extra_page_shifts;
	big_int_4 value;

	if ((way < 0) || (way > 2)) {
		printf("internal error: get_PTEC_entry passed illegal way %d\n", way);
		abort();
	}

	if (debug_mmu_flag)
		printf("Checking way %d...\n", way);

	/*
	** This section of code calculates the PTEC index from the ASID and VPN and checks it against the previous MMU accesses.
	** Sometimes this check fails because the TLB Update occurs late, and the MMU Read for the next TLB Update has already occured.
	** Therfore, we only print a note when this check fails instead of aborting.
	*/

	/* For 16k page mode, the VPN needs to be shifted right one because the LSB is always zero. */

	asid_shifts = (PTEC_ENTRIES_LOG2 > 10) ? (PTEC_ENTRIES_LOG2 - 10) : 0;
	page_shift = (PAGE_MASK == 3);

	ptec_index = ((asid << asid_shifts) ^ (vpn >> page_shift)) & PTEC_ENTRIES_MASK;

	if (debug_mmu_flag)
		printf("    ptec_index:0x%x = ((asid:0x%x << asid_shifts:%d) ^ (vpn:0x%x >> page_shift:%d)) & ((1 << PTEC_ENTRIES_LOG2:%d) - 1)\n",
			ptec_index, asid, asid_shifts, vpn, page_shift, PTEC_ENTRIES_LOG2);

	if (ptec_index == last_ptec_index) {
		if (debug_mmu_flag)
			printf("    Calculated PTEC index (0x%x) matches last empirically observed PTEC index (0x%x)\n", ptec_index, last_ptec_index);
	} else {
		printf("    Note: Calculated PTEC index (0x%x) does not match last empirically observed PTEC index (0x%x)\n", ptec_index, last_ptec_index);
		printf("          This may be caused by a delayed TLB Update and is probably harmless\n");
	}

	/* 
	** This section of code reads the PTEC atag and entry bits for the specified way.
	*/

	atag_paddr = ATAG_BASE + (ptec_index << 2);

	if (debug_mmu_flag)
		printf("    Reading PTEC ATAG from paddr 0x%08x (ATAG_BASE:0x%x, ptec_index:0x%x)\n", atag_paddr, ATAG_BASE, ptec_index);

	if (!find_entry_in_ring_buffer(atag_paddr, ptec_atag))
		return 0;

	entry_paddr = ENTRY_BASE + (ptec_index << 2);

	if (way) {
		ptec_atag = EXTRACT_BITS_FROM_INT32(ptec_atag, 0, 16);
		entry_paddr += 1 << (PTEC_ENTRIES_LOG2 + 2);
	} else
		ptec_atag = EXTRACT_BITS_FROM_INT32(ptec_atag, 16, 16);

	if (debug_mmu_flag)
		printf("    Reading PTEC way %d from paddr %08x (ENTRY_BASE:0x%x, ptec_index:0x%x)\n", way, entry_paddr, ENTRY_BASE, ptec_index);

	if (!find_entry_in_ring_buffer(entry_paddr, ptec_entry))
		return 0;

	/*
	** This section of code actually extracts all the attributes from the previously-read PTEC values.
	*/

	ptec_asid = EXTRACT_BITS_FROM_INT32(ptec_atag, 0, 10);

	/* Not all the VPN bits are compared due to the RTAG_MASK, so build a compare mask for convenience.
	   Compare all the index and tag bits, but only compare the rtag bits using the RTAG_MASK. */

	vpn_mask = (RTAG_MASK << (PTEC_ENTRIES_LOG2 + 6)) | ((1 << (PTEC_ENTRIES_LOG2 + 6)) - 1);

	/* VPN from the PTEC: | RTAG_MASK bits from rtag | 6 bits from the ptec atag | PTEC_ENTRIES_LOG2 bits | */
	
	/* In 16k page mode, the VPN is shifted right one bit before building the tag and rtag (because the LSB is always zero).
	   Therefore, when we rebuild the VPN from the tag and rtag in 16k page mode, the VPN needs to be shifted left one bit. */

	extra_page_shifts = (PAGE_MASK == 3);

#ifdef USE_NONCACHEABLE_BIT
	vpn_entry_bits = ((ptec_entry >> 6) & RTAG_MASK) << (PTEC_ENTRIES_LOG2 + 6 + extra_page_shifts);	/* high bits from the ptec_entry */
#else
	vpn_entry_bits = ((ptec_entry >> 5) & RTAG_MASK) << (PTEC_ENTRIES_LOG2 + 6 + extra_page_shifts);	/* high bits from the ptec_entry */
#endif
	vpn_atag_bits  = ((ptec_atag >> 10) & ((1 << 6) - 1)) << (PTEC_ENTRIES_LOG2 + extra_page_shifts);	/* low bits from the ptec_atag */
	vpn_index_bits = ((ptec_index ^ (ptec_asid << asid_shifts)) & PTEC_ENTRIES_MASK) << page_shift;

	/* Build the final VPN from the partial bits from the entry, atag, and PTEC index bits */

	ptec_vpn = vpn_entry_bits | vpn_atag_bits | vpn_index_bits;

	if (debug_mmu_flag) {

		printf("    vpn_mask:0x%x = (RTAG_MASK:0x%x << (PTEC_ENTRIES_LOG2:%d + 6)) | ((1 << (PTEC_ENTRIES_LOG2:%d + 6)) - 1)\n",
			vpn_mask, RTAG_MASK, PTEC_ENTRIES_LOG2, PTEC_ENTRIES_LOG2);

		printf("    ptec_vpn:0x%x = vpn_entry_bits:0x%x | vpn_atag_bits:0x%x | vpn_index_bits:0x%x, calculated as:\n",
			ptec_vpn, vpn_entry_bits, vpn_atag_bits, vpn_index_bits);

		printf("        extra_page_shifts:%d = (PAGE_MASK:%d == 3)\n", extra_page_shifts, PAGE_MASK);

#ifdef USE_NONCACHEABLE_BIT
		printf("        vpn_entry_bits:0x%x = ((ptec_entry:0x%x >> 6) & RTAG_MASK:0x%x) << (PTEC_ENTRIES_LOG2:%d + 6 + extra_page_shifts:%d)\n",
			vpn_entry_bits, ptec_entry, RTAG_MASK, PTEC_ENTRIES_LOG2, extra_page_shifts);
#else
		printf("        vpn_entry_bits:0x%x = ((ptec_entry:0x%x >> 5) & RTAG_MASK:0x%x) << (PTEC_ENTRIES_LOG2:%d + 6 + extra_page_shifts:%d)\n",
			vpn_entry_bits, ptec_entry, RTAG_MASK, PTEC_ENTRIES_LOG2, extra_page_shifts);
#endif

		printf("        vpn_atag_bits:0x%x = ((ptec_atag:0x%x >> 10) & ((1 << 6) - 1)) << (PTEC_ENTRIES_LOG2:0x%x + extra_page_shifts:%d)\n",
			vpn_atag_bits, ptec_atag, PTEC_ENTRIES_LOG2, extra_page_shifts);

		printf("        vpn_index_bits:0x%x = ((ptec_index:0x%x ^ (ptec_asid:0x%x << asid_shifts:%d)) & PTEC_ENTRIES_MASK:0x%x) << page_shift:%d\n",
			vpn_index_bits, ptec_index, ptec_asid, asid_shifts, PTEC_ENTRIES_MASK, page_shift);
	}

	/* Extract all the other attributes from the PTEC. */

	ptec_ppn = EXTRACT_BITS_FROM_INT32(ptec_entry, 12, 20) >> page_shift;

	ptec_read_bit       = EXTRACT_BITS_FROM_INT32(ptec_entry, 4, 1);
	ptec_write_bit      = EXTRACT_BITS_FROM_INT32(ptec_entry, 3, 1);
	ptec_execute_bit    = EXTRACT_BITS_FROM_INT32(ptec_entry, 2, 1);
	ptec_supervisor_bit = EXTRACT_BITS_FROM_INT32(ptec_entry, 1, 1);

	return 1;
}

/*
** When a value is loaded into a TLB entry, this routine is called to check the value against the PTEC.
*/

void jupiterMMU::Check_TLB_Value(TLB_ENTRY tlb_value)

{
	int tlb_asid, tlb_vpn, tlb_ppn, tlb_read_bit, tlb_write_bit, tlb_execute_bit, tlb_supervisor_bit;
	int ptec_asid, ptec_vpn, vpn_mask, ptec_ppn, ptec_read_bit, ptec_write_bit, ptec_execute_bit, ptec_supervisor_bit;
	int match_way0, match_way1, read_flag;

	tlb_asid           = TLB_ENTRY_ASID(tlb_value);
	tlb_vpn            = TLB_ENTRY_VPN(tlb_value);
	tlb_ppn            = TLB_ENTRY_PPN(tlb_value);
	tlb_read_bit       = TLB_ENTRY_READ_BIT(tlb_value);
	tlb_write_bit      = TLB_ENTRY_WRITE_BIT(tlb_value);
	tlb_execute_bit    = TLB_ENTRY_EXECUTE_BIT(tlb_value);
	tlb_supervisor_bit = TLB_ENTRY_SUPERVISOR_BIT(tlb_value);

	match_way0 = match_way1 = 1;

	read_flag = get_PTEC_values(0, tlb_asid, tlb_vpn, ptec_asid, ptec_vpn, vpn_mask, ptec_ppn, ptec_read_bit, ptec_write_bit,
				    ptec_execute_bit, ptec_supervisor_bit);

	if (read_flag) {

		if ((ptec_asid != tlb_asid) || ((ptec_vpn & vpn_mask) != (tlb_vpn & vpn_mask)) || (ptec_ppn != tlb_ppn)) {

			match_way0 = 0;

			if (debug_mmu_flag)
				printf("    TLB does not match PTEC way 0 translation: RTAG_MASK: 0x%x, ASID/VPN/PPN: TLB: 0x%x/0x%x/0x%x, way 0: 0x%x/0x%x/0x%x\n",
					RTAG_MASK, tlb_asid, tlb_vpn, tlb_ppn, ptec_asid, ptec_vpn, ptec_ppn);
		}

		if ((ptec_read_bit != tlb_read_bit) || (ptec_write_bit != tlb_write_bit) || (ptec_execute_bit != tlb_execute_bit) ||
		    (ptec_supervisor_bit != tlb_supervisor_bit)) {

			match_way0 = 0;

			if (debug_mmu_flag)
				printf("    TLB does not match PTEC way 0 permissions: TLB: %d%d%d%d, PTEC way 0: %d%d%d%d\n",
					tlb_read_bit, tlb_write_bit, tlb_execute_bit, tlb_supervisor_bit,
					ptec_read_bit, ptec_write_bit, ptec_execute_bit, ptec_supervisor_bit);
		}

	} else {
		match_way0 = 0;

		if (debug_mmu_flag)
			printf("    Way 0 check aborted; hardware MMU never read PTEC way 0\n");
	}

	if (debug_mmu_flag && match_way0)
		printf("    TLB load matched PTEC way 0\n");

	read_flag = get_PTEC_values(1, tlb_asid, tlb_vpn, ptec_asid, ptec_vpn, vpn_mask, ptec_ppn, ptec_read_bit, ptec_write_bit,
				    ptec_execute_bit, ptec_supervisor_bit);

	if (read_flag) {

		if ((ptec_asid != tlb_asid) || ((ptec_vpn & vpn_mask) != (tlb_vpn & vpn_mask)) || (ptec_ppn != tlb_ppn)) {

			match_way1 = 0;

			if (debug_mmu_flag)
				printf("    TLB does not match PTEC way 1 translation: RTAG_MASK: 0x%x, ASID/VPN/PPN: TLB: 0x%x/0x%x/0x%x, way 1: 0x%x/0x%x/0x%x\n",
					RTAG_MASK, tlb_asid, tlb_vpn, tlb_ppn, ptec_asid, ptec_vpn, ptec_ppn);
		}

		if ((ptec_read_bit != tlb_read_bit) || (ptec_write_bit != tlb_write_bit) || (ptec_execute_bit != tlb_execute_bit) ||
		    (ptec_supervisor_bit != tlb_supervisor_bit)) {

			match_way1 = 0;

			if (debug_mmu_flag)
				printf("    TLB does not match PTEC way 1 permissions: TLB: %d%d%d%d, PTEC way 0: %d%d%d%d\n",
					tlb_read_bit, tlb_write_bit, tlb_execute_bit, tlb_supervisor_bit,
					ptec_read_bit, ptec_write_bit, ptec_execute_bit, ptec_supervisor_bit);
		}

	} else {
		match_way1 = 0;

		if (debug_mmu_flag)
			printf("    Aborted way 1 check; hardware MMU never read PTEC way 1\n");
	}


	if (match_way0 && match_way1) {

		printf("TLB load matched both way 0 and way 1\n");
		printf("Possible bug in either test code, sidrefcompare, trace file, or test may require TLB value checking disabled (-t option)\n");
		jupiterMMU_abort();

	} else if (!match_way0 && !match_way1) {

		printf("TLB load did not match either PTEC way 0 or way 1\n");
		printf("Possible bug in either sidrefcompare or trace file, or test may require TLB value checking disabled (-t option)\n");
		jupiterMMU_abort();

	} else if (debug_mmu_flag)

		printf("    TLB load matched PTEC way %d\n", match_way1);
}

/*************************************
* End check PTEC check code
*************************************/
/*************************************
* Start read/write TLB methods
*************************************/

bus::status jupiterMMU::TLB_write(host_int_4 address, little_int_1 data, int insn_flag) throw ()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_write(host_int_4 address, little_int_2 data, int insn_flag) throw ()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_write(host_int_4 address, little_int_4 data, int insn_flag) throw ()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_write(host_int_4 address, little_int_8 data, int insn_flag) throw ()
{
	int i, index, ptec_entries_num, asid;
	TLB_ENTRY *tlb;

	if (address >= TLB_ENTRIES_NUM) {
		printf(insn_flag ? "Tried to write to invalid insn TLB entry %d" : "Tried to write to invalid data TLB entry %d", address);
		jupiterMMU_abort();
	}

	/* If the page size is 16k, then we clear the LSB of the VPN (bit 35).
	   The hardware implementation does this for speed to avoid an extra gate when comparing VPNs. */

	if (PAGE_MASK == 3)
		data = data & ~(1LL << 35);		// Stupid host_int_64 does not support &=

	tlb = insn_flag ? insn_TLB : data_TLB;

	if (debug_mmu_flag) {
		printf("    old ");
		debug_TLB_entry(insn_flag, address, tlb[address]);
	}

	tlb[address] = data;

	if (debug_mmu_flag) {
		printf("    new ");
		debug_TLB_entry(insn_flag, address, tlb[address]);
	}

	/* If the value is zero, then it's a TLB invalidate, so don't check it. */

	if (PTEC_ENABLED && check_ptec_flag && data)
		Check_TLB_Value(data);
 
	return bus::ok;
}

bus::status jupiterMMU::TLB_write(host_int_4 address, big_int_1 data, int insn_flag) throw ()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_write(host_int_4 address, big_int_2 data, int insn_flag) throw ()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_write(host_int_4 address, big_int_4 data, int insn_flag) throw ()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_write(host_int_4 address, big_int_8 data, int insn_flag) throw ()
{
	int tree_index, tlb_index;
	long long int temp;
	TLB_ENTRY *tlb;

	if (address >= TLB_ENTRIES_NUM) {
		printf(insn_flag ? "Tried to write to invalid insn TLB entry %d" : "Tried to write to invalid data TLB entry %d", address);
		jupiterMMU_abort();
	}

	/* If the page size is 16k, then we clear the LSB of the VPN (bit 35).
	   The hardware implementation does this for speed to avoid an extra gate when comparing VPNs. */

	if (PAGE_MASK == 3)
		data = data & ~(1LL << 35);		// Stupid host_int_64 does not support &=

	tlb = insn_flag ? insn_TLB : data_TLB;

	if (debug_mmu_flag) {
		printf("    old ");
		debug_TLB_entry(insn_flag, address, tlb[address]);
	}

	tlb[address] = data;

	if (debug_mmu_flag) {
		printf("    new ");
		debug_TLB_entry(insn_flag, address, tlb[address]);
	}

	/* If the value is zero, then it's a TLB invalidate, so don't check it. */

	if (PTEC_ENABLED && check_ptec_flag && data)
		Check_TLB_Value(data);

	return bus::ok;
}

bus::status jupiterMMU::TLB_read(host_int_4 address, little_int_1 &data, int insn_flag) throw()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_read(host_int_4 address, little_int_2 &data, int insn_flag) throw()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_read(host_int_4 address, little_int_4 &data, int insn_flag) throw()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_read(host_int_4 address, little_int_8 &data, int insn_flag) throw()
{
	TLB_ENTRY *tlb;

	if (address >= TLB_ENTRIES_NUM) {
		printf(insn_flag ? "Tried to read from invalid insn TLB entry %d" : "Tried to read from invalid data TLB entry %d", address);
		jupiterMMU_abort();
	}

	tlb = insn_flag ? insn_TLB : data_TLB;

	data = tlb[address];

	debug_TLB_entry(insn_flag, address, data);

	/* If the value is zero, then it's a TLB invalidate, so don't check it. */

	if (PTEC_ENABLED && check_ptec_flag && data)
		Check_TLB_Value(data);

	return bus::ok;
}

bus::status jupiterMMU::TLB_read(host_int_4 address, big_int_1 &data, int insn_flag) throw()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_read(host_int_4 address, big_int_2 &data, int insn_flag) throw()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_read(host_int_4 address, big_int_4 &data, int insn_flag) throw()
{
	return bus::unmapped;
}

bus::status jupiterMMU::TLB_read(host_int_4 address, big_int_8 &data, int insn_flag) throw()
{
	TLB_ENTRY *tlb;

	if (address >= TLB_ENTRIES_NUM) {
		printf(insn_flag ? "Tried to read from invalid insn TLB entry %d" : "Tried to read from invalid data TLB entry %d", address);
		jupiterMMU_abort();
	}

	tlb = insn_flag ? insn_TLB : data_TLB;

	data = tlb[address];

	debug_TLB_entry(insn_flag, address, data);

	/* If the value is zero, then it's a TLB invalidate, so don't check it. */

	if (PTEC_ENABLED && check_ptec_flag && data)
		Check_TLB_Value(data);

	return bus::ok;
}

/*************************************
* End read/write TLB methods
*************************************/

string jupiterMMU::save_state()
{
	return make_attribute(*this);
}

sid::component::status jupiterMMU::restore_state(const string& state)
{
	return parse_attribute(state, *this);
}

ostream& operator<< (ostream& op, const jupiterMMU& obj)
{
	return op;
}
istream& operator>> (istream& ip, jupiterMMU& obj)
{
	return ip;
}

void jupiterMMU::reset(host_int_4 value)
{
	if (value > 0)
		resetStatus |= 1;
}

void jupiterMMU::enable(host_int_4 value)
{
	printf("MMU enable flag set to %d\n", (int)value);

	enabled_flag = !!value;
}

static vector<string> compMmuListTypes()
{
	vector<string> types;

	types.push_back("hw-mmu-ubicom32/jupiterMMU");

	return types;
}

static component* compMmuCreate(const string& typeName)
{
	if (typeName == "hw-mmu-ubicom32/jupiterMMU")
		return new jupiterMMU();

	return 0;
}

static void compMmuDelete(component* c)
{
	delete dynamic_cast<jupiterMMU*>(c);
}

DLLEXPORT extern const component_library jupiterMMU_component_library;

const component_library jupiterMMU_component_library =
{
	COMPONENT_LIBRARY_MAGIC,
	&compMmuListTypes,
	&compMmuCreate,
	&compMmuDelete
};
