/* ip3kcopro sid encapsulation library test case 2
# mach(): ip3k

*/

// This test case is designed to load a program, manipulate it so it will
// print out "pass", then save the state to a file.  The original ip3kcopro
// instance is destroyed without running the program to completion.  A
// new ip3kcopro instance is created and restored to the state saved earlier.
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
  host_int_8 r3 = 0;
  host_int_8 r2 = 0;
  host_int_8 pc = 0;
  big_int_2 half;
  big_int_1 top_byte;
  fstream save_state ("my.file",
                      ios::binary | ios::in | ios::out | ios::trunc);

  {
  ip3kcopro *mycpu = new ip3kcopro();

  mycpu->load ("ip3kcopro.out");

  mycpu->step (3);

  mycpu->read_register (ip3kcopro::r3, r3);

  CHECK(r3 == 12);
     
  mycpu->write_register (ip3kcopro::r3, 5);
  mycpu->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 5)

  mycpu->read_register (ip3kcopro::pc, pc);
  CHECK(pc == 24);

  mycpu->read_register (ip3kcopro::r2, r2);
  mycpu->read_data_memory (r2, top_byte);

  CHECK(top_byte.integer_value() == 'h');

  top_byte = 'p';
  mycpu->write_data_memory (r2, top_byte);
  top_byte = 'a';
  mycpu->write_data_memory (r2+1, top_byte);
  top_byte = 's';
  mycpu->write_data_memory (r2+2, top_byte);
  top_byte = 's';
  mycpu->write_data_memory (r2+3, top_byte);
  top_byte = '\n';
  mycpu->write_data_memory (r2+4, top_byte);
  
  host_int_1 k[12];
  mycpu->read_insn_memory (8, 8, k);

  CHECK(k[0]==0x00);
  CHECK(k[1]==0x00);
  CHECK(k[2]==0x00);
  CHECK(k[3]==0x00);
  CHECK(k[4]==0x09);
  CHECK(k[5]==0xc0);
  CHECK(k[6]==0x30);
  CHECK(k[7]==0x0c);
 
  big_int_4 z = 0x0fe00025; // stop #37
  mycpu->write_insn_memory (36, z);
 
  mycpu->checkpoint_all_state (save_state);  /* save current state */

  delete mycpu;
  }

  ip3kcopro *newcpu = new ip3kcopro("ip3kcopro.out.conf");

  newcpu->load ("ip3kcopro.out");

  save_state.seekp(0);
  newcpu->restore_all_state (save_state);

  r3 = 0;
  newcpu->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 5);

  newcpu->read_register (ip3kcopro::pc, pc);
  CHECK(pc == 24);

  newcpu->read_register (ip3kcopro::r2, r2);
  newcpu->read_data_memory (r2, top_byte);
  CHECK(top_byte.integer_value() == 'p');

  newcpu->read_insn_memory (36, half);
  CHECK(half == 0x0fe0);
  newcpu->read_insn_memory (38, half);
  CHECK(half == 0x0025);

  CHECK(!(newcpu->step(100)));
  return 0;
}
