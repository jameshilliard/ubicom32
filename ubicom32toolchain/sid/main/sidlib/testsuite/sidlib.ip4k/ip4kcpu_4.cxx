/* ip4kcpu sid encapsulation library test case 4
# mach(): ip4k

*/

// This test case is designed to load a program, manipulate it, and then
// save the state using the map interface.  Without destroying the
// instance, a new instance is created and then it is initialized using
// the previous map.  The new instance is then stepped to completion so
// it issues "pass".  The first instance is then destroyed without
// stepping to completion.

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
  big_int_1 top_byte;
  map<string,string> state;

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

  host_int_1 k[12];
  mycpu->read_insn_memory (pc, 4, k);

  CHECK(k[0]==0xc9);
  CHECK(k[1]==0x02);
  CHECK(k[2]==0x00);
  CHECK(k[3]==0x01);

  mycpu->read_register (ip4kcpu::d3, d3);
  mycpu->read_data_memory (d3, top_byte);

  CHECK(top_byte.integer_value() == 'h');

  top_byte = 'H';

  mycpu->write_data_memory (d3, top_byte);

  state = mycpu->save_state_map();

  mycpu->step(2);
  mycpu->write_register (ip4kcpu::d4, 15);
  mycpu->read_register (ip4kcpu::pc, pc);
  CHECK(pc == 20);

  big_int_4 insn = 0x2146f800; // bset int_set1,#0,#31
  mycpu->write_insn_memory (20, insn);

  ip4kcpu *newcpu = new ip4kcpu();
  newcpu->load("ip4kcpu.out");

  newcpu->restore_state_map(state);

  newcpu->read_insn_memory (20, insn);
  CHECK(insn == 0xc8000000);

  newcpu->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 5);

  newcpu->read_register (ip4kcpu::pc, pc);
  CHECK(pc == 12);

  newcpu->read_register (ip4kcpu::d3, d3);
  newcpu->read_data_memory (d3, top_byte);
  CHECK(top_byte.integer_value() == 'H');

  insn = 0x2146f800; // bset int_set1,#0,#31
  newcpu->write_insn_memory (20, insn);

  big_int_8 data;
  data.write_byte(0, 'p');
  data.write_byte(1, 'a');
  data.write_byte(2, 's');
  data.write_byte(3, 's');
  data.write_byte(4, '\n');
  data.write_byte(5, ' ');
  data.write_byte(6, ' ');
  data.write_byte(7, ' ');

  newcpu->write_data_memory (d3, data);

  newcpu->step(100);

  delete mycpu;
  delete newcpu;

  return 0;
}
