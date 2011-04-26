
#include <log.h>
#include <cmdargs.h>
#include <mcast.h>

#include <log.h>
#include <service.h>

int
start_service()
{
	char *msg;

	// tempmond server is a multicast client and
	// tempmond client is a multicast server
	if(server_mode == SERVER_MODE)
	{
		if( !mclient_start() )
		{
			write_log(ERRO, "mclient_start() failed");
			return 0;
		}
	}
	else
	{
		if( !mserver_start() )
		{
			write_log(ERRO, "mserver_start() failed");
			return 0;
		}
	}

	// The big loop
	while(1)
	{
		sleep(1);
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
}


int
stop_service()
{
	if(server_mode == SERVER_MODE)
		return mclient_stop();
	else
		return mserver_stop();
}
