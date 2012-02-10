// ip4kcpu.cxx - ip4k main processor sid encapsulation.  -*- C++ -*-

// Copyright (C) 2001, 2002 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include "config.h"

#include <ip4ksid.h>
#include <sidcpuutil.h>
#include <stdlib.h>

using namespace std;
using namespace sid;
using namespace sidlib;
using namespace sidutil;


// ----------------------------------------------------------------------------

ip4kcpu::ip4kcpu (const string& conf_file_name) :
  simwcpu (conf_file_name)
{
  // set the insn step count for the cpu to be 1
  this->cpu = lookup_component ("cpu");
  assert (cpu != 0);
  this->cpu_data_bus = cpu->find_bus ("data-bus");
  assert (cpu_data_bus != 0);
  this->cpu_insn_bus = cpu->find_bus ("debugger-bus");
  assert (cpu_insn_bus != 0);

  cpu->set_attribute_value ("step-insn-count", "1");
}

sid::bus::status 
ip4kcpu::read_data_memory (sid::host_int_4 addr,
			   unsigned int length,
			   sid::host_int_1 *buf)
{
  for (unsigned int i = 0; i < length; ++i)
    {
      big_int_1 k;
      sid::bus::status s = this->read_data_memory (addr, k);
      if (s != sid::bus::ok)
	return s;
      buf[i] = k;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip4kcpu::write_data_memory (sid::host_int_4 addr,
		  	    unsigned int length,
			    sid::host_int_1 *buf)
{
  for (unsigned int i = 0; i < length; ++i)
    {
      big_int_1 k = buf[i];
      sid::bus::status s = this->write_data_memory (addr, k);
      if (s != sid::bus::ok)
	return s;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip4kcpu::read_insn_memory (sid::host_int_4 addr,
			   unsigned int length,
			   sid::host_int_1 *buf)
{
  for (unsigned int i = 0; i < length; ++i)
    {
      big_int_1 k;
      sid::bus::status s = this->read_insn_memory (addr, k);
      if (s != sid::bus::ok)
	return s;
      buf[i] = k;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip4kcpu::write_insn_memory (sid::host_int_4 addr,
		  	    unsigned int length,
			    sid::host_int_1 *buf)
{
  for (unsigned int i = 0; i < length; ++i)
    {
      big_int_1 k = buf[i];
      sid::bus::status s = this->write_insn_memory (addr, k);
      if (s != sid::bus::ok)
	return s;
      ++addr;
    }

  return sid::bus::ok;
}

sid::component::status 
ip4kcpu::read_register (ip4kcpu::regno_t regno,
			sid::host_int_4& value)
{
  check_load ();
  string regname = string("gdb-register-") + sidutil::make_numeric_attribute (regno);
  string reg_value = cpu->attribute_value (regname);
  big_int_4 target_value = 0;
  if (reg_value.size() > 0)
    {
      for (unsigned int i = 0; i < reg_value.size(); ++i)
	target_value.write_byte (i, reg_value[i]);
      value = target_value;
      return sid::component::ok;
    }
  return sid::component::not_found;
}

sid::component::status 
ip4kcpu::read_register (int context,
			ip4kcpu::regno_t regno,
			sid::host_int_4& value)
{
  check_load ();
  string regname = string("c") + sidutil::make_numeric_attribute (context) + 
    string(":") + ip4kcpu::get_regname (regno);
  string reg_value = cpu->attribute_value (regname);
  if (reg_value.size() > 0)
    {
      value = atol (reg_value.c_str());
      return sid::component::ok;
    }

  /* otherwise assume it is global register and try again */
  return this->read_register (regno, value);
}

sid::component::status 
ip4kcpu::write_register (ip4kcpu::regno_t regno,
			 sid::host_int_4 value)
{
  check_load ();
  string regname = string("gdb-register-") + sidutil::make_numeric_attribute (regno);
  big_int_4 target_value = value;
  string attr;
  for (unsigned int i = 0; i < 4; ++i)
    attr += target_value.read_byte (i);
  return cpu->set_attribute_value (regname, attr);
}

sid::component::status 
ip4kcpu::write_register (int context,
			 ip4kcpu::regno_t regno,
			 sid::host_int_4 value)
{
  check_load ();
  string regname = string("c") + sidutil::make_numeric_attribute (context) + 
    string(":") + ip4kcpu::get_regname (regno);
  if (cpu->set_attribute_value (regname, sidutil::make_numeric_attribute (value)) 
                                == sid::component::not_found)
    return this->write_register (regno, value);
}

sid::component::status
ip4kcpu::interrupt (unsigned irq_num)
{
  if (irq_num < 0 || irq_num > 31)
    return component::not_found;

  check_load ();
  string pin_name = string("irq_") + sidutil::make_numeric_attribute (irq_num);
  sid::pin *irq_pin = cpu->find_pin (pin_name);

  if (!irq_pin)
    return component::not_found;

  irq_pin->driven (1);
  return component::ok;
}

void
ip4kcpu::checkpoint_all_state (std::ostream& file)
{
  map<string,string> snapshot = simwcpu::save_state_map ();

  typedef map<string,string>::const_iterator CI;
  for (CI i = snapshot.begin(); i != snapshot.end(); ++i)
    file << " " << i->first
         << " " << string2stream(i->second);
}


sid::component::status
ip4kcpu::restore_all_state (std::istream& file)
{
  map<string,string> snapshot;

  typedef map<string,string>::iterator It;
  while (!file.eof())
    {
      string x, y;
      file >> x;
      file >> stream2string(y);
      snapshot[x] = y;
    }

  return simwcpu::restore_state_map (snapshot);
}

string
ip4kcpu::get_regname (ip4kcpu::regno_t regno)
{
  switch (regno)
    {
    case ip4kcpu::d0:
      return "d0";
    case ip4kcpu::d1:
      return "d1";
    case ip4kcpu::d2:
      return "d2";
    case ip4kcpu::d3:
      return "d3";
    case ip4kcpu::d4:
      return "d4";
    case ip4kcpu::d5:
      return "d5";
    case ip4kcpu::d6:
      return "d6";
    case ip4kcpu::d7:
      return "d7";
    case ip4kcpu::d8:
      return "d8";
    case ip4kcpu::d9:
      return "d9";
    case ip4kcpu::d10:
      return "d10";
    case ip4kcpu::d11:
      return "d11";
    case ip4kcpu::d12:
      return "d12";
    case ip4kcpu::d13:
      return "d13";
    case ip4kcpu::d14:
      return "d14";
    case ip4kcpu::d15:
      return "d15";

#if 0
    case ip4kcpu::d16:
      return "d16";
    case ip4kcpu::d17:
      return "d17";
    case ip4kcpu::d18:
      return "d18";
    case ip4kcpu::d19:
      return "d19";
    case ip4kcpu::d20:
      return "d20";
    case ip4kcpu::d21:
      return "d21";
    case ip4kcpu::d22:
      return "d22";
    case ip4kcpu::d23:
      return "d23";
    case ip4kcpu::d24:
      return "d24";
    case ip4kcpu::d25:
      return "d25";
    case ip4kcpu::d26:
      return "d26";
    case ip4kcpu::d27:
      return "d27";
    case ip4kcpu::d28:
      return "d28";
    case ip4kcpu::d29:
      return "d29";
    case ip4kcpu::d30:
      return "d30";
    case ip4kcpu::d31:
      return "d31";
#endif
    case ip4kcpu::a0:
      return "a0";
    case ip4kcpu::a1:
      return "a1";
    case ip4kcpu::a2:
      return "a2";
    case ip4kcpu::a3:
      return "a3";
    case ip4kcpu::a4:
      return "a4";
    case ip4kcpu::a5:
      return "a5";
    case ip4kcpu::a6:
      return "a6";
    case ip4kcpu::sp:
      return "a7";
    case ip4kcpu::mac_hi:
      return "mac_hi";
    case ip4kcpu::mac_lo:
      return "mac_lo";
    case ip4kcpu::mac_rc16:
      return "mac_rc16";
    case ip4kcpu::source3:
      return "source3";
    case ip4kcpu::inst_cnt:
      return "inst_cnt";
    case ip4kcpu::csr:
      return "csr";
    case ip4kcpu::rosr:
      return "rosr";
    case ip4kcpu::iread_data:
      return "iread_data";
    case ip4kcpu::int_mask0:
      return "int_mask0";
    case ip4kcpu::int_mask1:
      return "int_mask1";
    case ip4kcpu::pc:
      return "pc";
    case ip4kcpu::scratchpad0:
      return "scratchpad0";
    case ip4kcpu::scratchpad1:
      return "scratchpad1";
    case ip4kcpu::scratchpad2:
      return "scratchpad2";
    case ip4kcpu::scratchpad3:
      return "scratchpad3";
    default:
      break;
    }

  return "";
}
