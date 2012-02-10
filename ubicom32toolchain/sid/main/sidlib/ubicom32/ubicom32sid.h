// ubicom32sid.h - Define the external interface of the ubicom32 SID encapsulation
// classes.

// Copyright (C) 2001, 2002 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.
  
#ifndef IP3KSID_H
#define IP3KSID_H 
  
#include <sidlib.h>
namespace sidlib
{
  class ubicom32cpu: public simwcpu
  {
  public:
    
    // gdb register numbers for ubicom32 main 
    enum regno_t
    {
      d0                  = 0,
      d1                  = 1,
      d2                  = 2,
      d3                  = 3,
      d4                  = 4,
      d5                  = 5,
      d6                  = 6,
      d7                  = 7,
      d8                  = 8,
      d9                  = 9,
      d10                 = 10,
      d11                 = 11,
      d12                 = 12,
      d13                 = 13,
      d14                 = 14,
      d15                 = 15,

      a0                  = 16,
      a1                  = 17,
      a2                  = 18,
      a3                  = 19,
      a4                  = 20,
      a5                  = 21,
      a6                  = 22,
      sp                  = 23,
      mac_hi              = 24,
      mac_lo              = 25,
      mac_rc16            = 26,
      source3             = 27,
      inst_cnt            = 28,
      csr                 = 29,
      rosr                = 30,
      iread_data          = 31,
      int_mask0           = 32,
      int_mask1           = 33,
      int_mask2		  = 34,
      pc                  = 35,


      trap_cause          = 36,
      acc1_hi		  = 37,
      acc1_lo		  = 38,
      previous_pc	  = 39,
      ucsr                = 40,
      chip_id             = 41,
      int_stat0           = 42,
      int_stat1           = 43,
      int_stat2           = 44,
      int_set0            = 45,
      int_set1            = 46,
      int_set2            = 47,
      int_clr0            = 48,
      int_clr1            = 49,
      int_clr2            = 50,
      global_ctrl         = 51,
      mt_active           = 52,
      mt_active_set       = 53,
      mt_active_clr       = 54,
      mt_dbg_active       = 55,
      mt_dbg_active_set   = 56,
      mt_en               = 57,
      mt_hpri             = 58,
      mt_hrt              = 59,
      mt_break_clr        = 60,
      mt_break            = 51,
      mt_single_step      = 62,
      mt_min_delay_en     = 63,
      mt_break_set        = 64,
      dcapt               = 65,
      mt_dbg_active_clr   = 66,
      scratchpad0	  = 67,
      scratchpad1	  = 68,
      scratchpad2	  = 69,
      scratchpad3	  = 70,
      scratchpad4         = 71,
      scratchpad5         = 72,
      scratchpad6         = 73,

      /* 0x19c - 0x19f Reserved */
      chip_cfg            = 74,
      mt_i_blocked        = 75,
      mt_d_blocked        = 76,
      mt_i_blocked_set    = 77,
      mt_d_blocked_set    = 78,
      mt_blocked_clr      = 79,
      mt_trap_en          = 80,
      mt_trap             = 81,
      mt_trap_set         = 82,
      mt_trap_clr         = 83,
      sep                 = 84,
      
      /* 0x1cc-0x1FF Reserved */
      i_range0_hi         = 85,
      i_range1_hi         = 86,
      i_range2_hi         = 87,
      i_range3_hi         = 88,
      
      /* 0x210-0x21f Reserved */
      i_range0_lo         = 89,
      i_range1_lo         = 90,
      i_range2_lo         = 91,
      i_range3_lo         = 92,
      
      /* 0x230-0x23f Reserved */
      i_range0_en         = 93,
      i_range1_en         = 94,
      i_range2_en         = 95,
      i_range3_en         = 96,
      
      /* 0x250-0x25f Reserved */
      d_range0_hi         = 97,
      d_range1_hi         = 98,
      d_range2_hi         = 99,
      d_range3_hi         = 100,
      d_range4_hi         = 101,
      d_range5_hi         = 102,
      
