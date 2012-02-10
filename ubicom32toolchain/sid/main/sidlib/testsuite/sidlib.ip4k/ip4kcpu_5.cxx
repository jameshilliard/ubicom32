/* ip4kcpu sid encapsulation library test case 5
# mach(): ip4k

*/

// This test case is designed to test multiple instances all running
// to completion.  The test program in question will simply exit if
// not manipulated.  One of the programs will be manipulated to print
// "pass".

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

  ip4kcpu *mycpu = new ip4kcpu();

  mycpu->load ("ip4kcpu.out");
  mycpu->step (4);
  mycpu->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 12);
  mycpu->read_register (ip4kcpu::pc, pc);
  CHECK(pc == 16);

  d4 = 0;

  ip4kcpu *mycpu2 = new ip4kcpu();
  mycpu2->load ("ip4kcpu.out");
  mycpu2->step (3);
  mycpu2->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 12);
  mycpu2->read_register (ip4kcpu::pc, pc);
  CHECK(pc == 12);
  // save msg pointer for later
  mycpu2->read_register (ip4kcpu::d3, d3);

  d4 = 0;
  ip4kcpu *mycpu3 = new ip4kcpu();
  mycpu3->load ("ip4kcpu.out");
  mycpu3->step (2);
  mycpu3->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 12);
  mycpu3->read_register (ip4kcpu::pc, pc);
  CHECK(pc == 8);

  // manipulate mycpu2 to print out "pass"
  half = 0x0005;     
  mycpu2->write_insn_memory (2, half);  // change size of write to 5
  mycpu2->reset();

  mycpu2->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 0)
  mycpu2->read_register (ip4kcpu::pc, pc);
  CHECK(pc == 0);

  mycpu2->read_data_memory (d3, top_byte);
  CHECK(top_byte.integer_value() == 'h');

  big_int_4 new_msg;
  new_msg.write_byte(0, 'p');
  new_msg.write_byte(1, 'a');
  new_msg.write_byte(2, 's');
  new_msg.write_byte(3, 's');

  mycpu2->write_data_memory (d3, new_msg);
  top_byte = '\n';
  mycpu2->write_data_memory (d3+4, top_byte);
  
  // change insn at 20 to be bset int_set1,#0,#31 
  half = 0x2146;
  mycpu2->write_insn_memory (20, half);
  half = 0xf800;
  mycpu2->write_insn_memory (22, half);

  mycpu->step(100);
  mycpu2->step(100);
  mycpu3->step(100);

  delete mycpu2;
  delete mycpu;
  delete mycpu3;

  return 0;
}
