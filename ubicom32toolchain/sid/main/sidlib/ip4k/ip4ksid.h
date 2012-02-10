// ip4ksid.h - Define the external interface of the ip4k SID encapsulation
// classes.

// Copyright (C) 2001, 2002 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.
  
#ifndef IP4KSID_H
#define IP4KSID_H 
  
#include <sidlib.h>
namespace sidlib
{
  class ip4kcpu: public simwcpu
  {
  public:
    
    // gdb register numbers for ip4k main 
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

#if 0
      d16                 = 16,
      d17                 = 17,
      d18                 = 18,
      d19                 = 19,
      d20                 = 20,
      d21                 = 21,
      d22                 = 22,
      d23                 = 23,
      d24                 = 24,
      d25                 = 25,
      d26                 = 26,
      d27                 = 27,
      d28                 = 28,
      d29                 = 29,
      d30                 = 30,
      d31                 = 31,
#endif

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
      pc                  = 34,
      chip_id             = 35,
      int_stat0           = 36,
      int_stat1           = 37,
      int_set0            = 38,
      int_set1            = 39,
      int_clr0            = 40,
      int_clr1            = 41,
      global_ctrl         = 42,
      mt_active_set       = 43,
      mt_active_clr       = 44,
      mt_active           = 45,
      mt_dbg_active_set   = 46,
      mt_dbg_active       = 47,
      mt_en               = 48,
      mt_hpri             = 49,
      mt_hrt              = 50,
      mt_break_clr        = 51,
      mt_break            = 52,
      mt_single_step      = 53,
      mt_min_delay_en     = 54,
      mt_dbg_active_clr   = 55,
      perr_addr           = 56,
      dcapt_tnum          = 57,
      dcapt_pc            = 58,
      dcapt               = 59,
      scratchpad0	  = 60,
      scratchpad1	  = 61,
      scratchpad2	  = 62,
      scratchpad3	  = 63,
    };
    
    // constructor
    ip4kcpu (const string& conf_file = "ip4k-chiphw-gloss.conf");
    
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

  class ip4kcopro: public simwcpu
  {
    public:

