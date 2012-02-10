/* ip3kcopro sid encapsulation library test case 5
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
  host_int_8 r3 = 0;
  host_int_8 r2 = 0;
  host_int_8 pc = 0;
  big_int_2 half;
  big_int_1 top_byte;

  ip3kcopro *mycpu = new ip3kcopro();

  mycpu->load ("ip3kcopro.out");
  mycpu->step (4);
  mycpu->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 12);
  mycpu->read_register (ip3kcopro::pc, pc);
  CHECK(pc == 32);

  r3 = 0;

  ip3kcopro *mycpu2 = new ip3kcopro();
  mycpu2->load ("ip3kcopro.out");
  mycpu2->step (3);
  mycpu2->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 12);
  mycpu2->read_register (ip3kcopro::pc, pc);
  CHECK(pc == 24);
  // save msg pointer for later
  mycpu2->read_register (ip3kcopro::r2, r2);

  r3 = 0;
  ip3kcopro *mycpu3 = new ip3kcopro();
  mycpu3->load ("ip3kcopro.out");
  mycpu3->step (2);
  mycpu3->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 12);
  mycpu3->read_register (ip3kcopro::pc, pc);
  CHECK(pc == 16);

  // manipulate mycpu2 to print out "pass"
  half = 0x3005;     
  mycpu2->write_insn_memory (14, half);  // change size of write to 5
  mycpu2->reset();

  mycpu2->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 12)
  mycpu2->write_register (ip3kcopro::r3, 0);
  mycpu2->read_register (ip3kcopro::pc, pc);
  CHECK(pc == 24);
  mycpu2->write_register (ip3kcopro::pc, 0);

  mycpu2->read_data_memory (r2, top_byte);
  CHECK(top_byte.integer_value() == 'h');

  big_int_4 new_msg;
  new_msg.write_byte(0, 'p');
  new_msg.write_byte(1, 'a');
  new_msg.write_byte(2, 's');
  new_msg.write_byte(3, 's');

  mycpu2->write_data_memory (r2, new_msg);
  top_byte = '\n';
  mycpu2->write_data_memory (r2+4, top_byte);
  
  // change insn at 36 to be stop #37
  half = 0x0fe0;
  mycpu2->write_insn_memory (36, half);
  half = 0x0025;
  mycpu2->write_insn_memory (38, half);

  CHECK(!(mycpu->step(100)));
  CHECK(!(mycpu2->step(100)));
  CHECK(!(mycpu3->step(100)));

  delete mycpu2;
  delete mycpu;
  delete mycpu3;

  return 0;
}
