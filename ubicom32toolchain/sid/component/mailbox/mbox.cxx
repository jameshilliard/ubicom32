// Uart.cxx - NS16550 UART component.  -*- C++ -*-

// Copyright (C) 1999, 2000 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.


#include "mbox.h"

#include <sidpinattrutil.h>
#include <sidso.h>

using sid::component_library;
using sid::COMPONENT_LIBRARY_MAGIC;
void
Mbox :: reset()
{
  this->in_count = this->out_count = 0;
  this->numBytes = 0;
  memset((void *)this->replyBuf, 0, 5);
  this->replyBuf[0] = 0x80;
  this->mbox_in_fifo[0] = this->mbox_in_fifo[1] = 0;
  this->mbox_out_fifo[0] = this->mbox_out_fifo[1] = 0;
}

Mbox :: Mbox()
  :bus(this), 
  remote_rx(this, & Mbox::remote_rx_handler),
  reset_pin(this, & Mbox::reset_pin_handler),
  triggerpoint_manager(this)
{
  in_count = 0;
  out_count = 0;

  add_bus( "Mbox-Bus", &this->bus );

  add_watchable_pin( "remote-rx", &this->remote_rx );
  add_watchable_pin( "remote-tx", &this->remote_tx );
  add_watchable_pin( "interrupt", &this->interrupt );
  add_watchable_pin( "reset", &this->reset_pin );
  this->reset();
}

bus::status
Mbox::bus_interface::word_write(host_int_4 addr,
				sid::big_int_4 mask,
				sid::big_int_4 data)
{
  // From the cpu you can only write the OutBox at 04
  switch(addr)
    {
    case 1:
      // write the out box
      if(host->out_count == 2)
	{
	  // full fifo and we are going to lose the first word 
	  host->mbox_out_fifo[0] = host->mbox_out_fifo[1];
	  host->mbox_out_fifo[1] = data;
	  return sid::bus::ok;
	}
      else if(host->out_count == 1)
	{
	  host->mbox_out_fifo[1] = data;
	}
      else
	{
	  host->mbox_out_fifo[0] = data;
	}

      host->out_count++;
      return sid::bus::ok;
      break;
    default:
      return sid::bus::unpermitted;
    }
}


bus::status
Mbox::bus_interface::word_read(host_int_4 addr,
				sid::big_int_4 mask,
				sid::big_int_4& data)
{
  // From the cpu you can only read the InBox at 0 and Status at 0x8
  switch(addr)
    {
    case 0:
      data = host->mbox_in_fifo[0];
      if(host->in_count)
	{
	  if(host->in_count == 2)
	    {
	      // collapse the fifo 
	      host->mbox_in_fifo[0] = host->mbox_in_fifo[1];
	    }
	  host->in_count --;
	}
      return sid::bus::ok;
      break;
    case 2:
      {
	// status request. We have to build it
	host_int_4 stat=0;
	
	if(host->in_count == 0)
	  {
	    // In Box is empty
	    stat |= 0x40000000;
	  }
	else if(host->in_count == 2)
	  {
	    // In Box is full
	    stat |= 0x80000000;
	  }

	if(host->out_count == 0)
	  {
	    // In Box is empty
	    stat |= 0x10000000;
	  }
	else if(host->out_count == 2)
	  {
	    // In Box is full
	    stat |= 0x20000000;
	  }
	data = stat;
	return sid::bus::ok;
      }
      break;
    default:
      // Bad address
      return sid::bus::unpermitted;
    }
}

void
Mbox::remote_rx_handler( host_int_4 value )
{
  // sid-to-socket-server sends requests through here 
  if (value & ~0x00FF)
    {
      numBytes =0;
      return ;
    }

  this->buffer[this->numBytes++] = value&0xff;

  if(this->numBytes == 5)
    {
      this->numBytes=0;
      this->command = buffer[0];
    }
  else
    {
      // nothing to do
      return;
    }

  // Ship out the pending reply
  for(int i=0; i< 5; i++)
    this->remote_tx.drive(replyBuf[i]);

  memset(this->replyBuf, 0, 5);
  this->replyBuf[0] = 0x80|(unsigned char ) this->command;
  
  // now interpret the request
  switch(this->command)
    {
    case ISD_OPEN:
    case ISD_CLOSE:
    case ISD_TEST_MODE:
    case ISD_DEBUG_RST_REQ:
    case ISD_DEVICE_ID:
    case ISD_DEVICE_VER:
      // For the time being treat the above as a NOP command
    case ISD_NOP:
      {
	// NOP command
	break;
      }
    case ISD_MAIL_STATUS:
      {
	// status request. Need to build it 
	// status request. We have to build it
	host_int_4 stat=0;
	big_int_4 stat1;

	if(this->in_count == 0)
	  {
	    // In Box is empty
	    stat |= 0x40000000;
	  }
	else if(this->in_count == 2)
	  {
	    // In Box is full
	    stat |= 0x80000000;
	  }

	if(this->out_count == 0)
	  {
	    // In Box is empty
	    stat |= 0x10000000;
	  }
	else if(this->out_count == 2)
	  {
	    // In Box is full
	    stat |= 0x20000000;
	  }
	
	stat1 = stat;
	// copy the result into the reply buffer
	memmove((void *)&this->replyBuf[1], (void *) &stat, 4);
	break;
      }

    case ISD_WRITE_MAIL_BOX:
      {
	if(this->in_count == 2)
	  {
	    // full in fifo. We are going to lose the first word
	    this->mbox_in_fifo[0] = this->mbox_in_fifo[1];

	    /* copy the data in from the command buffer */
	    memmove((void *) &this->buffer[1], (void *)&this->mbox_in_fifo[1], 4);
	  }
	else
	  { 
	    if(this->in_count == 1)
	      {
		/* copy the data in from the command buffer */
		memmove((void *)&this->mbox_in_fifo[1], (void *) &this->buffer[1], 4);
	      }
	    else
	      {
		/* copy the data in from the command buffer */
		memmove((void *)&this->mbox_in_fifo[0], (void *) &this->buffer[1], 4);
	      }
	    this->in_count ++;
	  }

	// generate a mailbox interrupt
	this->interrupt.drive(1);
	break;
      }

    case ISD_READ_MAIL_BOX:
      {
	// move top of fifo to reply buffer
	memmove((void *)&this->replyBuf[1], (void *) &this->mbox_out_fifo[0], 4);
	if(this->out_count)
	  {
	    if(this->out_count == 2)
	      {
		// collapse the fifo 
		this->mbox_out_fifo[0] = this->mbox_out_fifo[1];
	      }
	    this->out_count --;
	  }
	break;
      }
    case ISD_CLEAN_MAIL:
      {
	// reset the in and out fifo counts
	this->in_count = this->out_count = 0;
	break;
      }
    }
}


//DLL wrapper stuff

static vector<string>
MboxListTypes()
{
  vector<string> types;
  types.push_back(string("hw-ubicom32-mailbox"));
  return types;
}

static component*
MboxCreate(const string& typeName)
{
  if(typeName == "hw-ubicom32-mailbox")
    return new Mbox();
  else
    return 0;
}


static void
MboxDelete(component* c)
{
  delete dynamic_cast<Mbox*>(c);
}


// static object
DLLEXPORT extern const component_library mbox_component_library;

const component_library mbox_component_library = 
{
  COMPONENT_LIBRARY_MAGIC,
  & MboxListTypes, 
  & MboxCreate,
  & MboxDelete
};