    enum regno_t
    {
      r0 = 0, r1 = 1, r2 = 2, r3 = 3,
      r4 = 4, r5 = 5, r6 = 6, r7 = 7,
      r8 = 8, r9 = 9, r10 = 10, r11 = 11,
      r12 = 12, r13 = 13, r14 = 14, r15 = 15,
      r16 = 16, r17 = 17, r18 = 18, r19 = 19,
      r20 = 20, r21 = 21, r22 = 22, r23 = 23,
      r24 = 24, r25 = 25, r26 = 26, r27 = 27,
      r28 = 28, r29 = 29, r30 = 30, r31 = 31,
      cpsr = 32, pc = 33, vcmp = 34, vz = 35, vc = 36,
      vacc0 = 37, vacc1 = 38, vacc2 = 39, vacc3 = 40,
      vacc4 = 41, vacc5 = 42, vacc6 = 43, vacc7 = 44,
      vr0_0 = 45, vr0_1 = 46, vr0_2 = 47, vr0_3 = 48,
      vr0_4 = 49, vr0_5 = 50, vr0_6 = 51, vr0_7 = 52,
      vr1_0 = 53, vr1_1 = 54, vr1_2 = 55, vr1_3 = 56,
      vr1_4 = 57, vr1_5 = 58, vr1_6 = 59, vr1_7 = 60,
      vr2_0 = 61, vr2_1 = 62, vr2_2 = 63, vr2_3 = 64,
      vr2_4 = 65, vr2_5 = 66, vr2_6 = 67, vr2_7 = 68,
      vr3_0 = 69, vr3_1 = 70, vr3_2 = 71, vr3_3 = 72,
      vr3_4 = 73, vr3_5 = 74, vr3_6 = 75, vr3_7 = 76,
      vr4_0 = 77, vr4_1 = 78, vr4_2 = 79, vr4_3 = 80,
      vr4_4 = 81, vr4_5 = 82, vr4_6 = 83, vr4_7 = 84,
      vr5_0 = 85, vr5_1 = 86, vr5_2 = 87, vr5_3 = 88,
      vr5_4 = 89, vr5_5 = 90, vr5_6 = 91, vr5_7 = 92,
      vr6_0 = 93, vr6_1 = 94, vr6_2 = 95, vr6_3 = 96,
      vr6_4 = 97, vr6_5 = 98, vr6_6 = 99, vr6_7 = 100,
      vr7_0 = 101, vr7_1 = 102, vr7_2 = 103, vr7_3 = 104,
      vr7_4 = 105, vr7_5 = 106, vr7_6 = 107, vr7_7 = 108,
      vr8_0 = 109, vr8_1 = 110, vr8_2 = 111, vr8_3 = 112,
      vr8_4 = 113, vr8_5 = 114, vr8_6 = 115, vr8_7 = 116,
      vr9_0 = 117, vr9_1 = 118, vr9_2 = 119, vr9_3 = 120,
      vr9_4 = 121, vr9_5 = 122, vr9_6 = 123, vr9_7 = 124,
      vr10_0 = 125, vr10_1 = 126, vr10_2 = 127, vr10_3 = 128,
      vr10_4 = 129, vr10_5 = 130, vr10_6 = 131, vr10_7 = 132,
      vr11_0 = 133, vr11_1 = 134, vr11_2 = 135, vr11_3 = 136,
      vr11_4 = 137, vr11_5 = 138, vr11_6 = 139, vr11_7 = 140,
      vr12_0 = 141, vr12_1 = 142, vr12_2 = 143, vr12_3 = 144,
      vr12_4 = 145, vr12_5 = 146, vr12_6 = 147, vr12_7 = 148,
      vr13_0 = 149, vr13_1 = 150, vr13_2 = 151, vr13_3 = 152,
      vr13_4 = 153, vr13_5 = 154, vr13_6 = 155, vr13_7 = 156,
      vr14_0 = 157, vr14_1 = 158, vr14_2 = 159, vr14_3 = 160,
      vr14_4 = 161, vr14_5 = 162, vr14_6 = 163, vr14_7 = 164,
      vr15_0 = 165, vr15_1 = 166, vr15_2 = 167, vr15_3 = 168,
      vr15_4 = 169, vr15_5 = 170, vr15_6 = 171, vr15_7 = 172,
      vr16_0 = 173, vr16_1 = 174, vr16_2 = 175, vr16_3 = 176,
      vr16_4 = 177, vr16_5 = 178, vr16_6 = 179, vr16_7 = 180,
      vr17_0 = 181, vr17_1 = 182, vr17_2 = 183, vr17_3 = 184,
      vr17_4 = 185, vr17_5 = 186, vr17_6 = 187, vr17_7 = 188,
      vr18_0 = 189, vr18_1 = 190, vr18_2 = 191, vr18_3 = 192,
      vr18_4 = 193, vr18_5 = 194, vr18_6 = 195, vr18_7 = 196,
      vr19_0 = 197, vr19_1 = 198, vr19_2 = 199, vr19_3 = 200,
      vr19_4 = 201, vr19_5 = 202, vr19_6 = 203, vr19_7 = 204,
      vr20_0 = 205, vr20_1 = 206, vr20_2 = 207, vr20_3 = 208,
      vr20_4 = 209, vr20_5 = 210, vr20_6 = 211, vr20_7 = 212,
      vr21_0 = 213, vr21_1 = 214, vr21_2 = 215, vr21_3 = 216,
      vr21_4 = 217, vr21_5 = 218, vr21_6 = 219, vr21_7 = 220,
      vr22_0 = 221, vr22_1 = 222, vr22_2 = 223, vr22_3 = 224,
      vr22_4 = 225, vr22_5 = 226, vr22_6 = 227, vr22_7 = 228,
      vr23_0 = 229, vr23_1 = 230, vr23_2 = 231, vr23_3 = 232,
      vr23_4 = 233, vr23_5 = 234, vr23_6 = 235, vr23_7 = 236,
      vr24_0 = 237, vr24_1 = 238, vr24_2 = 239, vr24_3 = 240,
      vr24_4 = 241, vr24_5 = 242, vr24_6 = 243, vr24_7 = 244,
      vr25_0 = 245, vr25_1 = 246, vr25_2 = 247, vr25_3 = 248,
      vr25_4 = 249, vr25_5 = 250, vr25_6 = 251, vr25_7 = 252,
      vr26_0 = 253, vr26_1 = 254, vr26_2 = 255, vr26_3 = 256,
      vr26_4 = 257, vr26_5 = 258, vr26_6 = 259, vr26_7 = 260,
      vr27_0 = 261, vr27_1 = 262, vr27_2 = 263, vr27_3 = 264,
      vr27_4 = 265, vr27_5 = 266, vr27_6 = 267, vr27_7 = 268,
      vr28_0 = 269, vr28_1 = 270, vr28_2 = 271, vr28_3 = 272,
      vr28_4 = 273, vr28_5 = 274, vr28_6 = 275, vr28_7 = 276,
      vr29_0 = 277, vr29_1 = 278, vr29_2 = 279, vr29_3 = 280,
      vr29_4 = 281, vr29_5 = 282, vr29_6 = 283, vr29_7 = 284,
      vr30_0 = 285, vr30_1 = 286, vr30_2 = 287, vr30_3 = 288,
      vr30_4 = 289, vr30_5 = 290, vr30_6 = 291, vr30_7 = 292,
      vr31_0 = 293, vr31_1 = 294, vr31_2 = 295, vr31_3 = 296,
      vr31_4 = 297, vr31_5 = 298, vr31_6 = 299, vr31_7 = 300,
    }; 

