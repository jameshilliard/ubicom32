// sidlib.h - Define the external interface of the SID encapsulation
// library.

// Copyright (C) 2001, 2002 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.
  
#ifndef SIDLIB_H
#define SIDLIB_H 
  
#include <stdlib.h>
#include <sidtypes.h>
#include <sidcomp.h>
  
#include <string> 
#include <vector>
#include <map>

using namespace std;
using namespace sid;
  
namespace sidlib
{ 
  class cfgroot_interface 
  {
  public:

    // Default constructor creates blank sid system containing a cfgroot
    // component
    cfgroot_interface ();
    
    ~cfgroot_interface ();
    
    // add one configuration command line
    void add_configure_line (const string& line);
    
    // add an entire file of configuration commands
    void add_configure_file (const string& file_name);
    
    // query configuration to get list of all component names instantiated
    vector<string> list_component_names ();
    
    // look up component by name and return a pointer to it's instance
    sid::component *lookup_component (const string& component_name);
    
    // advance one execution iteration
    bool advance ();
    
    // make sure all components including memory are initialized
    void check_load ()
      {
	check_init ();
	if (!load_done)
	  {
	    load_done = true;
	    step_pin->driven (1);
	  }
      }

    void check_init  ()
      {
	if (!init_done)
	  {
	    init_done = true;
	    step_pin->driven (1);
	  }
      }

  protected:

    // complete initialization of components
    bool load_done;
    void complete_init ();

  private:
    
    sid::component *root; // pointer to actual cfgroot component
    sid::component *catalog; // pointer to component to catalog all components
    sid::pin *step_pin;   // pointer to cfgroot step! pin
    bool init_done;

  }; // class cfgroot_interface
  
  class simwcpu: public cfgroot_interface
    {
    public:
      
      simwcpu (const string& conf_file_name);
      
      bool step (unsigned max_iterations);
      bool step () { return step (1); }
      
      void reset ();

      sid::component::status get_attribute (const string& component,
					    const string& attribute,
					    string& value);

      sid::component::status set_attribute (const string& component,
					    const string& attribute,
					    const string& value);
      
      bool load (const string& loader_name, const string& object_file_name);
      
      template <typename Dtype>  
	sid::bus::status
	read_any (const string& component,
		  const string& accessor,
		  sid::host_int_4 address,
		  Dtype& value)
	{
	  sid::component *comp = this->lookup_component (component);
	  if (!comp)
	    return sid::bus::unpermitted;
	  sid::bus *acc = comp->find_bus (accessor);
	  if (!acc)
	    return sid::bus::unpermitted;
	  this->check_load ();
	  return acc->read (address, value);
	}
      
      template <typename Dtype>  
	sid::bus::status
	write_any (const string& component,
		   const string& accessor,
		   sid::host_int_4 address,
		   const Dtype& value)
	{
	  sid::component *comp = this->lookup_component (component);
	  if (!comp)
	    return sid::bus::unpermitted;
	  sid::bus *acc = comp->find_bus (accessor);
	  if (!acc)
	    return sid::bus::unpermitted;
	  this->check_load ();
	  return acc->write (address, value);
	}
      
      // Some macros to make manufacturing of the cartesian-product
      // calls simpler.
#define SIMWCPU_WRITE(dtype) \
      sid::bus::status write_memory (const string& component, \
                                     const string& accessor,  \
                                     sid::host_int_4 addr,    \
                                     const dtype& data) throw ()     \
        { return write_any (component, accessor, addr, data); }
      
#define SIMWCPU_READ(dtype) \
      sid::bus::status read_memory (const string& component, \
                                    const string& accessor,  \
                                    sid::host_int_4 addr,    \
                                    dtype& data) throw ()    \
        { return read_any (component, accessor, addr, data); }

#define SIMWCPU_WRITE_BUS(dtype) \
      sid::bus::status write_memory_bus (sid::bus *acc, \
                                     sid::host_int_4 addr,    \
                                     const dtype& data) throw ()     \
        { this->check_load (); return acc->write (addr, data); }
      
#define SIMWCPU_READ_BUS(dtype) \
      sid::bus::status read_memory_bus (sid::bus *acc, \
                                    sid::host_int_4 addr,    \
                                    dtype& data) throw ()    \
        { this->check_load(); return acc->read (addr, data); }
      
      // memory read/write routines
      SIMWCPU_READ(sid::big_int_1);
      SIMWCPU_READ(sid::little_int_1);
      SIMWCPU_READ(sid::big_int_2);
      SIMWCPU_READ(sid::little_int_2);
      SIMWCPU_READ(sid::big_int_4);
      SIMWCPU_READ(sid::little_int_4);
      SIMWCPU_READ(sid::big_int_8);
      SIMWCPU_READ(sid::little_int_8);
      SIMWCPU_READ_BUS(sid::big_int_1);
      SIMWCPU_READ_BUS(sid::little_int_1);
      SIMWCPU_READ_BUS(sid::big_int_2);
      SIMWCPU_READ_BUS(sid::little_int_2);
      SIMWCPU_READ_BUS(sid::big_int_4);
      SIMWCPU_READ_BUS(sid::little_int_4);
      SIMWCPU_READ_BUS(sid::big_int_8);
      SIMWCPU_READ_BUS(sid::little_int_8);
      
      SIMWCPU_WRITE(sid::big_int_1);
      SIMWCPU_WRITE(sid::little_int_1);
      SIMWCPU_WRITE(sid::big_int_2);
      SIMWCPU_WRITE(sid::little_int_2);
      SIMWCPU_WRITE(sid::big_int_4);
      SIMWCPU_WRITE(sid::little_int_4);
      SIMWCPU_WRITE(sid::big_int_8);
      SIMWCPU_WRITE(sid::little_int_8);
      SIMWCPU_WRITE_BUS(sid::big_int_1);
      SIMWCPU_WRITE_BUS(sid::little_int_1);
      SIMWCPU_WRITE_BUS(sid::big_int_2);
      SIMWCPU_WRITE_BUS(sid::little_int_2);
      SIMWCPU_WRITE_BUS(sid::big_int_4);
      SIMWCPU_WRITE_BUS(sid::little_int_4);
      SIMWCPU_WRITE_BUS(sid::big_int_8);
      SIMWCPU_WRITE_BUS(sid::little_int_8);
     
#undef SIMWCPU_READ
#undef SIMWCPU_WRITE
      
      // cpu register read/write routines - uses gdb register numbers
      
      sid::component::status read_register (const string& component,
                                            unsigned regno,
                                            string& value);
      
      sid::component::status write_register (const string& component,
                                             unsigned regno,
                                             const string& value);
      
      // drive a specified pin
      sid::component::status drive_pin (const string& component,
                                        const string& pin_name,
                                        sid::host_int_4 value);

      // create/restore snapshot of components
      map<string,string> save_state_map ();
      sid::component::status restore_state_map (const map<string,string>& s);

      // get exit code of application
      sid::component::status get_exit_code (sid::host_int_4& x);

    private:

      sid::pin *reset_pin;
      sid::component *target_sched;
      sid::component *gloss_comp;

    }; // class simwcpu
  
} // namespace sidlib
      
#endif // SIDLIB_H
