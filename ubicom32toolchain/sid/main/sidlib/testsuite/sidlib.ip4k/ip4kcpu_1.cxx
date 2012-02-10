/* ip4kcpu sid encapsulation library test case 1
# mach(): ip4k

*/

// This test case is designed to load a program, step a number of insns,
// then the first insn is changed and the data area manipulated so the
// program will print out "pass" if run from the start.  The program is
// reset and then run to completion.

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
  host_int_4 d8 = 0;
  host_int_4 pc = 0;
  host_int_4 tmp = 0;
  host_int_4 stat0 = 0;
  host_int_4 exit_code = 0;
  big_int_2 half;
  big_int_1 top_byte;
  string str;
  {
  ip4kcpu *mycpu = new ip4kcpu();

  mycpu->load ("ip4kcpu.out");

  for (int i=0; i < 8; ++i)
    {
      mycpu->write_register (i, ip4kcpu::d0, i*10 + ip4kcpu::d0);
      mycpu->write_register (i, ip4kcpu::d1, i*10 + ip4kcpu::d1);
      mycpu->write_register (i, ip4kcpu::d2, i*10 + ip4kcpu::d2);
      mycpu->write_register (i, ip4kcpu::d3, i*10 + ip4kcpu::d3);
      mycpu->write_register (i, ip4kcpu::d4, i*10 + ip4kcpu::d4);
      mycpu->write_register (i, ip4kcpu::d5, i*10 + ip4kcpu::d5);
      mycpu->write_register (i, ip4kcpu::d6, i*10 + ip4kcpu::d6);
      mycpu->write_register (i, ip4kcpu::d7, i*10 + ip4kcpu::d7);
      mycpu->write_register (i, ip4kcpu::d8, i*10 + ip4kcpu::d8);
      mycpu->write_register (i, ip4kcpu::d9, i*10 + ip4kcpu::d9);
      mycpu->write_register (i, ip4kcpu::d10, i*10 + ip4kcpu::d10);
      mycpu->write_register (i, ip4kcpu::d11, i*10 + ip4kcpu::d11);
      mycpu->write_register (i, ip4kcpu::d12, i*10 + ip4kcpu::d12);
      mycpu->write_register (i, ip4kcpu::d13, i*10 + ip4kcpu::d13);
      mycpu->write_register (i, ip4kcpu::d14, i*10 + ip4kcpu::d14);
      mycpu->write_register (i, ip4kcpu::d15, i*10 + ip4kcpu::d15);
      mycpu->write_register (i, ip4kcpu::d16, i*10 + ip4kcpu::d16);
      mycpu->write_register (i, ip4kcpu::d17, i*10 + ip4kcpu::d17);
      mycpu->write_register (i, ip4kcpu::d18, i*10 + ip4kcpu::d18);
      mycpu->write_register (i, ip4kcpu::d19, i*10 + ip4kcpu::d19);
      mycpu->write_register (i, ip4kcpu::d20, i*10 + ip4kcpu::d20);
      mycpu->write_register (i, ip4kcpu::d21, i*10 + ip4kcpu::d21);
      mycpu->write_register (i, ip4kcpu::d22, i*10 + ip4kcpu::d22);
      mycpu->write_register (i, ip4kcpu::d23, i*10 + ip4kcpu::d23);
      mycpu->write_register (i, ip4kcpu::d24, i*10 + ip4kcpu::d24);
      mycpu->write_register (i, ip4kcpu::d25, i*10 + ip4kcpu::d25);
      mycpu->write_register (i, ip4kcpu::d26, i*10 + ip4kcpu::d26);
      mycpu->write_register (i, ip4kcpu::d27, i*10 + ip4kcpu::d27);
      mycpu->write_register (i, ip4kcpu::d28, i*10 + ip4kcpu::d28);
      mycpu->write_register (i, ip4kcpu::d29, i*10 + ip4kcpu::d29);
      mycpu->write_register (i, ip4kcpu::d30, i*10 + ip4kcpu::d30);
      mycpu->write_register (i, ip4kcpu::d31, i*10 + ip4kcpu::d31);
      mycpu->write_register (i, ip4kcpu::a0, i*10 + ip4kcpu::a0);
      mycpu->write_register (i, ip4kcpu::a1, i*10 + ip4kcpu::a1);
      mycpu->write_register (i, ip4kcpu::a2, i*10 + ip4kcpu::a2);
      mycpu->write_register (i, ip4kcpu::a3, i*10 + ip4kcpu::a3);
      mycpu->write_register (i, ip4kcpu::a4, i*10 + ip4kcpu::a4);
      mycpu->write_register (i, ip4kcpu::a5, i*10 + ip4kcpu::a5);
      mycpu->write_register (i, ip4kcpu::a6, i*10 + ip4kcpu::a6);
      mycpu->write_register (i, ip4kcpu::sp, i*10 + ip4kcpu::sp);
      mycpu->write_register (i, ip4kcpu::mac_hi, i*10 + ip4kcpu::mac_hi);
      mycpu->write_register (i, ip4kcpu::mac_lo, i*10 + ip4kcpu::mac_lo);
      mycpu->write_register (i, ip4kcpu::mac_rc16, i*10 + ip4kcpu::mac_rc16);
      mycpu->write_register (i, ip4kcpu::source3, i*10 + ip4kcpu::source3);
      mycpu->write_register (i, ip4kcpu::int_mask0, i*10 + ip4kcpu::int_mask0);
      mycpu->write_register (i, ip4kcpu::int_mask1, i*10 + ip4kcpu::int_mask1);
    }

  for (int i=0; i < 8; ++i)
    {
      CHECK (mycpu->read_register (i,ip4kcpu::d0,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d0);
      CHECK (mycpu->read_register (i,ip4kcpu::d1,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d1);
      CHECK (mycpu->read_register (i,ip4kcpu::d2,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d2);
      CHECK (mycpu->read_register (i,ip4kcpu::d3,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d3);
      CHECK (mycpu->read_register (i,ip4kcpu::d4,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d4);
      CHECK (mycpu->read_register (i,ip4kcpu::d5,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d5);
      CHECK (mycpu->read_register (i,ip4kcpu::d6,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d6);
      CHECK (mycpu->read_register (i,ip4kcpu::d7,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d7);
      CHECK (mycpu->read_register (i,ip4kcpu::d8,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d8);
      CHECK (mycpu->read_register (i,ip4kcpu::d9,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d9);
      CHECK (mycpu->read_register (i,ip4kcpu::d10,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d10);
      CHECK (mycpu->read_register (i,ip4kcpu::d11,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d11);
      CHECK (mycpu->read_register (i,ip4kcpu::d12,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d12);
      CHECK (mycpu->read_register (i,ip4kcpu::d13,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d13);
      CHECK (mycpu->read_register (i,ip4kcpu::d14,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d14);
      CHECK (mycpu->read_register (i,ip4kcpu::d15,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d15);
      CHECK (mycpu->read_register (i,ip4kcpu::d16,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d16);
      CHECK (mycpu->read_register (i,ip4kcpu::d17,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d17);
      CHECK (mycpu->read_register (i,ip4kcpu::d18,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d18);
      CHECK (mycpu->read_register (i,ip4kcpu::d19,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d19);
      CHECK (mycpu->read_register (i,ip4kcpu::d20,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d20);
      CHECK (mycpu->read_register (i,ip4kcpu::d21,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d21);
      CHECK (mycpu->read_register (i,ip4kcpu::d22,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d22);
      CHECK (mycpu->read_register (i,ip4kcpu::d23,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d23);
      CHECK (mycpu->read_register (i,ip4kcpu::d24,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d24);
      CHECK (mycpu->read_register (i,ip4kcpu::d25,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d25);
      CHECK (mycpu->read_register (i,ip4kcpu::d26,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d26);
      CHECK (mycpu->read_register (i,ip4kcpu::d27,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d27);
      CHECK (mycpu->read_register (i,ip4kcpu::d28,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d28);
      CHECK (mycpu->read_register (i,ip4kcpu::d29,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d29);
      CHECK (mycpu->read_register (i,ip4kcpu::d30,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d30);
      CHECK (mycpu->read_register (i,ip4kcpu::d31,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::d31);
      CHECK (mycpu->read_register (i,ip4kcpu::a0,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::a0);
      CHECK (mycpu->read_register (i,ip4kcpu::a1,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::a1);
      CHECK (mycpu->read_register (i,ip4kcpu::a2,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::a2);
      CHECK (mycpu->read_register (i,ip4kcpu::a3,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::a3);
      CHECK (mycpu->read_register (i,ip4kcpu::a4,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::a4);
      CHECK (mycpu->read_register (i,ip4kcpu::a5,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::a5);
      CHECK (mycpu->read_register (i,ip4kcpu::a6,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::a6);
      CHECK (mycpu->read_register (i,ip4kcpu::sp,tmp) != component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::sp);
      CHECK (mycpu->read_register (i,ip4kcpu::mac_hi,tmp) != 
             component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::mac_hi);
      CHECK (mycpu->read_register (i,ip4kcpu::mac_lo,tmp) !=  
             component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::mac_lo);
      CHECK (mycpu->read_register (i,ip4kcpu::mac_rc16,tmp) != 
             component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::mac_rc16);
      CHECK (mycpu->read_register (i,ip4kcpu::source3,tmp) != 
             component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::source3);
      CHECK (mycpu->read_register (i,ip4kcpu::int_mask0,tmp) != 
             component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::int_mask0);
      CHECK (mycpu->read_register (i,ip4kcpu::int_mask1,tmp) != 
             component::not_found);
      CHECK (tmp == i*10 + ip4kcpu::int_mask1);
    }

  CHECK(mycpu->step (4));

  mycpu->read_register (ip4kcpu::d4, d4);

  CHECK(d4 == 12);

  CHECK(mycpu->get_attribute ("cpu", "syscall-trap-num", 
        str) == component::not_found);
  CHECK(mycpu->get_attribute ("cpu", "syscall-trap", str) == component::ok);
  CHECK(str == "63");
  CHECK(mycpu->set_attribute ("cpu", "syscall-trap-num", 
        str) == component::not_found);
  CHECK(mycpu->set_attribute ("cpu", "c0:d8", "15") == component::ok);
  mycpu->read_register (ip4kcpu::d8, d8);
  CHECK(d8 == 15);

  // save msg pointer for later
  mycpu->read_register (ip4kcpu::d3, d3);

  half = 0x0005;     
  mycpu->write_insn_memory (2, half);  // change size of write to 5

  d4 = 44;
  mycpu->write_register (ip4kcpu::d4, d4);
  mycpu->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 44)

  mycpu->reset();

  mycpu->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 0)

  mycpu->read_register (ip4kcpu::pc, pc);

  CHECK(pc == 0);

  mycpu->read_data_memory (d3, top_byte);
  CHECK(top_byte.integer_value() == 'h');

  big_int_4 new_msg;
  new_msg.write_byte(0, 'p');
  new_msg.write_byte(1, 'a');
  new_msg.write_byte(2, 's');
  new_msg.write_byte(3, 's');

  mycpu->write_data_memory (d3, new_msg);

  // still need to add newline
  top_byte = '\n';
  mycpu->write_data_memory (d3+4, top_byte);
  
  // change insn at 20 to be bset int_set1,#0,#31 
  half = 0x2146;
  mycpu->write_insn_memory (20, half);
  half = 0xf800;
  mycpu->write_insn_memory (22, half);

  host_int_1 k[12];
  mycpu->read_insn_memory (21, 3, k);

  CHECK(k[0]==0x46);
  CHECK(k[1]==0xf8);
  CHECK(k[2]==0x00);

  big_int_8 two_insns;
  mycpu->read_insn_memory (12, two_insns);
  CHECK(two_insns.read_byte(0)==0xC9);
  CHECK(two_insns.read_byte(1)==0x02);
  CHECK(two_insns.read_byte(2)==0x00);
  CHECK(two_insns.read_byte(3)==0x01);
  CHECK(two_insns.read_byte(4)==0xc9);
  CHECK(two_insns.read_byte(5)==0x01);
  CHECK(two_insns.read_byte(6)==0x00);
  CHECK(two_insns.read_byte(7)==0x05);

  mycpu->read_insn_memory (2, half);
  CHECK(half == 0x5);

  mycpu->step(1);

  big_int_4 insn;
  mycpu->read_insn_memory (0, insn);
  CHECK(insn == 0xc9040005);

  mycpu->read_register (ip4kcpu::pc, pc);
  CHECK(pc == 4);

  mycpu->read_register (ip4kcpu::d4, d4);
  CHECK(d4 == 5);

  mycpu->interrupt (0);
  mycpu->read_register (ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);
  /* global register should be the same for all contexts */
  mycpu->read_register (0, ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);
  mycpu->read_register (1, ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);
  mycpu->read_register (2, ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);
  mycpu->read_register (3, ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);
  mycpu->read_register (4, ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);
  mycpu->read_register (5, ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);
  mycpu->read_register (6, ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);
  mycpu->read_register (7, ip4kcpu::int_stat0, stat0);
  CHECK(stat0 == 0x1);

  CHECK(!(mycpu->step(100)));

  CHECK(mycpu->get_exit_code(exit_code) == component::ok);
  CHECK(exit_code == 3);
  }
  return 0;
}
