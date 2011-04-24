
#include <log.h>
#include <cmdargs.h>
#include <mcast.h>


void
start_service()
{
	char *msg;

	// tempmond server is a multicast client and
	// tempmond client is a multicast server
	if(server_mode == SERVER_MODE)
	{
		if( !mclient_start() )
		{
			write_log(CRIT, "mclient_start() failed");
			clean_exit();
		}
	}
	else
	{
		if( !mserver_start() )
		{
			write_log(CRIT, "mserver_start() failed");
			clean_exit();
		}
	}

	// The big loop
	while(1)
	{
		sleep(5);
		if(server_mode == SERVER_MODE)
		{
			if( !snd_mcast_msg("test") )
				write_log(CRIT, "server message send failed");
		}
		else
		{
			if( !(msg = rcv_mcast_msg()) )
				write_log(CRIT, "message message receive failed");
		}
	}
}


void
stop_service()
{
	if(server_mode == SERVER_MODE)
		mclient_stop();
	else
		mserver_stop();
}
