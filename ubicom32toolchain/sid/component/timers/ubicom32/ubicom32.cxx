// ubicom32.cxx - An implementation of the timers for the Ubicom32
// family of boards.  -*- C++ -*-

// Copyright (C) 1999, 2000 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.
// ubicom32.cxx

#include "ubicom32-timer.h"

using namespace sid;
using namespace sidutil;

ubicom32TimerGeneric::ubicom32TimerGeneric()
  :mptval(0), rsgcfg(0), rtcom(0), tkey(0), wdcom(0),
   wdcfg(0xa1b2c3d4), sysval(0), cbcfg1(0), cbcfg2(0),
   rstflag(0), trng(0xffffffff), brown(0xffffffff),
   syscom1(0), syscom2(0), syscom3(0), syscom4(0),
   syscom5(0), syscom6(0), syscom7(0), syscom8(0),
   bus(this),
   reset_pin(this, & ubicom32TimerGeneric::reset_pin_handler), 
   clockMptVal_pin(this, & ubicom32TimerGeneric::tickMptVal),
   clockSysVal_pin(this, & ubicom32TimerGeneric::tickSysVal),
   triggerpoint_manager(this)
{
  add_bus("Timer-bus", &this->bus);

  // pins
  add_watchable_pin("int_set_0", &this->int_set_0);
  add_watchable_pin("int_set_1", &this->int_set_1);
  add_watchable_pin("int_set_2", &this->int_set_2);
  add_watchable_pin("int_set_3", &this->int_set_3);
  add_watchable_pin("int_set_4", &this->int_set_4);
  add_watchable_pin("int_set_5", &this->int_set_5);
  add_watchable_pin("int_set_6", &this->int_set_6);
  add_watchable_pin("int_set_7", &this->int_set_7);

  add_watchable_pin("int_set_rttmr", &this->int_set_rttmr);
  add_watchable_pin("wd_rst_out", &this->wd_timer_rst_out);

  add_watchable_pin("reset", & this->reset_pin);
  
  add_pin("clockMptVal", &clockMptVal_pin);
  add_pin("clockSysVal", &clockSysVal_pin);

  // make the registers watchable so sidlib can read and write these registers
  add_watchable_register("mptval", &this->mptval); 
  add_watchable_register("rsgcfg", &this->rsgcfg); 
  add_watchable_register("rtcom", &this->rtcom); 
  add_watchable_register("tkey", &this->tkey); 
  add_watchable_register("wdcom", &this->wdcom); 
  add_watchable_register("wdcfg", &this->wdcfg); 
  add_watchable_register("sysval", &this->sysval); 
  add_watchable_register("cbcfg1", &this->cbcfg1); 
  add_watchable_register("cbcfg2", &this->cbcfg2); 
  add_watchable_register("rstflag", &this->rstflag); 
  add_watchable_register("trng", &this->trng); 
  add_watchable_register("brown", &this->brown); 
  add_watchable_register("syscom1", &this->syscom1); 
  add_watchable_register("syscom2", &this->syscom2); 
  add_watchable_register("syscom3", &this->syscom3); 
  add_watchable_register("syscom4", &this->syscom4); 
  add_watchable_register("syscom5", &this->syscom5); 
  add_watchable_register("syscom6", &this->syscom6); 
  add_watchable_register("syscom7", &this->syscom7); 
  add_watchable_register("syscom8", &this->syscom8);
}

void
ubicom32TimerGeneric::reset ()
{
  this->mptval = 0;
  this->rsgcfg = 0;
  this->rtcom = 0; 
  this->tkey =0;
  this->wdcom = 0;
  this->wdcfg = WD_OUTPUT_DISABLE;
  this->sysval =0;
  this->cbcfg1 = 0x80000080;
  this->cbcfg2 =0x80;
  this->rstflag = 0;
  this->trng = 0xffffffff;
  this->brown = 0xffffffff;
  this->syscom1 = 0;
  this->syscom2 = 0;
  this->syscom3 = 0;
  this->syscom4 = 0;
  this->syscom5 = 0;
  this->syscom6 = 0;
  this->syscom7 = 0;
  this->syscom8 = 0;

  if (this->int_set_0.recall() != 0)
    this->int_set_0.drive (0);
  if (this->int_set_1.recall() != 0)
    this->int_set_1.drive (0);
  if (this->int_set_2.recall() != 0)
    this->int_set_2.drive (0);
  if (this->int_set_3.recall() != 0)
    this->int_set_3.drive (0);
  if (this->int_set_4.recall() != 0)
    this->int_set_4.drive (0);
  if (this->int_set_5.recall() != 0)
    this->int_set_5.drive (0);
  if (this->int_set_6.recall() != 0)
    this->int_set_6.drive (0);
  if (this->int_set_7.recall() != 0)
    this->int_set_7.drive (0);
  if (this->int_set_rttmr.recall() != 0)
    this->int_set_rttmr.drive (0);
  if (this->wd_timer_rst_out.recall() != 0)
    this->wd_timer_rst_out.drive (0);
}

