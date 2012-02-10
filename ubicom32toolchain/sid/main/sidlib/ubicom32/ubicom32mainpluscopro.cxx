// ip3kmainpluscopro.cxx - ip3k main plus coprocessor sid encapsulation.  -*- C++ -*-

// Copyright (C) 2001, 2002 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include "config.h"

#include <ip3ksid.h>
#include <sidcpuutil.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
using namespace sid;
using namespace sidlib;
using namespace sidutil;


// ----------------------------------------------------------------------------

ip3k_main_plus_copro::ip3k_main_plus_copro (const string& conf_file_name) :
  simwcpu (conf_file_name)
{
  // set the insn step count for the cpu to be 1
  this->cpu = lookup_component ("cpu");
  assert (cpu != 0);
  this->copro = lookup_component ("coprocessor");
  assert (copro != 0);
  this->cpu_data_bus = cpu->find_bus ("data-bus");
  assert (cpu_data_bus != 0);
  this->cpu_insn_bus = cpu->find_bus ("debugger-bus");
  assert (cpu_insn_bus != 0);
  this->copro_data_bus = copro->find_bus ("internal-data-bus");
  assert (copro_data_bus != 0);
  this->copro_insn_bus = copro->find_bus ("debugger-bus");
  assert (copro_insn_bus != 0);
  this->timer = lookup_component("ip3ktimer");
  assert(timer !=0);
  
  //Set the pins to be NULL;
  this->mpt_pin = NULL;
  this->sys_pin = NULL;
  this->execute_pin_main = this->cpu->find_pin("mercury-execute");
  assert(execute_pin_main !=0);
  this->execute_pin_copro = this->copro->find_pin("mercury-execute");
  assert(execute_pin_copro !=0);
  this->scheduler_pin = this->cpu->find_pin("mercury-scheduler");
  assert(scheduler_pin !=0);
  cpu->set_attribute_value ("step-insn-count", "1");
  copro->set_attribute_value ("step-insn-count", "1");
}

