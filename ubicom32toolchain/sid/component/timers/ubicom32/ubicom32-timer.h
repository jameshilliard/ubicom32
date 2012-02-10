// ubicom32-timer.h - An implementation of the timers for the Ubicom32
// family of boards.  -*- C++ -*-

// Copyright (C) 1999, 2000 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.
// ubicom32-timer.h

#ifndef _UBICOM32_TIMER_H
#define _UBICOM32_TIMER_H

#include <sidcomp.h>
#include <sidso.h>
#include <sidcomputil.h>
#include <sidattrutil.h>
#include <sidpinutil.h>
#include <sidpinattrutil.h>
#include <sidbusutil.h>
#include <sidschedutil.h>
#include <sidtypes.h>
#include <sidwatchutil.h>

#include <string>
#include <iostream>

using std::vector;
using std::string;
using std::ostream;
using std::istream;
using std::ios;
using std::cerr;
using std::endl;

using sid::component;
using sid::bus;
using sid::big_int_4;
using sid::big_int_2;
using sid::host_int_2;
using sid::host_int_4;
using sid::component_library;
using sid::COMPONENT_LIBRARY_MAGIC;

using sidutil::fixed_attribute_map_component;
using sidutil::fixed_bus_map_component;
using sidutil::fixed_pin_map_component;
using sidutil::no_accessor_component;
using sidutil::no_relation_component;
using sidutil::callback_pin;
using sidutil::output_pin;
using sidutil::word_bus;
using sidutil::scheduler_event_subscription;
using sidutil::make_attribute;
using sidutil::parse_attribute;
using sidutil::self_watcher;

#include <vector>
#include <string>

