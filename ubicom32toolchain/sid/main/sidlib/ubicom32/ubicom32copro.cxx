// ip3kcopro.cxx - ip3k coprocessor sid encapsulation.  -*- C++ -*-

// Copyright (C) 2001, 2002 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include "config.h"

#include <ip3ksid.h>
#include <sidcpuutil.h>
#include <stdlib.h>

using namespace std;
using namespace sid;
using namespace sidlib;
using namespace sidutil;


// ----------------------------------------------------------------------------

ip3kcopro::ip3kcopro (const string& conf_file_name) :
  simwcpu (conf_file_name)
{
  // set the insn step count for the cpu to be 1
  this->copro = lookup_component ("cpu");
  assert (copro != 0);
  // use the internal-data-bus to do all the accesses
  this->copro_data_bus = copro->find_bus ("internal-data-bus");
  assert (copro_data_bus != 0);
  this->copro_insn_bus = copro->find_bus ("debugger-bus");
  assert (copro_insn_bus != 0);

  copro->set_attribute_value ("step-insn-count", "1");
}

sid::bus::status 
ip3kcopro::read_data_memory (sid::host_int_4 addr,
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
ip3kcopro::write_data_memory (sid::host_int_4 addr,
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
ip3kcopro::read_insn_memory (sid::host_int_4 addr,
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
ip3kcopro::write_insn_memory (sid::host_int_4 addr,
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
ip3kcopro::read_register (ip3kcopro::regno_t regno,
			  sid::host_int_8& value)
{
  this->check_load ();
  string regname = string("gdb-register-") + sidutil::make_numeric_attribute (regno);
  string reg_value = copro->attribute_value (regname);
  big_int_8 target_value = 0;
  if (reg_value.size() > 0)
    {
      unsigned int j = 0;
      for (unsigned int i = 8-reg_value.size(); i < 8; ++i, ++j)
	target_value.write_byte (i, reg_value[j]);
      value = target_value;
      return sid::component::ok;
    }
  return sid::component::not_found;
}

sid::component::status 
ip3kcopro::write_register (ip3kcopro::regno_t regno,
			   sid::host_int_8 value)
{
  this->check_load ();
  string regname = string("gdb-register-") + sidutil::make_numeric_attribute (regno);
  big_int_8 target_value = value;
  string attr;
  for (unsigned int i = 0; i < 8; ++i)
    attr += target_value.read_byte (i);
  return copro->set_attribute_value (regname, attr);
}

sid::component::status
ip3kcopro::interrupt (unsigned irq_num)
{
  return component::not_found;
}

void
ip3kcopro::checkpoint_all_state (std::ostream& file)
{
  map<string,string> snapshot = simwcpu::save_state_map ();

  typedef map<string,string>::const_iterator CI;
  for (CI i = snapshot.begin(); i != snapshot.end(); ++i)
    file << " " << i->first
         << " " << string2stream(i->second);
}


sid::component::status
ip3kcopro::restore_all_state (std::istream& file)
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
