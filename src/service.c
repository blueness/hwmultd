


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

	if(server_mode == SERVER_MODE || server_mode == BOTH_MODE)
	{
		if((ret = (*init_hw)()) != 1)
		{
			write_log(CRIT, __FILE__, "initialized hardware plugin failed: %d", ret);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "initialized hardware plugin");
	}

	if(server_mode == CLIENT_MODE || server_mode == BOTH_MODE)
	{
		if((ret = (*init_cl)()) != 1)
		{
			write_log(CRIT, __FILE__, "initialized client plugin failed: %d", ret);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "initialized client plugin");
	}

	// the hwmultd server is a multicast client and
	// the hwmultd client is a multicast server
	if(server_mode == SERVER_MODE || server_mode == BOTH_MODE)
	{
		if( !mclient_start() )
		{
			write_log(ERRO, __FILE__, "mclient_start() failed");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "mclient_start()");
	}

	//Don't start multicast server in client mode, just "listen" internally
	if (server_mode == CLIENT_MODE)
	{
		if( !mserver_start() )
		{
			write_log(ERRO, __FILE__, "mserver_start() failed");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "mserver_start()");
	}

	continue_big_loop = 1;

	return 1;
}


int
do_service()
{
	char *msg, *rmsg;

	while(continue_big_loop)
	{
		sleep(timing);
		if(server_mode == SERVER_MODE || server_mode == BOTH_MODE)
		{
			msg = read_hw();
			if( !snd_mcast_msg(msg) )
				return 0;
		}

		if (server_mode == CLIENT_MODE || server_mode == BOTH_MODE)
		{
			//Only in pure client mode do we listen for the multicasted info
			if(server_mode == CLIENT_MODE)
				if( !(msg = rcv_mcast_msg()) )
					return 0;

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

	if(server_mode == SERVER_MODE || server_mode == BOTH_MODE)
	{
		if((ret = (*close_hw)()) != 1)
		{
			write_log(CRIT, __FILE__, "close hardware plugin failed: %d", ret);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "closed hardware plugin");
	}

	if (server_mode == CLIENT_MODE || server_mode == BOTH_MODE)
	{
		if((ret = (*close_cl)()) != 1)
		{
			write_log(CRIT, __FILE__, "close client plugin failed: %d", ret);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "closed client plugin");
	}

	if(server_mode == SERVER_MODE || server_mode == BOTH_MODE)
	{
		if( !mclient_stop() )
		{
			write_log(CRIT, __FILE__, "close multicast client failed");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "closed multicast client");
	}

	if (server_mode == CLIENT_MODE)
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
