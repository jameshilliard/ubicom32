/* ip4kcopro sid encapsulation library test case 4
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
  host_int_8 r3 = 0;
  host_int_8 r2 = 0;
  host_int_8 pc = 0;
  big_int_1 top_byte;
  map<string,string> state;

  ip4kcopro *mycpu = new ip4kcopro();

  mycpu->load ("ip4kcopro.out");

  mycpu->step (3);

  mycpu->read_register (ip4kcopro::r3, r3);
  CHECK(r3 == 12);
     
  mycpu->write_register (ip4kcopro::r3, 5);
  mycpu->read_register (ip4kcopro::r3, r3);
  CHECK(r3 == 5)

  mycpu->read_register (ip4kcopro::pc, pc);
  CHECK(pc == 24);

  host_int_1 k[12];
  mycpu->read_insn_memory (pc+4, 4, k);

  CHECK(k[0]==0x09);
  CHECK(k[1]==0xc0);
  CHECK(k[2]==0x00);
  CHECK(k[3]==0x05);

  mycpu->read_register (ip4kcopro::r2, r2);
  mycpu->read_data_memory (r2, top_byte);

  CHECK(top_byte.integer_value() == 'h');

  top_byte = 'H';

  mycpu->write_data_memory (r2, top_byte);

  state = mycpu->save_state_map();

  mycpu->step(2);
  mycpu->write_register (ip4kcopro::r3, 15);
  mycpu->read_register (ip4kcopro::pc, pc);
  CHECK(pc == 40);

  big_int_4 insn = 0x0fe00025; // stop #37
  mycpu->write_insn_memory (36, insn);

  ip4kcopro *newcpu = new ip4kcopro();
  newcpu->load("ip4kcopro.out");

  newcpu->restore_state_map(state);

  newcpu->read_insn_memory (36, insn);
  CHECK(insn == 0x00000000);

  newcpu->read_register (ip4kcopro::r3, r3);
  CHECK(r3 == 5);

  newcpu->read_register (ip4kcopro::pc, pc);
  CHECK(pc == 24);

  newcpu->read_register (ip4kcopro::r2, r2);
  newcpu->read_data_memory (r2, top_byte);
  CHECK(top_byte.integer_value() == 'H');

  insn = 0x0fe00025; // stop #37
  newcpu->write_insn_memory (36, insn);

  big_int_8 data;
  data.write_byte(0, 'p');
  data.write_byte(1, 'a');
  data.write_byte(2, 's');
  data.write_byte(3, 's');
  data.write_byte(4, '\n');
  data.write_byte(5, ' ');
  data.write_byte(6, ' ');
  data.write_byte(7, ' ');

  newcpu->write_data_memory (r2, data);

  CHECK(!(newcpu->step(100)));

  delete mycpu;
  delete newcpu;

  return 0;
}
