/* ip4kcpu sid encapsulation library test case 2
# mach(): ip4k

*/

// This test case is designed to load a program, manipulate it so it will
// print out "pass", then save the state to a file.  The original ip4kcpu
// instance is destroyed without running the program to completion.  A
// new ip4kcpu instance is created and restored to the state saved earlier.
// The program is run to completion which then prints out "pass".  Checks
// are made along the way to ensure that insn memory, data memory, and
// registers have values as expected are all successfully changed from
// their original values.

#include "ip4ksid.h"
#include "test.h"
#include <stdio.h>
#include <fstream>
using namespace sidlib;
using namespace sid;
using namespace std;

int main()
{
  host_int_4 d4 = 0;
  host_int_4 d3 = 0;
  host_int_4 pc = 0;
  big_int_2 half;
  big_int_1 top_byte;
  fstream save_state ("my.file",
                      ios::binary | ios::in | ios::out | ios::trunc);

  {
  ip4kcpu *mycpu = new ip4kcpu();

  mycpu->load ("ip4kcpu.out");

  mycpu->step (3);

  mycpu->read_register (ip4kcpu::d4, d4);

  CHECK(d4 == 12);
     
  mycpu->write_register (ip4kcpu::d4, 5);

  mycpu->read_register (ip4kcpu::d4, d4);

  CHECK(d4 == 5)

  mycpu->read_register (ip4kcpu::pc, pc);

  CHECK(pc == 12);

  mycpu->read_register (ip4kcpu::d3, d3);
  mycpu->read_data_memory (d3, top_byte);

  CHECK(top_byte.integer_value() == 'h');

  top_byte = 'p';
  mycpu->write_data_memory (d3, top_byte);
  top_byte = 'a';
  mycpu->write_data_memory (d3+1, top_byte);
  top_byte = 's';
  mycpu->write_data_memory (d3+2, top_byte);
  top_byte = 's';
  mycpu->write_data_memory (d3+3, top_byte);
  top_byte = '\n';
  mycpu->write_data_memory (d3+4, top_byte);
  
  host_int_1 k[12];
  mycpu->read_insn_memory (0, 8, k);

  CHECK(k[0]==0xc9);
  CHECK(k[1]==0x04);
  CHECK(k[2]==0x00);
  CHECK(k[3]==0x0c);
  CHECK(k[4]==0xe0);
  CHECK(k[5]==0x40);
  CHECK(k[6]==0xa0);
  CHECK(k[7]==0x00);
 
  big_int_4 z = 0x2146f800; // bset int_set1,#0,#31
  mycpu->write_insn_memory (20, z);
 
  mycpu->checkpoint_all_state (save_state);  /* save current state */

  delete mycpu;
  }

  ip4kcpu *newcpu = new ip4kcpu("ip4kcpu.out.conf");

  newcpu->load ("ip4kcpu.out");

  save_state.seekp(0);
  newcpu->restore_all_state (save_state);

  d4 = 0;
  newcpu->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 5);

  newcpu->read_register (ip4kcpu::pc, pc);
  CHECK(pc == 12);

  newcpu->read_register (ip4kcpu::d3, d3);
  newcpu->read_data_memory (d3, top_byte);
  CHECK(top_byte.integer_value() == 'p');

  newcpu->read_insn_memory (20, half);
  CHECK(half == 0x2146);
  newcpu->read_insn_memory (22, half);
  CHECK(half == 0xf800);

  newcpu->step(100);
  return 0;
}
