// simwcpu.cxx - simulator with at least one CPU component.  -*- C++ -*-

// Copyright (C) 2001 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sidlib.h>
#include <sidcpuutil.h>

using namespace std;
using namespace sid;
using namespace sidlib;
using namespace sidutil;


// ----------------------------------------------------------------------------

simwcpu::simwcpu (const string& conf_file_name)
{
  // open and read the configuration file
  this->add_configure_file (conf_file_name);

  // verify that there is a target scheduler
  this->target_sched = lookup_component ("target-sched");
  assert (target_sched != 0);

  // verify that there is a reset net
  component* reset_net = lookup_component ("hw-reset-net");
  assert (reset_net != 0);
  this->reset_pin = reset_net->find_pin ("input");
  assert (reset_pin != 0); 
  
  // verify we can set step cycle limit of target scheduler to 1
  assert (target_sched->set_attribute_value ("step-cycle-limit", "1")
          == component::ok);
}

bool 
simwcpu::step (unsigned max_iterations)
{
  for (unsigned int i = 0; i < max_iterations; ++i)
    {
      if (!advance())
        return false;
    }
  
  return true;
}

void 
simwcpu::reset ()
{
  check_load ();
  reset_pin->driven (1);
}

bool 
simwcpu::load (const string& loader_name,
           const string& file_name)
{
  component *loader = this->lookup_component (loader_name);
  if (!loader)
    return false;

  if (loader->set_attribute_value ("file", file_name) !=
      component::ok)
    return false;

  return true;
}

component::status
simwcpu::read_register (const string& component,
			unsigned regno,
			string& value)
{
  sid::component* cpu = this->lookup_component (component);
  if (!cpu)
    return component::not_found;

  check_load ();
  value = cpu->attribute_value (string("gdb-register-") + sidutil::make_numeric_attribute(regno));
  return (value == "") ? component::not_found : component::ok;
}


component::status
simwcpu::write_register (const string& component,
			 unsigned regno,
			 const string& value)
{
  sid::component* cpu = this->lookup_component (component);
  if (!cpu)
    return component::not_found;

  check_load ();
  return cpu->set_attribute_value (string("gdb-register-") + 
                                   sidutil::make_numeric_attribute(regno), value);
}

component::status 
simwcpu::get_attribute (const string& component,
			const string& attribute,
			string& value)
{
  sid::component* comp = this->lookup_component (component);
  if (comp == NULL)
    return component::not_found;

  check_load ();
  value = comp->attribute_value (attribute);
  return (value == "") ? component::not_found : component::ok;
}

component::status 
simwcpu::set_attribute (const string& component,
			const string& attribute,
			const string& value)
{
  sid::component* comp = this->lookup_component (component);
  if (comp == NULL)
    return component::not_found;

  check_load ();
  return comp->set_attribute_value (attribute, value);
}

component::status
simwcpu::drive_pin (const string &component_name,
		    const string& pin_name,
                    host_int_4 value)
{
  component *comp = this->lookup_component (component_name);
  if (!comp)
    return component::not_found;

  pin *p = comp->find_pin (pin_name);

  if (!p)
    return component::not_found;

  check_load ();
  p->driven (value);

  return component::ok;
}

map<string,string>
simwcpu::save_state_map ()
{
  vector<string> k;
  map<string,string> snapshot;

  // get list of component names and for each component, get a state-snapshot
  k = this->list_component_names ();

  check_load ();

  for (int i = 0; i < k.size(); ++i)
    {
      component *comp = this->lookup_component (k[i]);
      snapshot[k[i]] = comp->attribute_value ("state-snapshot");
    }

  return snapshot;
}

component::status
simwcpu::restore_state_map (const map<string,string>& state)
{
  vector<string> k;
  component::status status = component::ok;

  // get list of component names and for each component, set its state-snapshot
  k = this->list_component_names ();

  check_load ();

  for (int i = 0; i < k.size(); ++i)
    {
      string compname = k[i]; 
      component *comp = this->lookup_component (compname);

      if (!((map<string,string>&)state)[compname].empty())
	{
	  status = comp->set_attribute_value ("state-snapshot", 
					      ((map<string,string>&)state)[compname]);
	}

      if (status != component::ok)
	{
	  cerr << "simwcpu: Error restoring " << compname << endl;
	  return status;
	}
    }

  return component::ok;
}
  
component::status
simwcpu::get_exit_code (sid::host_int_4& exit_code)
{
  check_load ();
  string exit_string;
  if (gloss_comp == NULL)
    this->gloss_comp = this->lookup_component ("gloss");
    
  if (!gloss_comp)
    return component::not_found;
     
  exit_string = gloss_comp->attribute_value ("exit-code");
  if (exit_string == "") 
    return component::not_found;

  exit_code = atoi (exit_string.c_str());

  return component::ok;
}






                             
