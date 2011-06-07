
#include <stdlib.h>
#include <unistd.h>

#include <cmdargs.h>
#include <log.h>
#include <mcast.h>
#include <plugins.h>
#include <service.h>


#undef ME
#define ME "service.c"


int
start_service()
{
	// the hwmultd server is a multicast client and
	// the hwmultd client is a multicast server
	if(server_mode == SERVER_MODE)
	{
		if( !((*init_hw)()) )
		{
			write_log(CRIT, ME, "initialized hardware plugin failed");
			clean_exit();
		}
		else
			write_log(DBUG, ME, "initialized hardware plugin");

		if( !mclient_start() )
		{
			write_log(ERRO, ME, "mclient_start() failed");
			return 0;
		}
		else
			write_log(DBUG, ME, "mclient_start()");
	}
	else
	{
		if( !((*init_cl)()) )
		{
			write_log(CRIT, ME, "initialized client plugin failed");
			clean_exit();
		}
		else
			write_log(DBUG, ME, "initialized client plugin");

		if( !mserver_start() )
		{
			write_log(ERRO, ME, "mserver_start() failed");
			return 0;
		}
		else
			write_log(DBUG, ME, "mserver_start()");
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
		if(server_mode == SERVER_MODE)
		{
			msg = read_hw();
			if( !snd_mcast_msg(msg) )
				return 0;
		}
		else
		{
			if( !(msg = rcv_mcast_msg()) )		//caller must free buffer
				return 0;

			if( !( rmsg = (*act_cl)(msg) ) )	//caller must free buffer
			{
				write_log(CRIT, ME, "client action failed");
				clean_exit();
			}
			else
			{
				write_log(DBUG, ME, "client action: %s", rmsg);
			}

			free(rmsg);
			free(msg);
		}
	}

	if(server_mode == SERVER_MODE)
	{
		if( !((*close_hw)()) )
		{
			write_log(CRIT, ME, "close hardware plugin failed");
			clean_exit();
		}
		else
			write_log(DBUG, ME, "closed hardware plugin");
	}
	else
	{
		if( !((*close_cl)()) )
		{
			write_log(CRIT, ME, "close client plugin failed");
			clean_exit();
		}
		else
			write_log(DBUG, ME, "closed client plugin");
	}

	return 1;
}


int
stop_service()
{
	write_log(DBUG, ME, "stopping service");
	if(server_mode == SERVER_MODE)
		mclient_stop();
	else
		mserver_stop();

	return 1;
}
