// cfgroot_interface.cxx - interface to cfgroot component.  -*- C++ -*-

// Copyright (C) 2001 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include "config.h"

#include <stdio.h>
#include <sidso.h>
#include <stdlib.h>
#include <sidlib.h>
#include <sidcpuutil.h>

using namespace std;
using namespace sid;
using namespace sidlib;
using namespace sidutil;


// ----------------------------------------------------------------------------
// This component reads a flat config file, and acts as a root
// container for the simulation.

class catalog_component: public virtual component,
			 protected no_pin_component,
			 protected no_attribute_component,
			 protected no_bus_component,
			 protected no_accessor_component,
			 protected no_relation_component,
			 protected recursion_limited
{
public:
  catalog_component();
	~catalog_component() throw() {}
  vector<string> list_component_names ();
  sid::component *lookup_component (const string& component_name);

  sid::component::status relate (const string& relation, component *comp) throw();
  sid::component::status unrelate (const string& relation, component *comp) throw();
  vector<string> relationship_names () throw();
  vector<sid::component *> related_components (const string& name) throw();
  
private:

  map<string,sid::component *> catalog;

};

catalog_component::catalog_component() :
  recursion_limited ("catalog", 1)
{
}

sid::component::status
catalog_component::relate (const string& relation,
			   component *comp) throw()
{
  string comp_name = relation.substr (relation.find(' ') + 1);
  if (catalog.find(comp_name) != catalog.end())
    {
      return sid::component::bad_value;
    }
  catalog[comp_name] = comp;
  return sid::component::ok;
}
  
sid::component::status
catalog_component::unrelate (const string& relation,
			     component *comp) throw()
{
  string comp_name = relation.substr(relation.find(' ') + 1);
  if (catalog.erase(comp_name) == 0)
    return sid::component::bad_value;
  return sid::component::ok;
}

std::vector<string>
catalog_component::relationship_names () throw()
{
  std::vector<string> names;
  for (map<string,sid::component *>::const_iterator it =
	 this->catalog.begin();
       it != this->catalog.end();
       it++)
    {
      names.push_back(it->first);
    }
  return names;
}

std::vector<sid::component *>
catalog_component::related_components (const string& name) throw()
{
  std::vector<sid::component *> comp;
  if (catalog.find(name) != catalog.end())
    comp.push_back (catalog[name]);
  return comp;
}

// Link to component libraries directly.
extern component_library /* *not* DLLIMPORT */ config_component_library;

#if 0
extern component_library /* *not* DLLIMPORT */ audio_component_library;
extern component_library /* *not* DLLIMPORT */ cache_component_library;
extern component_library /* *not* DLLIMPORT */ cgen_component_library;
extern component_library /* *not* DLLIMPORT */ console_component_library;
extern component_library /* *not* DLLIMPORT */ gdb_component_library;
extern component_library /* *not* DLLIMPORT */ gloss_component_library;
extern component_library /* *not* DLLIMPORT */ glue_component_library;
extern component_library /* *not* DLLIMPORT */ ide_component_library;
extern component_library /* *not* DLLIMPORT */ interrupt_component_library;
extern component_library /* *not* DLLIMPORT */ hd44780u_component_library;
extern component_library /* *not* DLLIMPORT */ lcd_char_display_component_library;
extern component_library /* *not* DLLIMPORT */ t6963c_component_library;
extern component_library /* *not* DLLIMPORT */ loader_component_library;
extern component_library /* *not* DLLIMPORT */ mbox_component_library;
extern component_library /* *not* DLLIMPORT */ mapper_component_library;
extern component_library /* *not* DLLIMPORT */ mem_component_library;
extern component_library /* *not* DLLIMPORT */ mmu_component_library;
extern component_library /* *not* DLLIMPORT */ parport_component_library;
extern component_library /* *not* DLLIMPORT */ prof_component_library;
extern component_library /* *not* DLLIMPORT */ rtc_component_library;
extern component_library /* *not* DLLIMPORT */ sched_component_library;
extern component_library /* *not* DLLIMPORT */ timer_component_library;
extern component_library /* *not* DLLIMPORT */ uart_component_library;

component_library* audio = &audio_component_library;
component_library* cache = & cache_component_library;
component_library* cgen = &cgen_component_library;
component_library* console = &console_component_library;
component_library* gdb = &gdb_component_library;
component_library* gloss = &gloss_component_library;
component_library* glue = &glue_component_library;
component_library* ide = &ide_component_library;
component_library* interrupt = &interrupt_component_library;
component_library* hd44780u = &hd44780u_component_library;
component_library* lcd = &lcd_char_display_component_library;
component_library* t6963c = &t6963c_component_library;
component_library* loader = &loader_component_library;
component_library* mbox = &mbox_component_library;
component_library* mapper = &mapper_component_library;
component_library* mem = &mem_component_library;
component_library* mmu = &mmu_component_library;
component_library* parport = &parport_component_library;
component_library* prof = &prof_component_library;
component_library* rtc = &rtc_component_library;
component_library* sched = &sched_component_library;
component_library* timer = &timer_component_library;
component_library* uart = &uart_component_library;
#endif

//component_library* cfglib = &config_component_library;

cfgroot_interface::cfgroot_interface () :
  load_done (false),
  init_done (false)
{
  root = config_component_library.create_component("sid-control-cfgroot-libsid");
  if (!root)
    {
      cerr << "Cannot instantiate configuration root component." << endl;
      abort ();
    }
  step_pin = root->find_pin ("step!");
  assert (step_pin != NULL);
  catalog = new catalog_component();
  if (!catalog)
    {
      cerr << "Cannot instantiate catalog component." << endl;
      abort ();
    }
  // put catalog component on cfgroot informee list
  root->relate ("component-catalog-informees", catalog);
}

cfgroot_interface::~cfgroot_interface ()
{
  config_component_library.delete_component(this->root);
}

void
cfgroot_interface::add_configure_file (const string& file_name)
{
  root->set_attribute_value ("config-file!", file_name);
}

void
cfgroot_interface::add_configure_line (const string& file_name)
{
  root->set_attribute_value ("config-line!", file_name);
}

vector<string>
cfgroot_interface::list_component_names ()
{
  check_init ();
  return catalog->relationship_names ();
}

sid::component *
cfgroot_interface::lookup_component (const string& name)
{
  check_init ();
  vector<sid::component *> k = catalog->related_components (name);
  if (k.size() == 0)
    return NULL;
  return k.front();
}

bool
cfgroot_interface::advance ()
{
  check_load ();
  step_pin->driven (1);
  string str = root->attribute_value ("running");
  return str != "no";
}