      /* 0x278-0x27f Reserved */
      d_range0_lo         = 103,
      d_range1_lo         = 104,
      d_range2_lo         = 105,
      d_range3_lo         = 106,
      d_range4_lo         = 107,
      d_range5_lo         = 108,
      
      /* 0x298-0x29f Reserved */
      d_range0_en         = 109,
      d_range1_en         = 110,
      d_range2_en         = 111,
      d_range3_en         = 112,
      d_range4_en         = 113,
      d_range5_en         = 114,
      
      /* 0x2b8 - 0x2bf Reserved */
      i_range0_user_en    = 115,
      i_range1_user_en    = 116,
      i_range2_user_en    = 117,
      i_range3_user_en    = 118,
      
      /* 0x2d0-0x2df Reserved */
      d_range0_user_en    = 119,
      d_range1_user_en    = 120,
      d_range2_user_en    = 121,
      d_range3_user_en    = 122,
      d_range4_user_en    = 123,
      d_range5_user_en    = 124,

    };
    
    // constructor
    ubicom32cpu (const string& conf_file = "ubicom32-chiphw-gloss.conf");
    
    // load executable
    bool load (const string& object_file_name) 
     { return simwcpu::load ("cpu-loader", object_file_name); }

    // Some macros to make manufacturing of the cartesian-product
    // calls simpler.
#define DATA_READ(dtype) \
      sid::bus::status read_data_memory (sid::host_int_4 addr,    \
                                         dtype& data) \
        { return simwcpu::read_memory_bus (cpu_data_bus, addr, data); }
    
#define INSN_READ(dtype) \
      sid::bus::status read_insn_memory (sid::host_int_4 addr,    \
                                         dtype& data) \
        { return simwcpu::read_memory_bus (cpu_insn_bus, addr, data); }
    
#define DATA_WRITE(dtype) \
      sid::bus::status write_data_memory (sid::host_int_4 addr,    \
                                          const dtype& data) \
        { return simwcpu::write_memory_bus (cpu_data_bus, addr, data); }
    
#define INSN_WRITE(dtype) \
      sid::bus::status write_insn_memory (sid::host_int_4 addr,    \
                                          const dtype& data) \
        { return simwcpu::write_memory_bus (cpu_insn_bus, addr + 0x2000000, data); }
    
    // read/write memory calls for data and insn memory
    DATA_READ(sid::big_int_1);
    DATA_READ(sid::big_int_2);
    DATA_READ(sid::big_int_4);
    DATA_READ(sid::big_int_8);
    DATA_WRITE(sid::big_int_1);
    DATA_WRITE(sid::big_int_2);
    DATA_WRITE(sid::big_int_4);
    DATA_WRITE(sid::big_int_8);
    
    INSN_READ(sid::big_int_1);
    INSN_READ(sid::big_int_2);
    INSN_READ(sid::big_int_4);
    INSN_READ(sid::big_int_8);
    INSN_WRITE(sid::big_int_1);
    INSN_WRITE(sid::big_int_2);
    INSN_WRITE(sid::big_int_4);
    INSN_WRITE(sid::big_int_8);

#undef DATA_READ
#undef DATA_WRITE
#undef INSN_READ
#undef INSN_WRITE
    
    // block memory read/write
    sid::bus::status read_data_memory (sid::host_int_4 addr,
                                       unsigned int length,
				       sid::host_int_1 *buf);
    sid::bus::status write_data_memory (sid::host_int_4 addr,
                                        unsigned int length,
					sid::host_int_1 *buf);
    sid::bus::status read_insn_memory (sid::host_int_4 addr,
                                       unsigned int length,
				       sid::host_int_1 *buf);
    sid::bus::status write_insn_memory (sid::host_int_4 addr,
                                        unsigned int length,
					sid::host_int_1 *buf);

