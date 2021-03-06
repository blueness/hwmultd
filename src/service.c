/*
	This file is part of hwmultd.
	Copyright (C) 2011  Anthony G. Basile

	hwmultd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	hwmultd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "cmdargs.h"
#include "log.h"
#include "mcast.h"
#include "plugins.h"
#include "service.h"



#include <stdlib.h>
#include <unistd.h>



int
start_service()
{
	int ret ;

	// If we run in either server mode or both, initialize the server hardware
	if(operation_mode == SERVER_MODE || operation_mode == BOTH_MODE)
	{
		if((ret = (*init_hw)()) != 1)
		{
			write_log(CRIT, __FILE__, "initialized hardware plugin failed: %d", ret);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "initialized hardware plugin");
	}

	// If we run in either client mode or both, initialize the client hardware
	if(operation_mode == CLIENT_MODE || operation_mode == BOTH_MODE)
	{
		if((ret = (*init_cl)()) != 1)
		{
			write_log(CRIT, __FILE__, "initialized client plugin failed: %d", ret);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "initialized client plugin");
	}

	// The hwmultd server is a multicast client, which initiates net communication
	// The hwmultd client is a multicast server, which responds to received net communication
	if(operation_mode == SERVER_MODE || operation_mode == BOTH_MODE)
	{
		if( !mclient_start() )
		{
			write_log(ERRO, __FILE__, "mclient_start() failed");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "mclient_start()");
	}

	// Only start multicast server in pure client mode
	// If we are in both mode, our client will just "listen" internally
	if(operation_mode == CLIENT_MODE)
	{
		if( !mserver_start() )
		{
			write_log(ERRO, __FILE__, "mserver_start() failed");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "mserver_start()");
	}

	// We're ready to jump into the little loop
	continue_little_loop = 1;

	return 1;
}


int
do_service()
{
	char *msg, *rmsg;

	// The value of continue_little_loop is signal driven
	while(continue_little_loop)
	{
		// TODO - Do we want this for client mode?
		// Wait some delay time
		sleep(timing);

		// If we are running in server mode or both, we need to
		// read some value out of the hardware and multicast it out
		if(operation_mode == SERVER_MODE || operation_mode == BOTH_MODE)
		{
			msg = read_hw();
			if( !snd_mcast_msg(msg) )
				return 0;
		}

		// If we are running in client mode or both, we need to
		// receive a message and then act on it
		if (operation_mode == CLIENT_MODE || operation_mode == BOTH_MODE)
		{
			// We should only listen for multicasted messages in pure client mode
			// In both mode, we already have the msg we are to act on
			if(operation_mode == CLIENT_MODE)
				if( !(msg = rcv_mcast_msg()) )
					return 0;

			// TODO - we should not call clean_exit here but return 0
			// TODO - and let hwmultd.c take care of calling clean_exit
			if( !(rmsg = (*act_cl)(msg)) )
			{
				write_log(CRIT, __FILE__, "client action failed");
				clean_exit();
			}
			else
			{
				write_log(DBUG, __FILE__, "client action: %s", rmsg);
			}
		}
	}

	return 1;
}


int
stop_service()
{
	int ret ;

	// If we are running in server mode or both, we need to close down our server hardware
	if(operation_mode == SERVER_MODE || operation_mode == BOTH_MODE)
	{
		if((ret = (*close_hw)()) != 1)
		{
			write_log(CRIT, __FILE__, "close hardware plugin failed: %d", ret);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "closed hardware plugin");
	}

	// If we are running in client mode or both, we need to close down our client hardware
	if (operation_mode == CLIENT_MODE || operation_mode == BOTH_MODE)
	{
		if((ret = (*close_cl)()) != 1)
		{
			write_log(CRIT, __FILE__, "close client plugin failed: %d", ret);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "closed client plugin");
	}

	// If we are running in server mode or both, we need to shutdown the multicast client
	if(operation_mode == SERVER_MODE || operation_mode == BOTH_MODE)
	{
		if( !mclient_stop() )
		{
			write_log(CRIT, __FILE__, "close multicast client failed");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "closed multicast client");
	}

	// Only in pure client mode do we need to shutdown our multicast server
	if (operation_mode == CLIENT_MODE)
	{
		if( !mserver_stop() )
		{
			write_log(CRIT, __FILE__, "close multicast server failed");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "closed multicast server");
	}

	return 1;
}
