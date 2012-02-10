/* ip3kcopro sid encapsulation library test case 1
# mach(): ip3k

*/

// This test case is designed to load a program, step a number of insns,
// then the first insn is changed and the data area manipulated so the
// program will print out "pass" if run from the start.  The program is
// reset and then run to completion.

#include "ip3ksid.h"
#include "test.h"
#include <stdio.h>
#include <fstream>
using namespace sidlib;
using namespace sid;
using namespace std;

int main()
{
  host_int_8 r2 = 0;
  host_int_8 r3 = 0;
  host_int_8 r8 = 0;
  host_int_8 pc = 0;
  big_int_2 half;
  big_int_1 top_byte;
  string str;
  {
  ip3kcopro *mycpu = new ip3kcopro();

  mycpu->load ("ip3kcopro.out");

  CHECK(mycpu->step (3));

  mycpu->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 12);

  CHECK(mycpu->get_attribute ("cpu", "isnt-there", str) ==
	component::not_found);
  CHECK(mycpu->get_attribute ("cpu", "num-data-banks", str) ==
	component::ok);
  CHECK(str == "8");
  CHECK(mycpu->set_attribute ("cpu", "isnt-there", "5") ==
	component::not_found);
  CHECK(mycpu->set_attribute ("cpu", "r8", "5") ==
	component::ok);
  mycpu->read_register (ip3kcopro::r8, r8);
  CHECK(r8 == 5);

  half = 0x3005;     
  mycpu->write_insn_memory (14, half);  // change size of write to 5

  r3 = 44;
  mycpu->write_register (ip3kcopro::r3, r3);
  mycpu->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 44)

  mycpu->reset();

  mycpu->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 44)
  mycpu->write_register (ip3kcopro::r3, 0);

  mycpu->read_register (ip3kcopro::pc, pc);
  CHECK(pc == 24);
  mycpu->write_register (ip3kcopro::pc, 0);

  mycpu->read_register (ip3kcopro::r2, r2);
  mycpu->read_data_memory (r2, top_byte);
  CHECK(top_byte.integer_value() == 'h');

  big_int_4 new_msg;
  new_msg.write_byte(0, 'p');
  new_msg.write_byte(1, 'a');
  new_msg.write_byte(2, 's');
  new_msg.write_byte(3, 's');

  mycpu->write_data_memory (r2, new_msg);

  // still need to add newline
  top_byte = '\n';
  mycpu->write_data_memory (r2+4, top_byte);
  
  // change insn at 36 to be stop #37
  half = 0x0fe0;
  mycpu->write_insn_memory (36, half);
  half = 0x0025;
  mycpu->write_insn_memory (38, half);

  host_int_1 k[12];
  mycpu->read_insn_memory (37, 3, k);

  CHECK(k[0]==0xe0);
  CHECK(k[1]==0x00);
  CHECK(k[2]==0x25);

  big_int_8 insn;
  mycpu->read_insn_memory (24, insn);
  CHECK(insn.read_byte(0)==0x00);
  CHECK(insn.read_byte(1)==0x00);
  CHECK(insn.read_byte(2)==0x00);
  CHECK(insn.read_byte(3)==0x00);
  CHECK(insn.read_byte(4)==0x09);
  CHECK(insn.read_byte(5)==0xc0);
  CHECK(insn.read_byte(6)==0x00);
  CHECK(insn.read_byte(7)==0x05);

  mycpu->read_insn_memory (14, half);
  CHECK(half == 0x3005);

  mycpu->step(2);

  big_int_4 scalar_insn;
  mycpu->read_insn_memory (12, scalar_insn);
  CHECK(scalar_insn == 0x09c03005);

  mycpu->read_register (ip3kcopro::pc, pc);
  CHECK(pc == 16);

  mycpu->read_register (ip3kcopro::r3, r3);
  CHECK(r3 == 5);

  CHECK(!(mycpu->step(100)));
  }
  return 0;
}
