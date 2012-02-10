// mbox.h - Declarations for the Ubicom IP3k mailbox component.  -*- C++ -*-

// Copyright (C) 2002K Ubicom Inc.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#ifndef MBOX_DEF_H
#define MBOX_DEF_H	1

#include <sidtypes.h>
#include <sidcomp.h>
#include <sidcomputil.h>
#include <sidbusutil.h>
#include <sidcpuutil.h>
#include <sidpinutil.h>
#include <sidattrutil.h>
#include <sidschedutil.h>
#include <sidwatchutil.h>

using namespace std;
using namespace sid;
using namespace sidutil;



class Mbox: public virtual component,
	    public no_accessor_component,
	    public fixed_attribute_map_component,
	    public no_relation_component,
	    public fixed_bus_map_component,
	    public fixed_pin_map_component
{
private:
  enum isd_command {
    ISD_NOP,
    ISD_OPEN,
    ISD_CLOSE,
    ISD_DEVICE_ID,
    ISD_DEVICE_VER,
    ISD_MAIL_STATUS,
    ISD_DEBUG_RST_REQ,
    ISD_WRITE_MAIL_BOX,
    ISD_READ_MAIL_BOX,
    ISD_CLEAN_MAIL,
    ISD_TEST_MODE
  };

  big_int_4 mbox_in_fifo[2];	// Host to IP3k fifo
  big_int_4 mbox_out_fifo[2];	// IP3k to Host fifo

  unsigned int in_count;	// # of entries in in fifo
  unsigned int out_count;	// # of entries in out fifo


  unsigned int command;		// ISD command
  unsigned char buffer[5];	// command buffer 5 bytes = 1 command
  unsigned char numBytes;	// No of received bytes
  unsigned char replyBuf[5];	// Reply for previous command
  
  // Pins for the sid-to-socket-server
  callback_pin<Mbox> remote_rx;
  friend class callback_pin<Mbox>;
  callback_pin<Mbox> reset_pin;
  void reset_pin_handler (host_int_4) { this->reset(); }

  void remote_rx_handler( host_int_4 value );
  
  output_pin remote_tx;

  output_pin interrupt;		// Mailbox interrupt
  // The bus
private:
  class bus_interface: public sidutil::word_bus<big_int_4>
  {
  public:
    bus_interface(Mbox *h): host(h) {};
    bus::status word_write(host_int_4 addr,
			   sid::big_int_4 mask,
			   sid::big_int_4 data);

    bus::status word_read(host_int_4 addr,
			  sid::big_int_4 mask,
			  sid::big_int_4& data);
  private:
    Mbox *host;
  };
  friend class bus_interface;

  bus_interface bus;
  // Triggerpoint manager
  friend class self_watcher<Mbox>;
  self_watcher<Mbox> triggerpoint_manager;
  
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

public:
  // Helper functions for target view support
  template <class PinType>
  void add_watchable_pin (const std::string& name, PinType* pin)
    {
      this->add_pin (name, pin);
      this->add_attribute (name, pin, "pin");
      this->triggerpoint_manager.add_watchable_attribute (name);
      this->categorize (name, "watchable");
    }

  Mbox();
  ~Mbox() throw () {}
  void reset();
};

#endif // MBOX_DEF_H
