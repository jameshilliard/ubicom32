/* Target-dependent code for GDB, the GNU debugger.

   Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 
   2009
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "defs.h"
#include "gdbcmd.h"
#include "serial.h"
#include "gdb_select.h"
#include "ubicom32_dongle.h"

static struct serial *ubicom32ispisd_desc = NULL;
int ubicom32_dongle_timeout = 5;

void
dongle_error(const char *string)
{
  error("%s", string);
}

int
dongle_interface_status(void)
{
  if(ubicom32ispisd_desc)
    return 1;
  else
    return 0;
}

/* This shuts down the ISD connection and then shuts down the connection to the dongle */
int
dongle_close(void)
{
  serial_close(ubicom32ispisd_desc);
  ubicom32ispisd_desc = NULL;
  return 0;
}


int
dongle_open(char *remote) {
  ubicom32ispisd_desc = serial_open (remote);
  if (ubicom32ispisd_desc == NULL)
    return -1;
  
  serial_raw (ubicom32ispisd_desc);

  return 0;
}

int
dongle_read(char *pkt, unsigned int numbytes)
{
  unsigned int count =0;

  while(count < numbytes)
    {
      int c;
      if(ubicom32_dongle_timeout > 5)
	c = serial_readchar (ubicom32ispisd_desc, ubicom32_dongle_timeout);
      else
	c = serial_readchar (ubicom32ispisd_desc, count == 0 ? 5 : 1);
      if (c < 0)
	{
	  return -1;
	}
      *pkt++ = c;
      count++;
    }
  return 0;
}
      
int
dongle_write (char *packet, unsigned int length)
{
  if (serial_write (ubicom32ispisd_desc, (char *) packet, length) != 0)
    return -1;
  else
    return 0;
}

int
dongle_poll(void)
{
  if (ubicom32ispisd_desc->bufcnt)
    return 1;

  while (1)
    {
      int numfds;
      fd_set readfds, exceptfds;
      struct timeval tv;

      tv.tv_sec = 0;
      tv.tv_usec = 10000;

      FD_ZERO (&readfds);
      FD_ZERO (&exceptfds);
      FD_SET (ubicom32ispisd_desc->fd, &readfds);
      FD_SET (ubicom32ispisd_desc->fd, &exceptfds);

      numfds = gdb_select (ubicom32ispisd_desc->fd + 1, &readfds, 0, &exceptfds, &tv);

      if (numfds <= 0)
	{
	  if (numfds == 0)
	    return 0;		/* Nothing avaialble */
	  else if (errno == EINTR)
	    continue;
	  else
	    return -1;		/* Got an error from select or poll */
	}
      
      /* Read data available. */
      return 1;
    }
}


static void
set_dongle_timeout (char *args, int from_tty, struct cmd_list_element *c)
{
}  

static void
show_dongle_timeout (struct ui_file *file, int from_tty,
		     struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("\
Dongle timeout is set to %s.\n"),
		    value);
}

void
_initialize_ubicom32_dongle (void)
{
  add_setshow_uinteger_cmd ("dongletimeout", no_class, &ubicom32_dongle_timeout, _("\
Set timeout value for ubicom32 dongle."),  _("\
Show timeout value for ubicom32 dongle."),NULL,
			    set_dongle_timeout,
			    show_dongle_timeout,
			    &setlist, &showlist);
}
