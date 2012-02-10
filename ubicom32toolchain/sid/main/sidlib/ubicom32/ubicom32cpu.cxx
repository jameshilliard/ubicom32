// ubicom32cpu.cxx - ubicom32 main processor sid encapsulation.  -*- C++ -*-

// Copyright (C) 2001, 2002 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include "config.h"

#include <ubicom32sid.h>
#include <sidcpuutil.h>
#include <stdlib.h>

using namespace std;
using namespace sid;
using namespace sidlib;
using namespace sidutil;


// ----------------------------------------------------------------------------

ubicom32cpu::ubicom32cpu (const string& conf_file_name) :
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
ubicom32cpu::read_data_memory (sid::host_int_4 addr,
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
ubicom32cpu::write_data_memory (sid::host_int_4 addr,
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
ubicom32cpu::read_insn_memory (sid::host_int_4 addr,
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
ubicom32cpu::write_insn_memory (sid::host_int_4 addr,
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
ubicom32cpu::read_register (ubicom32cpu::regno_t regno,
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
ubicom32cpu::read_register (int context,
			ubicom32cpu::regno_t regno,
			sid::host_int_4& value)
{
  check_load ();
  string regname = string("c") + sidutil::make_numeric_attribute (context) + 
    string(":") + ubicom32cpu::get_regname (regno);
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
ubicom32cpu::write_register (ubicom32cpu::regno_t regno,
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
ubicom32cpu::write_register (int context,
			 ubicom32cpu::regno_t regno,
			 sid::host_int_4 value)
{
  check_load ();
  string regname = string("c") + sidutil::make_numeric_attribute (context) + 
    string(":") + ubicom32cpu::get_regname (regno);
  if (cpu->set_attribute_value (regname, sidutil::make_numeric_attribute (value)) 
                                == sid::component::not_found)
    return this->write_register (regno, value);
}

sid::component::status
ubicom32cpu::interrupt (unsigned irq_num)
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
ubicom32cpu::checkpoint_all_state (std::ostream& file)
{
  map<string,string> snapshot = simwcpu::save_state_map ();

  typedef map<string,string>::const_iterator CI;
  for (CI i = snapshot.begin(); i != snapshot.end(); ++i)
    file << " " << i->first
         << " " << string2stream(i->second);
}


sid::component::status
ubicom32cpu::restore_all_state (std::istream& file)
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
ubicom32cpu::get_regname (ubicom32cpu::regno_t regno)
{
  switch (regno)
    {
    case ubicom32cpu::d0:
      return "d0";
    case ubicom32cpu::d1:
      return "d1";
    case ubicom32cpu::d2:
      return "d2";
    case ubicom32cpu::d3:
      return "d3";
    case ubicom32cpu::d4:
      return "d4";
    case ubicom32cpu::d5:
      return "d5";
    case ubicom32cpu::d6:
      return "d6";
    case ubicom32cpu::d7:
      return "d7";
    case ubicom32cpu::d8:
      return "d8";
    case ubicom32cpu::d9:
      return "d9";
    case ubicom32cpu::d10:
      return "d10";
    case ubicom32cpu::d11:
      return "d11";
    case ubicom32cpu::d12:
      return "d12";
    case ubicom32cpu::d13:
      return "d13";
    case ubicom32cpu::d14:
      return "d14";
    case ubicom32cpu::d15:
      return "d15";

#if 0
    case ubicom32cpu::d16:
      return "d16";
    case ubicom32cpu::d17:
      return "d17";
    case ubicom32cpu::d18:
      return "d18";
    case ubicom32cpu::d19:
      return "d19";
    case ubicom32cpu::d20:
      return "d20";
    case ubicom32cpu::d21:
      return "d21";
    case ubicom32cpu::d22:
      return "d22";
    case ubicom32cpu::d23:
      return "d23";
    case ubicom32cpu::d24:
      return "d24";
    case ubicom32cpu::d25:
      return "d25";
    case ubicom32cpu::d26:
      return "d26";
    case ubicom32cpu::d27:
      return "d27";
    case ubicom32cpu::d28:
      return "d28";
    case ubicom32cpu::d29:
      return "d29";
    case ubicom32cpu::d30:
      return "d30";
    case ubicom32cpu::d31:
      return "d31";
#endif
    case ubicom32cpu::a0:
      return "a0";
    case ubicom32cpu::a1:
      return "a1";
    case ubicom32cpu::a2:
      return "a2";
    case ubicom32cpu::a3:
      return "a3";
    case ubicom32cpu::a4:
      return "a4";
    case ubicom32cpu::a5:
      return "a5";
    case ubicom32cpu::a6:
      return "a6";
    case ubicom32cpu::sp:
      return "a7";
    case ubicom32cpu::mac_hi:
      return "mac_hi";
    case ubicom32cpu::mac_lo:
      return "mac_lo";
    case ubicom32cpu::mac_rc16:
      return "mac_rc16";
    case ubicom32cpu::source3:
      return "source3";
    case ubicom32cpu::inst_cnt:
      return "inst_cnt";
    case ubicom32cpu::csr:
      return "csr";
    case ubicom32cpu::rosr:
      return "rosr";
    case ubicom32cpu::iread_data:
      return "iread_data";
    case ubicom32cpu::int_mask0:
      return "int_mask0";
    case ubicom32cpu::int_mask1:
      return "int_mask1";
    case ubicom32cpu::pc:
      return "pc";
    case ubicom32cpu::scratchpad0:
      return "scratchpad0";
    case ubicom32cpu::scratchpad1:
      return "scratchpad1";
    case ubicom32cpu::scratchpad2:
      return "scratchpad2";
    case ubicom32cpu::scratchpad3:
      return "scratchpad3";
    case ubicom32cpu::acc1_hi:
      return "acc1_hi";
    case ubicom32cpu::acc1_lo:
      return "acc1_lo";
    default:
      break;
    }

  return "";
}
