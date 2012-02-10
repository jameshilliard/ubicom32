/* ip3ki_main_plus_copro sid encapsulation library test case 2
# mach(): ip3k
  
*/
  
// This test case is designed to load a program, manipulate it so it will
// print out "pass", then save the state to a file.  The original
// instance is destroyed without running the program to completion.  A
// new instance is created and restored to the state saved earlier.
// The program is run to completion which then prints out "pass".  Checks
// are made along the way to ensure that insn memory, data memory, and
// registers have values as expected are all successfully changed from
// their original values.


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
  big_int_2 half;
  big_int_1 top_byte;
  string str;
  fstream save_state ("my.file",
                      ios::binary | ios::in | ios::out | ios::trunc);

  {
  ip3k_main_plus_copro *mycpu = new ip3k_main_plus_copro();

  mycpu->load_main ("ip3kmpc_main.out");
  mycpu->load_copro ("ip3kmpc_copro.out");

  CHECK(mycpu->step (4));

  mycpu->read_register_main (ip3kcpu::d4, d4);

  CHECK(d4 == 12);

  // save msg pointer for later
  mycpu->read_register_main (ip3kcpu::d12, d12);

  half = 0x0005;     
  mycpu->write_insn_memory_main (2, half);  // change size of write to 5

  d4 = 44;
  mycpu->write_register_main (ip3kcpu::d4, d4);
  mycpu->read_register_main (ip3kcpu::d4, d4);
  CHECK(d4 == 44)

  // 231 insns to where the copro starts, less 4 already executed, plus
  // 10 insns executed in parallel.
  CHECK((mycpu->step((231-4)+(10*2))));

  mycpu->read_register_copro (ip3kcopro::r2, r2);
  CHECK (r2 == 4);

  mycpu->reset();

  mycpu->read_register_main (ip3kcpu::d4, d4);
  CHECK(d4 == 0)

  mycpu->read_register_main (ip3kcpu::pc, pc);

  CHECK(pc == 0);

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
  half = 0x2146;
  mycpu->write_insn_memory_main (0xdc, half);
  half = 0xf800;
  mycpu->write_insn_memory_main (0xde, half);

  host_int_1 k[12];
  mycpu->read_insn_memory_main (0xdd, 3, k);

  CHECK(k[0]==0x46);
  CHECK(k[1]==0xf8);
  CHECK(k[2]==0x00);

  big_int_8 two_insns;

  // change copro insn at 0x4 to be ldi r1,#0 
  half = 0x1000;
  mycpu->write_insn_memory_copro (0x6, half);

  // change copro insn at 0x14 to be ldi r2,#2
  big_int_4 cinsn = 0x09c02002;
  mycpu->write_insn_memory_copro (0x14, cinsn);

  mycpu->read_insn_memory_copro (0x4,cinsn);
  CHECK(cinsn == 0x09c01000);

  mycpu->read_insn_memory_copro (0x10, two_insns);
  CHECK(two_insns.read_byte(0)==0x00);
  CHECK(two_insns.read_byte(1)==0x00);
  CHECK(two_insns.read_byte(2)==0x00);
  CHECK(two_insns.read_byte(3)==0x00);
  CHECK(two_insns.read_byte(4)==0x09);
  CHECK(two_insns.read_byte(5)==0xc0);
  CHECK(two_insns.read_byte(6)==0x20);
  CHECK(two_insns.read_byte(7)==0x02);

  mycpu->read_insn_memory_main (12, two_insns);
  CHECK(two_insns.read_byte(0)==0xC9);
  CHECK(two_insns.read_byte(1)==0x0a);
  CHECK(two_insns.read_byte(2)==0x00);
  CHECK(two_insns.read_byte(3)==0x00);
  CHECK(two_insns.read_byte(4)==0xe0);
  CHECK(two_insns.read_byte(5)==0x00);
  CHECK(two_insns.read_byte(6)==0x80);
  CHECK(two_insns.read_byte(7)==0x00);

  mycpu->read_insn_memory_main (2, half);
  CHECK(half == 0x5);

  mycpu->step(1);

  big_int_4 insn;
  mycpu->read_insn_memory_main (0, insn);
  CHECK(insn == 0xc9040005);

  mycpu->read_register_main (ip3kcpu::pc, pc);
  CHECK(pc == 4);

  mycpu->read_register_main (ip3kcpu::d4, d4);
  CHECK(d4 == 5);

  mycpu->read_register_copro (ip3kcopro::pc, cpc);
  CHECK(cpc == 40);
  mycpu->write_register_copro (ip3kcopro::pc, 0);

  mycpu->read_register_copro (ip3kcopro::r2, r2);
  CHECK(r2 == 4);

  mycpu->interrupt_main (0);
  mycpu->read_register_main (ip3kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);

  // 231 insns to where the copro starts, less 1 already executed, plus
  // 7 insns executed in parallel.
  CHECK((mycpu->step((231-1)+(7*2))));

  mycpu->read_register_copro (ip3kcopro::r2, r2);
  CHECK (r2 == 2);

  mycpu->read_register_copro (ip3kcopro::pc, cpc);
  CHECK (cpc == 56);

  mycpu->read_data_memory_copro (0x2000, half);
  CHECK (half == 64);

  half = 65;
  mycpu->write_data_memory_copro (0x2000, half);

  mycpu->checkpoint_all_state (save_state);  /* save current state */

  delete mycpu;
  }

  ip3k_main_plus_copro *newcpu = new ip3k_main_plus_copro("ip3kmpc.out.conf");

  newcpu->load_main ("ip3kmpc_main.out");
  newcpu->load_copro ("ip3kmpc_copro.out");

  save_state.seekp(0);
  newcpu->restore_all_state (save_state);

  big_int_4 new_insn;
  newcpu->read_insn_memory_main (0xdc, new_insn);
  CHECK (new_insn == 0x2146f800);

  newcpu->read_register_copro (ip3kcopro::r2, r2);
  CHECK (r2 == 2);

  newcpu->read_register_copro (ip3kcopro::pc, cpc);
  CHECK (cpc == 56);

  newcpu->read_data_memory_copro (0x2000, half);
  CHECK (half == 65);

  CHECK(!(newcpu->step(1000)));
  return 0;
}