void
ubicom32TimerGeneric::tickMptVal()
{
  // check for triggerpoints
  triggerpoint_manager.check_and_dispatch();

  // test the various comparators and generate interrupts if needed
  if(this->mptval == this->rtcom)
    {
      // generate an interrupt on irq_set_rttmr
      this->int_set_rttmr.drive (1);
    }

  if(this->wdcfg != WD_OUTPUT_DISABLE)
    {
      //watch timer output is enabled
      if(this->mptval == this->wdcom)
	{
	  // Generate a pulse on wd_timer_rst_out
	  this->wd_timer_rst_out.drive (1);
	}
    }

  // OK to bump up MPTVAL
  this->mptval = this->mptval +1;
}

void
ubicom32TimerGeneric::tickSysVal()
{
  // now test the SYSCON timers
  if(this->sysval == this->syscom1)
    {
      // It matches. Time to generate interrupts
      this->int_set_0.drive (1);
    }

  if(this->sysval == this->syscom2)
    {
      // It matches. Time to generate interrupts
      this->int_set_1.drive (1);
    }

  if(this->sysval == this->syscom3)
    {
      // It matches. Time to generate interrupts
      this->int_set_2.drive (1);
    }

  if(this->sysval == this->syscom4)
    {
      // It matches. Time to generate interrupts
      this->int_set_3.drive (1);
    }

  if(this->sysval == this->syscom5)
    {
      // It matches. Time to generate interrupts
      this->int_set_4.drive (1);
    }

  if(this->sysval == this->syscom6)
    {
      // It matches. Time to generate interrupts
      this->int_set_5.drive (1);
    }

  if(this->sysval == this->syscom7)
    {
      // It matches. Time to generate interrupts
      this->int_set_6.drive (1);
    }

  if(this->sysval == this->syscom8)
    {
      // It matches. Time to generate interrupts
      this->int_set_7.drive (1);
    }


  // now bump up the 
  this->sysval = this->sysval + 1;
}