class ubicom32TimerGeneric: public virtual component, 
		protected fixed_attribute_map_component,
		protected fixed_bus_map_component,
		protected fixed_pin_map_component,
		protected no_accessor_component,
		protected no_relation_component
{
public:
  ubicom32TimerGeneric();
  ~ubicom32TimerGeneric() throw() {}
  
  // Helper functions for target view support
  template <class PinType>
  void add_watchable_pin (const std::string& name, PinType* pin)
    {
      this->add_pin (name, pin);
      this->add_attribute (name, pin, "pin");
      this->triggerpoint_manager.add_watchable_attribute (name);
      this->categorize (name, "watchable");
    }
  
  template <class ValueType>
  void add_watchable_register (const std::string& name, ValueType* value)
    {
      this->add_attribute (name, value, "register");
      this->triggerpoint_manager.add_watchable_attribute (name);
      this->categorize (name, "watchable");
    }

  template <class Class, typename Getter, typename Setter>
  void add_watchable_register (const std::string& name,
                               Class* receiver,
                               Getter getter,
                               Setter setter)
    {
      this->add_attribute_virtual (name, receiver, getter, setter, "register");
      this->triggerpoint_manager.add_watchable_attribute (name);
      this->categorize (name, "watchable");
    }

  template <class Class, typename Getter, typename Setter, typename Parameter>
  void add_watchable_register (const std::string& name,
                               Parameter param,
                               Class* receiver,
                               Getter getter,
                               Setter setter)
    {
      this->add_attribute_virtual_parameterized (name, param, receiver, 
                                                 getter, setter, "register");
      this->triggerpoint_manager.add_watchable_attribute (name);
      this->categorize (name, "watchable");
    }

  template <class ValueType>
  void add_watchable_ro_register (const std::string& name, ValueType* value)
    {
      this->add_attribute_ro (name, value, "register");
      this->triggerpoint_manager.add_watchable_attribute (name);
      this->categorize (name, "watchable");
    }

private:
  class bus_interface: public sidutil::word_bus<big_int_4>
  {
  public:
    bus_interface(ubicom32TimerGeneric *h): host(h) {}
    bus::status word_write(host_int_4 addr,
			   big_int_4 mask,
			   big_int_4 data);

    bus::status word_read(host_int_4 addr,
			  big_int_4 mask,
			  big_int_4& data);
  private:
    ubicom32TimerGeneric *host;
  };
  friend class bus_interface;

  // timer registers
  big_int_4 mptval;		/* Multi Purpose Timer Value */
  big_int_4 rsgcfg;		/* Random Number Generator Configuration */
  big_int_4 rtcom;		/* Real Time compare value */
  big_int_4 tkey;		/* Timer block security key */
  big_int_4 wdcom;		/* Watch Dog compare value */
  big_int_4 wdcfg;		/* Watch Dog Output Disable = 0x4d3c2b1a Enable is anything else */
  big_int_4 sysval;		/* System timer value */
  big_int_4 cbcfg1;		/* Clock Congiguration Reg 1 */
  big_int_4 cbcfg2;		/* Clock Congiguration Reg 2 */
  big_int_4 rstflag;		/* Reset Flags */
  big_int_4 trng;		/* Random Number */
  big_int_4 brown;		/* Brown-out trim select */
  big_int_4 syscom1;		/* System Timer compare Register 1 value */
  big_int_4 syscom2;		/* System Timer compare Register 2 value */
  big_int_4 syscom3;		/* System Timer compare Register 3 value */
  big_int_4 syscom4;		/* System Timer compare Register 4 value */
  big_int_4 syscom5;		/* System Timer compare Register 5 value */
  big_int_4 syscom6;		/* System Timer compare Register 6 value */
  big_int_4 syscom7;		/* System Timer compare Register 7 value */
  big_int_4 syscom8;		/* System Timer compare Register 8 value */

  bus_interface bus;

  static const int WD_TKEY_ENABLE = 0xa1b2c3d4;	/* Tkey register has to equal this for
						 WDCFG and WDCOM to be writable*/

  static const int WD_OUTPUT_DISABLE = 0x4d3c2b1a; /* WDCFG should equal this value to disable watchdog timer */

  //Register addresses and indices
  static const int MPTVAL_ADDR = 0x00;
  static const int RSGCFG_ADDR = 0x04;
  static const int RTCOM_ADDR = 0x08;
  static const int TKEY_ADDR = 0x0c;
  static const int WDCOM_ADDR = 0x10;
  static const int WDCFG_ADDR = 0x14;
  static const int SYSVAL_ADDR = 0x18;
  static const int CBCFG1_ADDR = 0x1c;
  static const int CBCFG2_ADDR = 0x20;
  static const int RSTFLAG_ADDR = 0x24;
  static const int TRNG_ADDR = 0x28;
  static const int BROWN_ADDR = 0x2c;
  static const int SYSCOM1_ADDR = 0x80;
  static const int SYSCOM2_ADDR = 0x84;
  static const int SYSCOM3_ADDR = 0x88;
  static const int SYSCOM4_ADDR = 0x8c;
  static const int SYSCOM5_ADDR = 0x90;
  static const int SYSCOM6_ADDR = 0x94;
  static const int SYSCOM7_ADDR = 0x98;
  static const int SYSCOM8_ADDR = 0x9c;
  static const int MPTVAL_INDEX = MPTVAL_ADDR /4;
  static const int RSGCFG_INDEX = RSGCFG_ADDR /4;
  static const int RTCOM_INDEX = RTCOM_ADDR /4;
  static const int TKEY_INDEX = TKEY_ADDR /4;
  static const int WDCOM_INDEX = WDCOM_ADDR /4;
  static const int WDCFG_INDEX = WDCFG_ADDR /4;
  static const int SYSVAL_INDEX = SYSVAL_ADDR /4;
  static const int CBCFG1_INDEX = CBCFG1_ADDR /4;
  static const int CBCFG2_INDEX = CBCFG2_ADDR /4;
  static const int RSTFLAG_INDEX = RSTFLAG_ADDR /4;
  static const int TRNG_INDEX = TRNG_ADDR /4;
  static const int BROWN_INDEX = BROWN_ADDR /4;
  static const int SYSCOM1_INDEX = SYSCOM1_ADDR /4;
  static const int SYSCOM2_INDEX = SYSCOM2_ADDR /4;
  static const int SYSCOM3_INDEX = SYSCOM3_ADDR /4;
  static const int SYSCOM4_INDEX = SYSCOM4_ADDR /4;
  static const int SYSCOM5_INDEX = SYSCOM5_ADDR /4;
  static const int SYSCOM6_INDEX = SYSCOM6_ADDR /4;
  static const int SYSCOM7_INDEX = SYSCOM7_ADDR /4;
  static const int SYSCOM8_INDEX = SYSCOM8_ADDR /4;

  // pins and associated handlers
  output_pin int_set_0;		/* Gets attached to cpu irq_0 */
  output_pin int_set_1;		/* Gets attached to cpu irq_1 */
  output_pin int_set_2;		/* Gets attached to cpu irq_2 */
  output_pin int_set_3;		/* Gets attached to cpu irq_3 */
  output_pin int_set_4;		/* Gets attached to cpu irq_4 */
  output_pin int_set_5;		/* Gets attached to cpu irq_5 */
  output_pin int_set_6;		/* Gets attached to cpu irq_6 */
  output_pin int_set_7;		/* Gets attached to cpu irq_7 */

  output_pin int_set_rttmr;		/* Gets attached to cpu irq_rttmr */

  output_pin wd_timer_rst_out;	/* Output of watchdog timer */

  callback_pin<ubicom32TimerGeneric> reset_pin;
  void reset_pin_handler (host_int_4) { this->reset(); }
  void reset ();
  callback_pin<ubicom32TimerGeneric> clockMptVal_pin;
  void tickMptVal ();
  void tickMptVal(host_int_4) { this->tickMptVal(); } // assume a pulse rather than an edge
  callback_pin<ubicom32TimerGeneric> clockSysVal_pin;
  void tickSysVal ();
  void tickSysVal(host_int_4) { this->tickSysVal(); } // assume a pulse rather than an edge


  // Triggerpoint manager
  friend class self_watcher<ubicom32TimerGeneric>;
  self_watcher<ubicom32TimerGeneric> triggerpoint_manager;
  
  sid::component::status
  pin_factory(const string& name)
    {
      return triggerpoint_manager.create_virtual_pin(name);
    }
  
  void
  pin_junkyard(const string& name)
    {
      return triggerpoint_manager.destroy_virtual_pin(name);
    }

};


#endif // _UBICOM32_TIMER_H
