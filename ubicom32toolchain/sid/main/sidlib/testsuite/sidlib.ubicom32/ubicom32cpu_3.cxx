/* ip3kcpu sid encapsulation library test case 3
# mach(): ip3k

*/

// This test case is designed to load a program, manipulate it, and then
// save the state using the map interface.  Without destroying the
// instance, it is manipulated further then restored via the map.
// The program is further manipulated to make it print "pass" then
// the program is run to completion.

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
  host_int_4 d3 = 0;
  host_int_4 pc = 0;
  big_int_1 top_byte;
  map<string,string> state;

  ip3kcpu *mycpu = new ip3kcpu();

  mycpu->load ("ip3kcpu.out");

  mycpu->step (3);

  mycpu->read_register (ip3kcpu::d4, d4);

  CHECK(d4 == 12);
     
  mycpu->write_register (ip3kcpu::d4, 5);

  mycpu->read_register (ip3kcpu::d4, d4);

  CHECK(d4 == 5)

  mycpu->read_register (ip3kcpu::pc, pc);

  CHECK(pc == 12);

  host_int_1 k[12];
  mycpu->read_insn_memory (pc, 4, k);

  CHECK(k[0]==0xc9);
  CHECK(k[1]==0x02);
  CHECK(k[2]==0x00);
  CHECK(k[3]==0x01);

  mycpu->read_register (ip3kcpu::d3, d3);
  mycpu->read_data_memory (d3, top_byte);

  CHECK(top_byte.integer_value() == 'h');

  top_byte = 'H';

  mycpu->write_data_memory (d3, top_byte);

  state = mycpu->save_state_map();

  mycpu->step(2);
  mycpu->write_register (ip3kcpu::d4, 15);
  mycpu->read_register (ip3kcpu::pc, pc);
  CHECK(pc == 20);

  big_int_4 insn = 0x2146f800; // bset int_set1,#0,#31
  mycpu->write_insn_memory (20, insn);

  mycpu->restore_state_map(state);

  mycpu->read_insn_memory (20, insn);
  CHECK(insn == 0xc8000000);

  mycpu->read_register (ip3kcpu::d4, d4);
  CHECK(d4 == 5);

  mycpu->read_register (ip3kcpu::pc, pc);
  CHECK(pc == 12);

  mycpu->read_register (ip3kcpu::d3, d3);
  mycpu->read_data_memory (d3, top_byte);
  CHECK(top_byte.integer_value() == 'H');

  insn = 0x2146f800; // bset int_set1,#0,#31
  mycpu->write_insn_memory (20, insn);

  big_int_8 data;
  data.write_byte(0, 'p');
  data.write_byte(1, 'a');
  data.write_byte(2, 's');
  data.write_byte(3, 's');
  data.write_byte(4, '\n');
  data.write_byte(5, ' ');
  data.write_byte(6, ' ');
  data.write_byte(7, ' ');

  mycpu->write_data_memory (d3, data);

  mycpu->step(100);

  return 0;
}