sid::bus::status 
ip3k_main_plus_copro::read_data_memory_main (sid::host_int_4 addr,
					     unsigned int length,
					     sid::host_int_1 *buf)
{
  big_int_1 k;
  for (unsigned int i = 0; i < length; ++i)
    {
      sid::bus::status s = this->read_data_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      buf[i] = k;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_data_memory_main (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_1 *buf)
{
  big_int_1 k;
  for (unsigned int i = 0; i < length; ++i)
    {
      k = buf[i];
      sid::bus::status s = this->write_data_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_insn_memory_main (sid::host_int_4 addr,
					     unsigned int length,
					     sid::host_int_1 *buf)
{
  big_int_1 k;
  for (unsigned int i = 0; i < length; ++i)
    {
      sid::bus::status s = this->read_insn_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      buf[i] = k;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_insn_memory_main (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_1 *buf)
{
  big_int_1 k;

  for (unsigned int i = 0; i < length; ++i)
    {
      k = buf[i];
      sid::bus::status s = this->write_insn_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_data_memory_copro (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_1 *buf)
{
  big_int_1 k;
  for (unsigned int i = 0; i < length; ++i)
    {
      sid::bus::status s = this->read_data_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      buf[i] = k;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_data_memory_copro (sid::host_int_4 addr,
					       unsigned int length,
					       sid::host_int_1 *buf)
{
  big_int_1 k;
  for (unsigned int i = 0; i < length; ++i)
    {
      k = buf[i];
      sid::bus::status s = this->write_data_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_insn_memory_copro (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_1 *buf)
{
  big_int_1 k;
  for (unsigned int i = 0; i < length; ++i)
    {
      sid::bus::status s = this->read_insn_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      buf[i] = k;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_insn_memory_copro (sid::host_int_4 addr,
					       unsigned int length,
					       sid::host_int_1 *buf)
{
  big_int_1 k;
  for (unsigned int i = 0; i < length; ++i)
    {
      k = buf[i];
      sid::bus::status s = this->write_insn_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      ++addr;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_data_memory_main (sid::host_int_4 addr,
					     unsigned int length,
					     sid::host_int_4 *buf)
{
  big_int_4 k;
  big_int_4 * dest = reinterpret_cast<big_int_4 *> (buf);
  for (unsigned int i = 0; i < length; i+= 4)
    {
      sid::bus::status s = this->read_data_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      *dest++ = k;
      addr+= 4;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_data_memory_main (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_4 *buf)
{
  big_int_4 k;
  big_int_4 * dest = reinterpret_cast<big_int_4 *> (buf);
  for (unsigned int i = 0; i < length; i+= 4)
    {
      k = *dest++;
      sid::bus::status s = this->write_data_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      addr+= 4;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_insn_memory_main (sid::host_int_4 addr,
					     unsigned int length,
					     sid::host_int_4 *buf)
{
  big_int_4 k;
  big_int_4 * dest = reinterpret_cast<big_int_4 *> (buf);
  for (unsigned int i = 0; i < length; i+= 4)
    {
      sid::bus::status s = this->read_insn_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      *dest++ = k;
      addr+= 4;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_insn_memory_main (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_4 *buf)
{
  big_int_4 k;
  big_int_4 * dest = reinterpret_cast<big_int_4 *> (buf);

  for (unsigned int i = 0; i < length; i+= 4)
    {
      k = *dest++;
      sid::bus::status s = this->write_insn_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      addr+= 4;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_data_memory_copro (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_4 *buf)
{
  big_int_4 k;
  big_int_4 * dest = reinterpret_cast<big_int_4 *> (buf);
  for (unsigned int i = 0; i < length; i+= 4)
    {
      sid::bus::status s = this->read_data_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      *dest++ = k;
      addr+= 4;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_data_memory_copro (sid::host_int_4 addr,
					       unsigned int length,
					       sid::host_int_4 *buf)
{
  big_int_4 k;
  big_int_4 * dest = reinterpret_cast<big_int_4 *> (buf);
  for (unsigned int i = 0; i < length; i+= 4)
    {
      k = *dest++;
      sid::bus::status s = this->write_data_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      addr+= 4;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_insn_memory_copro (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_4 *buf)
{
  big_int_4 k;
  big_int_4 * dest = reinterpret_cast<big_int_4 *> (buf);
  for (unsigned int i = 0; i < length; i+= 4)
    {
      sid::bus::status s = this->read_insn_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      *dest++ = k;
      addr+= 4;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_insn_memory_copro (sid::host_int_4 addr,
					       unsigned int length,
					       sid::host_int_4 *buf)
{
  big_int_4 k;
  big_int_4 * dest = reinterpret_cast<big_int_4 *> (buf);
  for (unsigned int i = 0; i < length; i+= 4)
    {
      k = *dest++;
      sid::bus::status s = this->write_insn_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      addr+= 4;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_data_memory_main (sid::host_int_4 addr,
					     unsigned int length,
					     sid::host_int_8 *buf)
{
  big_int_8 k;
  big_int_8 * dest = reinterpret_cast<big_int_8 *> (buf);
  for (unsigned int i = 0; i < length; i+= 8)
    {
      sid::bus::status s = this->read_data_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      *dest++ = k;
      addr+= 8;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_data_memory_main (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_8 *buf)
{
  big_int_8 k;
  big_int_8 * dest = reinterpret_cast<big_int_8 *> (buf);
  for (unsigned int i = 0; i < length; i+= 8)
    {
      k = *dest++;
      sid::bus::status s = this->write_data_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      addr+= 8;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_insn_memory_main (sid::host_int_4 addr,
					     unsigned int length,
					     sid::host_int_8 *buf)
{
  big_int_8 k;
  big_int_8 * dest = reinterpret_cast<big_int_8 *> (buf);
  for (unsigned int i = 0; i < length; i+= 8)
    {
      sid::bus::status s = this->read_insn_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      *dest++ = k;
      addr+= 8;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_insn_memory_main (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_8 *buf)
{
  big_int_8 k;
  big_int_8 * dest = reinterpret_cast<big_int_8 *> (buf);

  for (unsigned int i = 0; i < length; i+= 8)
    {
      k = *dest++;
      sid::bus::status s = this->write_insn_memory_main (addr, k);
      if (s != sid::bus::ok)
	return s;
      addr+= 8;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_data_memory_copro (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_8 *buf)
{
  big_int_8 k;
  big_int_8 * dest = reinterpret_cast<big_int_8 *> (buf);
  for (unsigned int i = 0; i < length; i+= 8)
    {
      sid::bus::status s = this->read_data_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      *dest++ = k;
      addr+= 8;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_data_memory_copro (sid::host_int_4 addr,
					       unsigned int length,
					       sid::host_int_8 *buf)
{
  big_int_8 k;
  big_int_8 * dest = reinterpret_cast<big_int_8 *> (buf);
  for (unsigned int i = 0; i < length; i+= 8)
    {
      k = *dest++;
      sid::bus::status s = this->write_data_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      addr+= 8;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::read_insn_memory_copro (sid::host_int_4 addr,
					      unsigned int length,
					      sid::host_int_8 *buf)
{
  big_int_8 k;
  big_int_8 * dest = reinterpret_cast<big_int_8 *> (buf);
  for (unsigned int i = 0; i < length; i+= 8)
    {
      sid::bus::status s = this->read_insn_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      *dest++ = k;
      addr+= 8;
    }

  return sid::bus::ok;
}

sid::bus::status 
ip3k_main_plus_copro::write_insn_memory_copro (sid::host_int_4 addr,
					       unsigned int length,
					       sid::host_int_8 *buf)
{
  big_int_8 k;
  big_int_8 * dest = reinterpret_cast<big_int_8 *> (buf);
  for (unsigned int i = 0; i < length; i+= 8)
    {
      k = *dest++;
      sid::bus::status s = this->write_insn_memory_copro (addr, k);
      if (s != sid::bus::ok)
	return s;
      addr+= 8;
    }

  return sid::bus::ok;
}

sid::component::status 
ip3k_main_plus_copro::read_register_main (ip3kcpu::regno_t regno,
					  sid::host_int_4& value)
{
  this->check_load ();
  string regname = string("gdb-register-") + sidutil::make_numeric_attribute (regno);
  string reg_value = cpu->attribute_value (regname);
   big_int_4 target_value;
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
ip3k_main_plus_copro::read_register_main (int context,
					  ip3kcpu::regno_t regno,
					  sid::host_int_4& value)
{
  this->check_load ();
  string regname = string("c") + sidutil::make_numeric_attribute (context) + 
    string(":") + ip3kcpu::get_regname (regno);
  string reg_value = cpu->attribute_value (regname);
  if (reg_value.size() > 0)
    {
      value = strtoul (reg_value.c_str(), NULL, 10);
      return sid::component::ok;
    }
  /* otherwise assume it is global register and try again */
  return this->read_register_main (regno, value);
}

sid::component::status 
ip3k_main_plus_copro::write_register_main (ip3kcpu::regno_t regno,
					   sid::host_int_4 value)
{
  this->check_load ();
  string regname;
  if((regno == 36) || (regno == 37))
    {
      regname = string("int_stat") + sidutil::make_numeric_attribute(regno-36);
      return cpu->set_attribute_value (regname, sidutil::make_numeric_attribute (value));
    }
  else
    regname = string("gdb-register-") + sidutil::make_numeric_attribute (regno);

  big_int_4 target_value = value;
  string attr;
  for (unsigned int i = 0; i < 4; ++i)
    attr += target_value.read_byte (i);
  return cpu->set_attribute_value (regname, attr);
}

sid::component::status 
ip3k_main_plus_copro::write_register_main (int context,
					   ip3kcpu::regno_t regno,
					   sid::host_int_4 value)
{
  this->check_load ();
  string regname = string("c") + sidutil::make_numeric_attribute (context) + 
    string(":") + ip3kcpu::get_regname (regno);
  if (cpu->set_attribute_value (regname, sidutil::make_numeric_attribute (value)) 
                                == sid::component::not_found)
    return this->write_register_main (regno, value);
}

sid::component::status 
ip3k_main_plus_copro::read_register_copro (ip3kcopro::regno_t regno,
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
ip3k_main_plus_copro::write_register_copro (ip3kcopro::regno_t regno,
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
ip3k_main_plus_copro::interrupt_main (unsigned irq_num)
{
  if (irq_num < 0 || irq_num > 31)
    return component::not_found;

  string pin_name = string("irq_") + sidutil::make_numeric_attribute (irq_num);
  sid::pin *irq_pin = cpu->find_pin (pin_name);

  if (!irq_pin)
    return component::not_found;

  this->check_load ();
  irq_pin->driven (1);
  return component::ok;
}

void
ip3k_main_plus_copro::checkpoint_all_state (std::ostream& file)
{
  map<string,string> snapshot = simwcpu::save_state_map ();

  typedef map<string,string>::const_iterator CI;
  for (CI i = snapshot.begin(); i != snapshot.end(); ++i)
    file << " " << i->first
         << " " << string2stream(i->second);
}


sid::component::status
ip3k_main_plus_copro::restore_all_state (std::istream& file)
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

char * timerRegNames[] = {
  "mptval",
  "rsgcfg",
  "rtcom",
  "tkey",
  "wdcom",
  "wdcfg",
  "sysval",
  "cbcfg1",
  "cbcfg2",
  "rstflag",
  "trng",
  "brown",
  "syscom1",
  "syscom2",
  "syscom3",
  "syscom4",
  "syscom5",
  "syscom6",
  "syscom7",
  "syscom8",
  0
};

sid::component::status
ip3k_main_plus_copro::read_all_timer_registers(host_int_4 *buf)
{
  string sys_value;
  if(timer == NULL)
    return sid::component::not_found;
  
  for(int i=0; ; i++)
    {
      if(timerRegNames[i] == 0)
	break;

      sys_value= timer->attribute_value (string(timerRegNames[i]));
      if (sys_value.size() > 0)
	{
	  *buf++ = strtoul (sys_value.c_str(), NULL, 10);
	}
      else
	return sid::component::not_found;
    }
  return sid::component::ok;
}

sid::component::status
ip3k_main_plus_copro::write_all_timer_registers(host_int_4 *buf)
{
  for(int i=0; ; i++)
    {
      if(timerRegNames[i] == 0)
	break;

      // write it out
      if(timer->set_attribute_value(string(timerRegNames[i]), 
				    sidutil::make_numeric_attribute(*buf++)) == sid::component::not_found)
	return sid::component::not_found;
    }

  return sid::component::ok;
}

sid::component::status
ip3k_main_plus_copro::tick_mpt_clock(void)
{
  // Trying to kick the mpt clock. If it is the first time we have to hijack it
  if(this->mpt_pin == NULL)
    {
      // need to hijack the pin
      sid::component *scheduler_component = lookup_component ("target-sched");

      //Sanity check
      if( !scheduler_component )
	{
	  return sid::component::not_found;
	}

	//disable the mpt pin clock
	scheduler_component->set_attribute_value ("2-time","0" );

	// now find the pin
	this->mpt_pin = timer->find_pin("clockMptVal");
	if(!mpt_pin)
	  return sid::component::not_found;
    }

  // kick the pin
  this->mpt_pin->driven(1);
  return sid::component::ok;
}

sid::component::status
ip3k_main_plus_copro::tick_sys_clock(void)
{
  // Trying to kick the sys clock. If it is the first time we have to hijack it
  if(this->sys_pin == NULL)
    {
      // need to hijack the pin
      sid::component *scheduler_component = lookup_component ("target-sched");

      //Sanity check
      if( !scheduler_component )
	{
	  return sid::component::not_found;
	}

	//disable the sys pin clock
	scheduler_component->set_attribute_value ("3-time","0" );

	// now find the pin
	this->sys_pin = timer->find_pin("clockSysVal");
	if(!sys_pin)
	  return sid::component::not_found;
    }

  // kick the pin
  this->sys_pin->driven(1);
  return sid::component::ok;
}

int
ip3k_main_plus_copro::schedule()
{
  // kick the pin
  this->scheduler_pin->driven(1);

  // go retrieve the context number
  string reg_value = cpu->attribute_value (string("mpcontext"));
  if (reg_value.size() > 0)
    {
      int value = strtoul (reg_value.c_str(), NULL, 10);
      return value;
    }
  abort();
  return 0xfffffff0;
}

void
ip3k_main_plus_copro::execute_main()
{
  // kick the pin
  this->execute_pin_main->driven(1);
}

void
ip3k_main_plus_copro::execute_copro()
{
  // kick the pin
  this->execute_pin_copro->driven(1);
}
