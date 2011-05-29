
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
			write_log(CRIT, ME, "initialized hardware failed");
			clean_exit();
		}
		else
			write_log(DBUG, ME, "initialized hardware");

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
	char *msg;

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
			if( !(msg = rcv_mcast_msg()) )
				return 0;
		}
	}

	if( !((*close_hw)()) )
	{
		write_log(CRIT, ME, "close hardware failed");
		clean_exit();
	}
	else
		write_log(DBUG, ME, "closed hardware");

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