    // register access routines
    sid::component::status read_register (regno_t regno,
					  sid::host_int_4& value);
    sid::component::status read_register (int context,
					  regno_t regno,
					  sid::host_int_4& value);
    sid::component::status write_register (regno_t regno,
					   sid::host_int_4 value);
    sid::component::status write_register (int context,
					   regno_t regno,
					   sid::host_int_4 value);
    
    // translate interrupt number into appropriate interrupt pin
    sid::component::status interrupt (unsigned interrupt_number);
    
    // save/restore state of simulator
    void checkpoint_all_state (std::ostream& file);
    sid::component::status restore_all_state (std::istream& file);

    // get register attribute name from regno enum value
    static string get_regname (regno_t regno);

  private:

    sid::component *cpu;
    sid::bus *cpu_data_bus;
    sid::bus *cpu_insn_bus;

  };

  class ubicom32_main_plus_timer: public simwcpu
  {
    public:
    
    // default constructor
    ubicom32_main_plus_timer (const string& conf_file = "ubicom32-chiphw-gloss-loadf.conf");
    
    bool force_the_load (const string& loader_name)
      {
	sid::component *loader = this->lookup_component (loader_name);
	if (!loader)
	  return false;
       
	sid::pin *load_pin = loader->find_pin("load!");
	if(!load_pin)
	  return false;

	// Kick the pin to force the load
	load_pin->driven(1);

	return true;
      }

    // load core executable
    bool load_main (const string& object_file_name)
     { 
       bool ret= simwcpu::load ("cpu-loader", object_file_name); 
       if(ret == false)
	 return ret;

       return (force_the_load("cpu-loader"));
     }

    // Some macros to make manufacturing of the cartesian-product
    // calls simpler.
#define DATA_READ_MAIN(dtype) \
      sid::bus::status read_data_memory_main (sid::host_int_4 addr,    \
                                         dtype& data) \
        { return simwcpu::read_memory_bus (cpu_data_bus, addr, data); }

#define DATA_WRITE_MAIN(dtype) \
      sid::bus::status write_data_memory_main (sid::host_int_4 addr,    \
                                          const dtype& data) \
        { return simwcpu::write_memory_bus (cpu_data_bus, addr, data); }
    
#define INSN_READ_MAIN(dtype) \
      sid::bus::status read_insn_memory_main (sid::host_int_4 addr,    \
                                         dtype& data) \
        { return simwcpu::read_memory_bus (cpu_insn_bus, addr, data); }
    
#define INSN_WRITE_MAIN(dtype) \
      sid::bus::status write_insn_memory_main (sid::host_int_4 addr,    \
                                          const dtype& data) \
        { return simwcpu::write_memory_bus (cpu_insn_bus, addr, data); }


    // read/write memory calls for core data and insn memory
    DATA_READ_MAIN(sid::big_int_1);
    DATA_READ_MAIN(sid::big_int_2);
    DATA_READ_MAIN(sid::big_int_4);
    DATA_READ_MAIN(sid::big_int_8);
    DATA_WRITE_MAIN(sid::big_int_1);
    DATA_WRITE_MAIN(sid::big_int_2);
    DATA_WRITE_MAIN(sid::big_int_4);
    DATA_WRITE_MAIN(sid::big_int_8);
    
    INSN_READ_MAIN(sid::big_int_1);
    INSN_READ_MAIN(sid::big_int_2);
    INSN_READ_MAIN(sid::big_int_4);
    INSN_READ_MAIN(sid::big_int_8);
    INSN_WRITE_MAIN(sid::big_int_1);
    INSN_WRITE_MAIN(sid::big_int_2);
    INSN_WRITE_MAIN(sid::big_int_4);
    INSN_WRITE_MAIN(sid::big_int_8);

#undef DATA_READ_MAIN
#undef DATA_WRITE_MAIN
#undef INSN_READ_MAIN
#undef INSN_WRITE_MAIN

