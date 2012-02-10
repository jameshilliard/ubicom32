/* ip3k_main_plus_copro sid encapsulation library test case 4
# mach(): ip3k

*/

// This test case is designed to load a program, manipulate it, and then
// save the state using the map interface.  Without destroying the
// instance, a new instance is created and then it is initialized using
// the previous map.  The new instance is then stepped to completion so
// it issues "pass".  The first instance is then destroyed without
// stepping to completion.

#include "ip3ksid.h"
#include "test.h"
#include <stdio.h>
#include <fstream>
using namespace sidlib;
using namespace sid;
using namespace std;

int main()
{
  host_int_4 d4 = 0;
  host_int_4 d12 = 0;
  host_int_4 pc = 0;
  host_int_4 stat0 = 0;
  host_int_8 r2 = 0;
  host_int_8 r6 = 0;
  host_int_8 cpc = 0;
  host_int_8 creg = 0;
  big_int_2 half;
  big_int_1 top_byte;
  string str;
  map<string,string> state;
  ip3k_main_plus_copro *mycpu = new ip3k_main_plus_copro();

  mycpu->load_main ("ip3kmpc_main.out");
  mycpu->load_copro ("ip3kmpc_copro.out");

  CHECK(mycpu->step (1));

  mycpu->read_register_main (ip3kcpu::d4, d4);

  CHECK(d4 == 12);

  // save msg pointer for later
  mycpu->read_register_main (ip3kcpu::d12, d12);

  half = 0x0005;     
  mycpu->write_register_main (ip3kcpu::d4, half);

  // 231 insns to where the copro starts, less 1 already executed, plus
  // 7 insns executed in parallel.
  CHECK((mycpu->step((231-1)+(7*2))));

  // save msg pointer for later
  mycpu->read_register_main (ip3kcpu::d12, d12);

  mycpu->read_register_copro (ip3kcopro::r2, r2);
  CHECK (r2 == 4);

  mycpu->read_register_main (ip3kcpu::pc, pc);

  mycpu->read_data_memory_main (d12, top_byte);
  CHECK(top_byte.integer_value() == 'h');

  big_int_4 new_msg;
  new_msg.write_byte(0, 'p');
  new_msg.write_byte(1, 'a');
  new_msg.write_byte(2, 's');
  new_msg.write_byte(3, 's');

  mycpu->write_data_memory_main (d12, new_msg);

  // still need to add newline
  top_byte = '\n';
  mycpu->write_data_memory_main (d12+4, top_byte);

  // change main insn at 0xdc to be bset int_set1,#0,#31 
  big_int_4 insn = 0x2146f800;
  mycpu->write_insn_memory_main (0xdc, insn);

  mycpu->read_insn_memory_main (0xde, half);

  CHECK(half == 0xf800);

  big_int_8 two_insns;

  // change copro regs to be correct to calculate value properly
  // ideally we would want to set r1 to 0 and r6 to 1, however,
  // we have caught r6 before it will be set by the delay slot
  // so we must add 1 to r1 to have the loop work out ok
  creg = 1;
  mycpu->write_register_copro (ip3kcopro::r1, creg);
  creg = 2;
  mycpu->write_register_copro (ip3kcopro::r2, creg);
  creg = 0x207e;
  mycpu->write_register_copro (ip3kcopro::r4, creg);

  mycpu->read_register_copro (ip3kcopro::pc, cpc);
  CHECK (cpc == 56);

  mycpu->read_data_memory_copro (0x2000, half);
  CHECK (half == 64);

  half = 65;
  mycpu->write_data_memory_copro (0x2000, half);
  
  state = mycpu->save_state_map ();
  delete mycpu;

  ip3k_main_plus_copro *newcpu = new ip3k_main_plus_copro();

  newcpu->load_main ("ip3kmpc_main.out");
  newcpu->load_copro ("ip3kmpc_copro.out");
 
  newcpu->restore_state_map (state);
 
  CHECK(!(newcpu->step(1000)));
  return 0;
}