bus::status
ubicom32TimerGeneric::bus_interface::word_write(host_int_4 addr,
					     big_int_4 mask,
					     big_int_4 be_data)
{
  host_int_4 data = be_data;
  host_int_4 le_mask = mask;
  host_int_4 neg_mask = ~mask;
  host_int_4 masked_data = data & le_mask;

  switch (addr)
    {
    case RSGCFG_INDEX:
      // Set only bits 0-2
      data = this->host->rsgcfg;
      data &= neg_mask;
      data |= masked_data;
      data &= 7;
      this->host->rsgcfg = data;
      break;

    case RTCOM_INDEX:
      // set rtcom
      data = this->host->rtcom;
      data &= neg_mask;
      data |= masked_data;
      this->host->rtcom = data;
      break;

    case TKEY_INDEX:
      // Set TKEY
      data = this->host->tkey;
      data &= neg_mask;
      data |= masked_data;
      this->host->tkey = data;
      break;

    case WDCOM_INDEX:
      // can set this only if TKEY matches WD_TKEY_ENABLE
      if(this->host->tkey == WD_TKEY_ENABLE)
	{
	  data = this->host->wdcom;
	  data &= neg_mask;
	  data |= masked_data;
	  this->host->wdcom = data;
	}
      break;

    case WDCFG_INDEX:
      // can set this only if TKEY matches WD_TKEY_ENABLE
      if(this->host->tkey == WD_TKEY_ENABLE)
	{
	  data = this->host->wdcfg;
	  data &= neg_mask;
	  data |= masked_data;
	  this->host->wdcfg = data;
	}
      break;

    case CBCFG1_INDEX:
      // only bits 23-0 are writable
      data = this->host->cbcfg1;
      data &= neg_mask;
      data |= masked_data;
      data &= 0xffffff;
      this->host->cbcfg1 = data;
      break;

    case CBCFG2_INDEX:
      // only bits 22-0 are writable
      data = this->host->cbcfg2;
      data &= neg_mask;
      data |= masked_data;
      data &= 0x7fffff;
      this->host->cbcfg2 = data;
      break;

    case BROWN_INDEX:
      // only bits 2-0 are writable
      data = this->host->brown;
      data &= neg_mask;
      data |= masked_data;
      data &= 0x7;
      this->host->brown = data;
      break;

    case SYSCOM1_INDEX:
      data = this->host->syscom1;
      data &= neg_mask;
      data |= masked_data;
      this->host->syscom1 = data;
      break;

    case SYSCOM2_INDEX:
      data = this->host->syscom2;
      data &= neg_mask;
      data |= masked_data;
      this->host->syscom2 = data;
      break;

    case SYSCOM3_INDEX:
      data = this->host->syscom3;
      data &= neg_mask;
      data |= masked_data;
      this->host->syscom3 = data;
      break;

    case SYSCOM4_INDEX:
      data = this->host->syscom4;
      data &= neg_mask;
      data |= masked_data;
      this->host->syscom4 = data;
      break;

    case SYSCOM5_INDEX:
      data = this->host->syscom5;
      data &= neg_mask;
      data |= masked_data;
      this->host->syscom5 = data;
      break;

    case SYSCOM6_INDEX:
      data = this->host->syscom6;
      data &= neg_mask;
      data |= masked_data;
      this->host->syscom6 = data;
      break;

    case SYSCOM7_INDEX:
      data = this->host->syscom7;
      data &= neg_mask;
      data |= masked_data;
      this->host->syscom7 = data;
      break;

    case SYSCOM8_INDEX:
      data = this->host->syscom8;
      data &= neg_mask;
      data |= masked_data;
      this->host->syscom8 = data;
      break;

    default:
      // Unmapped
      return bus::unmapped;
    }

  // check for triggerpoints
  host->triggerpoint_manager.check_and_dispatch();
  return bus::ok;
}

bus::status
ubicom32TimerGeneric::bus_interface::word_read(host_int_4 addr,
					    big_int_4 mask,
					    big_int_4& data)
{
  switch (addr)
    { 
    case MPTVAL_INDEX:
      data = this->host->mptval;
      break;

    case RSGCFG_INDEX:
      data = this->host->rsgcfg;
      break;

    case RTCOM_INDEX:
      data = this->host->rtcom;
      break;

    case TKEY_INDEX:
      data = this->host->tkey;
      break;

    case WDCOM_INDEX:
      data = this->host->wdcom;
      break;

    case WDCFG_INDEX:
      data = this->host->wdcfg;
      break;

    case SYSVAL_INDEX:
      data = this->host->sysval;
      break;

    case CBCFG1_INDEX:
      data = this->host->cbcfg1;
      break;

    case CBCFG2_INDEX:
      data = this->host->cbcfg2;
      break;

    case RSTFLAG_INDEX:
      data = this->host->rstflag;
      break;

    case TRNG_INDEX:
      data = this->host->trng;
      break;

    case BROWN_INDEX:
      data = this->host->brown;
      break;

    case SYSCOM1_INDEX:
      data = this->host->syscom1;
      break;

    case SYSCOM2_INDEX:
      data = this->host->syscom2;
      break;

    case SYSCOM3_INDEX:
      data = this->host->syscom3;
      break;

    case SYSCOM4_INDEX:
      data = this->host->syscom4;
      break;

    case SYSCOM5_INDEX:
      data = this->host->syscom5;
      break;

    case SYSCOM6_INDEX:
      data = this->host->syscom6;
      break;

    case SYSCOM7_INDEX:
      data = this->host->syscom7;
      break;

    case SYSCOM8_INDEX:
      data = this->host->syscom8;
      break;

    default:
      return bus::unmapped;
    }

  // check for triggerpoints
  host->triggerpoint_manager.check_and_dispatch();
  return bus::ok;
}

