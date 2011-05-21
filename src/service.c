
#include <unistd.h>

#include <cmdargs.h>
#include <log.h>
#include <mcast.h>
#include <service.h>


int
start_service()
{
	// the hwmultd server is a multicast client and
	// the hwmultd client is a multicast server
	if(server_mode == SERVER_MODE)
	{
		if( !mclient_start() )
		{
			write_log(ERRO, "mclient_start() failed");
			return 0;
		}
		else
			write_log(DBUG, "mclient_start()");
	}
	else
	{
		if( !mserver_start() )
		{
			write_log(ERRO, "mserver_start() failed");
			return 0;
		}
		else
			write_log(DBUG, "mserver_start()");
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
			if( !snd_mcast_msg("test") )
				return 0;
		}
		else
		{
			if( !(msg = rcv_mcast_msg()) )
				return 0;
		}
	}

	return 1;
}


int
stop_service()
{
	/*
	if(server_mode == SERVER_MODE)
		return mclient_stop();
	else
		return mserver_stop();
	*/
	if(server_mode == SERVER_MODE)
		mclient_stop();
	else
		mserver_stop();

	return 1;
}
