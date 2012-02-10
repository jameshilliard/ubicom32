/* ip3k_main_plus_copro sid encapsulation library test case 5
# mach(): ip3k

*/

// This test case is designed to test multiple instances all running
// to completion.  The test program in question will simply exit if
// not manipulated.  One of the programs will be manipulated to print
// "pass".

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
  CHECK(mycpu->step (4));
  mycpu->read_register_main (ip3kcpu::d4, d4);
  CHECK(d4 == 12);
  mycpu->read_register_main (ip3kcpu::pc, pc);
  CHECK(pc == 16);

  ip3k_main_plus_copro *mycpu2 = new ip3k_main_plus_copro();
  mycpu2->load_main ("ip3kmpc_main.out");
  mycpu2->load_copro ("ip3kmpc_copro.out");
  CHECK(mycpu2->step (1));
  mycpu2->read_register_main (ip3kcpu::d4, d4);
  CHECK(d4 == 12);
  mycpu2->read_register_main (ip3kcpu::pc, pc);
  CHECK(pc == 4);

  ip3k_main_plus_copro *mycpu3 = new ip3k_main_plus_copro();
  mycpu3->load_main ("ip3kmpc_main.out");
  mycpu3->load_copro ("ip3kmpc_copro.out");
  CHECK(mycpu3->step (8));
  mycpu3->read_register_main (ip3kcpu::d4, d4);
  CHECK(d4 == 12);
  mycpu3->read_register_main (ip3kcpu::pc, pc);
  CHECK(pc == 32);

  // save msg pointer for later
  mycpu2->read_register_main (ip3kcpu::d12, d12);

  half = 0x0005;     
  mycpu2->write_register_main (ip3kcpu::d4, half);

  // 231 insns to where the copro starts, less 1 already executed, plus
  // 7 insns executed in parallel.
  CHECK((mycpu2->step((231-1)+(7*2))));

  // save msg pointer for later
  mycpu2->read_register_main (ip3kcpu::d12, d12);

  mycpu2->read_register_copro (ip3kcopro::r2, r2);
  CHECK (r2 == 4);

  mycpu2->read_register_main (ip3kcpu::pc, pc);

  mycpu2->read_data_memory_main (d12, top_byte);
  CHECK(top_byte.integer_value() == 'h');

  big_int_4 new_msg;
  new_msg.write_byte(0, 'p');
  new_msg.write_byte(1, 'a');
  new_msg.write_byte(2, 's');
  new_msg.write_byte(3, 's');

  mycpu2->write_data_memory_main (d12, new_msg);

  // still need to add newline
  top_byte = '\n';
  mycpu2->write_data_memory_main (d12+4, top_byte);

  // change main insn at 0xdc to be bset int_set1,#0,#31 
  big_int_4 insn = 0x2146f800;
  mycpu2->write_insn_memory_main (0xdc, insn);

  mycpu2->read_insn_memory_main (0xde, half);

  CHECK(half == 0xf800);

  big_int_8 two_insns;

  // change copro regs to be correct to calculate value properly
  // ideally we would want to set r1 to 0 and r6 to 1, however,
  // we have caught r6 before it will be set by the delay slot
  // so we must add 1 to r1 to have the loop work out ok
  creg = 1;
  mycpu2->write_register_copro (ip3kcopro::r1, creg);
  creg = 2;
  mycpu2->write_register_copro (ip3kcopro::r2, creg);
  creg = 0x207e;
  mycpu2->write_register_copro (ip3kcopro::r4, creg);

  mycpu2->read_register_copro (ip3kcopro::pc, cpc);
  CHECK (cpc == 56);

  mycpu2->read_data_memory_copro (0x2000, half);
  CHECK (half == 64);

  half = 65;
  mycpu2->write_data_memory_copro (0x2000, half);
 
  CHECK(!(mycpu->step(1000)));
  CHECK(!(mycpu2->step(1000)));
  CHECK(!(mycpu3->step(1000)));
 
  delete mycpu3;
  delete mycpu;
  delete mycpu2;

  return 0;
}