    // constructor
    ip4kcopro (const string& conf_file = "ip4kmath-gloss.conf");
    
    // load executable
    bool load (const string& object_file_name)
     { return simwcpu::load ("cpu-loader", object_file_name); }
     
    // Some macros to make manufacturing of the cartesian-product
    // calls simpler.
#define DATA_READ(dtype) \
      sid::bus::status read_data_memory (sid::host_int_4 addr,    \
                                         dtype& data) \
        { return simwcpu::read_memory_bus (copro_data_bus, addr, data); }
    
#define INSN_READ(dtype) \
      sid::bus::status read_insn_memory (sid::host_int_4 addr,    \
                                         dtype& data) \
        { return simwcpu::read_memory_bus (copro_insn_bus, addr + 0x4000000, data); }
    
#define DATA_WRITE(dtype) \
      sid::bus::status write_data_memory (sid::host_int_4 addr,    \
                                          const dtype& data) \
        { return simwcpu::write_memory_bus (copro_data_bus, addr, data); }
    
#define INSN_WRITE(dtype) \
      sid::bus::status write_insn_memory (sid::host_int_4 addr,    \
                                          const dtype& data) \
        { return simwcpu::write_memory_bus (copro_insn_bus, addr + 0x4000000, data); }
    
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
					  sid::host_int_8& value);
    sid::component::status write_register (regno_t regno,
					   sid::host_int_8 value);
    
    // translate interrupt number into appropriate interrupt pin
    sid::component::status interrupt (unsigned interrupt_number);
    
    // save/restore state of simulator
    void checkpoint_all_state (std::ostream& file);
    sid::component::status restore_all_state (std::istream& file);

  private:

    sid::component *copro;
    sid::bus *copro_data_bus;
    sid::bus *copro_insn_bus;

  };

  class ip4k_main_plus_copro: public simwcpu
  {
    public:
    
    // default constructor
    ip4k_main_plus_copro (const string& conf_file = "ip4k-chiphw-gloss-load-coprocessor.conf");
    
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

    // load copro executable
    bool load_copro (const string& object_file_name)
      { 
	bool ret = simwcpu::load ("coprocessor-loader", object_file_name);
	if(ret == false)
	  return ret;

	return(force_the_load("coprocessor-loader"));
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

#define DATA_READ_COPRO(dtype) \
      sid::bus::status read_data_memory_copro (sid::host_int_4 addr,    \
                                         dtype& data) \
        { return simwcpu::read_memory_bus (copro_data_bus, addr, data); }

#define DATA_WRITE_COPRO(dtype) \
      sid::bus::status write_data_memory_copro (sid::host_int_4 addr,    \
                                          const dtype& data) \
        { return simwcpu::write_memory_bus (copro_data_bus, addr, data); }
    
#define INSN_READ_COPRO(dtype) \
      sid::bus::status read_insn_memory_copro (sid::host_int_4 addr,    \
                                         dtype& data) \
        { return simwcpu::read_memory_bus (copro_insn_bus, addr + 0x4000000, data); }
    
#define INSN_WRITE_COPRO(dtype) \
      sid::bus::status write_insn_memory_copro (sid::host_int_4 addr,    \
                                          const dtype& data) \
        { return simwcpu::write_memory_bus (copro_insn_bus, addr + 0x4000000, data); }

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

    // read/write memory calls for coprocesor data
    DATA_READ_COPRO(sid::big_int_1);
    DATA_READ_COPRO(sid::big_int_2);
    DATA_READ_COPRO(sid::big_int_4);
    DATA_READ_COPRO(sid::big_int_8);
    DATA_WRITE_COPRO(sid::big_int_1);
    DATA_WRITE_COPRO(sid::big_int_2);
    DATA_WRITE_COPRO(sid::big_int_4);
    DATA_WRITE_COPRO(sid::big_int_8);
 
    INSN_READ_COPRO(sid::big_int_1);
    INSN_READ_COPRO(sid::big_int_2);
    INSN_READ_COPRO(sid::big_int_4);
    INSN_READ_COPRO(sid::big_int_8);
    INSN_WRITE_COPRO(sid::big_int_1);
    INSN_WRITE_COPRO(sid::big_int_2);
    INSN_WRITE_COPRO(sid::big_int_4);
    INSN_WRITE_COPRO(sid::big_int_8);

#undef DATA_READ_MAIN
#undef DATA_WRITE_MAIN
#undef DATA_READ_COPRO
#undef DATA_WRITE_COPRO
#undef INSN_READ_MAIN
#undef INSN_WRITE_MAIN
#undef INSN_READ_COPRO
#undef INSN_WRITE_COPRO

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
    sid::bus::status read_data_memory_copro (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_1 *buf);
    sid::bus::status write_data_memory_copro (sid::host_int_4 addr,    \
                                              unsigned int length,     \
					      sid::host_int_1 *buf);
    sid::bus::status read_insn_memory_copro (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_1 *buf);
    sid::bus::status write_insn_memory_copro (sid::host_int_4 addr,    \
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
    sid::bus::status read_data_memory_copro (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_4 *buf);
    sid::bus::status write_data_memory_copro (sid::host_int_4 addr,    \
                                              unsigned int length,     \
					      sid::host_int_4 *buf);
    sid::bus::status read_insn_memory_copro (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_4 *buf);
    sid::bus::status write_insn_memory_copro (sid::host_int_4 addr,    \
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
    sid::bus::status read_data_memory_copro (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_8 *buf);
    sid::bus::status write_data_memory_copro (sid::host_int_4 addr,    \
                                              unsigned int length,     \
					      sid::host_int_8 *buf);
    sid::bus::status read_insn_memory_copro (sid::host_int_4 addr,    \
                                             unsigned int length,     \
					     sid::host_int_8 *buf);
    sid::bus::status write_insn_memory_copro (sid::host_int_4 addr,    \
                                              unsigned int length,     \
					      sid::host_int_8 *buf);
 

    // register access routines for core
    sid::component::status read_register_main (ip4kcpu::regno_t regno,
					       sid::host_int_4& value);
    sid::component::status read_register_main (int context,
					       ip4kcpu::regno_t regno,
					       sid::host_int_4& value);
    sid::component::status write_register_main (ip4kcpu::regno_t regno,
					        sid::host_int_4 value);
    sid::component::status write_register_main (int context,
						ip4kcpu::regno_t regno,
					        sid::host_int_4 value);
    // register access routines for core
    sid::component::status read_register_copro (ip4kcopro::regno_t regno,
					        sid::host_int_8& value);
    sid::component::status write_register_copro (ip4kcopro::regno_t regno,
					         sid::host_int_8 value);
    
    
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
    
  private:

    sid::component *cpu;
    sid::component *copro;
    sid::component *timer;	/* Timer block */
    sid::pin *mpt_pin;		/* Timer block mpt clock */
    sid::pin *sys_pin;		/* Timer block sys clock */
    sid::pin *scheduler_pin;	/* Scheduler driver pin */
    sid::pin *execute_pin_main;	/* execute driver pin for main */
    sid::pin *execute_pin_copro; /* execute driver pin for copro */
    sid::bus *cpu_data_bus;
    sid::bus *cpu_insn_bus;
    sid::bus *copro_data_bus;
    sid::bus *copro_insn_bus;

  };
}

#endif // IP4KSID_H
