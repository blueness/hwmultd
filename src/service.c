
#include <stdlib.h>
#include <unistd.h>

#include <cmdargs.h>
#include <log.h>
#include <mcast.h>
#include <service.h>
#include <signalhandler.h>


int
start_service()
{
	char *msg;

	if((wpid = fork()) < 0)
	{
		write_log(ERRO, "worker: fork failed") ;
		return 0;
	}

	if(wpid > 0)
		return 1;

	write_log(DBUG, "worker: forked");

	if( !sighandler_worker() )
	{
		write_log(CRIT, "worker: register signals failed");
		return 0;
	}
	else
		write_log(DBUG, "worker: registered signals");

	// We are the worker process
	wpid = getpid() ;

	write_log(DBUG, "worker: set process id %d", (int)(wpid)) ;

	// tempmond server is a multicast client and
	// tempmond client is a multicast server
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

	// The small loop
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


void
stop_service()
{
	if(server_mode == SERVER_MODE)
		mclient_stop();
	else
		mserver_stop();

	exit(EXIT_SUCCESS);
}