    // block memory read/write 
    sid::bus::status read_data_memory_main (sid::host_int_4 addr,    \
                                            unsigned int length,     \
					    sid::host_int_1 *buf);
    sid::bus::status write_data_memory_main (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_1 *buf);

    sid::bus::status read_insn_memory_main (sid::host_int_4 addr,    \
                                            unsigned int length,     \
					    sid::host_int_1 *buf);
    sid::bus::status write_insn_memory_main (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_1 *buf);
    sid::bus::status read_data_memory_main (sid::host_int_4 addr,    \
                                            unsigned int length,     \
					    sid::host_int_4 *buf);
    sid::bus::status write_data_memory_main (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_4 *buf);
    sid::bus::status read_insn_memory_main (sid::host_int_4 addr,    \
                                            unsigned int length,     \
					    sid::host_int_4 *buf);
    sid::bus::status write_insn_memory_main (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_4 *buf);

    sid::bus::status read_data_memory_main (sid::host_int_4 addr,    \
                                            unsigned int length,     \
					    sid::host_int_8 *buf);
    sid::bus::status write_data_memory_main (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_8 *buf);
    sid::bus::status read_insn_memory_main (sid::host_int_4 addr,    \
                                            unsigned int length,     \
					    sid::host_int_8 *buf);
    sid::bus::status write_insn_memory_main (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_8 *buf);

    // register access routines for core
    sid::component::status read_register_main (ubicom32cpu::regno_t regno,
					       sid::host_int_4& value);
    sid::component::status read_register_main (int context,
					       ubicom32cpu::regno_t regno,
					       sid::host_int_4& value);
    sid::component::status write_register_main (ubicom32cpu::regno_t regno,
					        sid::host_int_4 value);
    sid::component::status write_register_main (int context,
						ubicom32cpu::regno_t regno,
					        sid::host_int_4 value);
    
    sid::bus::status set_MMU_thread_num(host_int_4 value);
    sid::bus::status set_MMU_privilege_level(host_int_4 value);
    sid::bus::status snoop_MMU_read(host_int_4 value);

    sid::bus::status read_data_TLB_entry(host_int_4 index, host_int_8 &value);
    sid::bus::status write_data_TLB_entry(host_int_4 index, host_int_8 value);

    sid::bus::status read_insn_TLB_entry(host_int_4 index, host_int_8 &value);
    sid::bus::status write_insn_TLB_entry(host_int_4 index, host_int_8 value);

    // translate interrupt number into appropriate interrupt pin
    sid::component::status interrupt_main (unsigned interrupt_number);
    
    // save/restore state of simulator
    void checkpoint_all_state (std::ostream& file);
    sid::component::status restore_all_state (std::istream& file);

    //Timer block functions
    sid::component::status read_all_timer_registers(sid::host_int_4 *buf);
    sid::component::status write_all_timer_registers(sid::host_int_4 *buf);
    sid::component::status tick_mpt_clock(void);
    sid::component::status tick_sys_clock(void);

    // Scheduler, Execute, Retrieve Context functions
    int schedule();
    void execute_main();
    void execute_copro();
   
	bool has_MMU_flag; 
  private:

    sid::component *cpu, *mmu;
    sid::component *timer;	/* Timer block */
    sid::pin *mpt_pin;		/* Timer block mpt clock */
    sid::pin *sys_pin;		/* Timer block sys clock */
    sid::pin *scheduler_pin;	/* Scheduler driver pin */
    sid::pin *execute_pin_main;	/* execute driver pin for main */
    sid::bus *cpu_data_bus, *cpu_insn_bus;
    sid::bus *data_tlb_bus, *insn_tlb_bus;
    sid::bus *thread_num_bus, *privilege_level_bus;
    sid::bus *snoop_MMU_read_bus;

  };
}

#endif // IP3KSID_H
