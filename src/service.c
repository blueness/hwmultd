
#include <stdlib.h>
#include <unistd.h>

#include <log.h>
#include <cmdargs.h>
#include <mcast.h>
#include <service.h>
#include <signalhandler.h>


int
start_service()
{
	char *msg;

	if((wpid = fork()) < 0)
	{
		write_log(ERRO, "worker fork failed") ;
		return 0;
	}

	if(wpid > 0)
		return 1;

	write_log(DBUG, "worker forked");

	if( !sighandler() )
	{
		write_log(CRIT, "register signals failed");
		clean_exit();
	}
	else
		write_log(DBUG, "registered signals");

	// We are the worker process
	worker_mode = 1 ;
	wpid = getpid() ;

	write_log(DBUG, "set worker process id %d", (int)(wpid)) ;

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
			write_log(DBUG, "mclient_start() started");
	}
	else
	{
		if( !mserver_start() )
		{
			write_log(ERRO, "mserver_start() failed");
			return 0;
		}
		else
			write_log(DBUG, "mserver_start() started");
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


int
stop_service()
{
	if(server_mode == SERVER_MODE)
		return mclient_stop();
	else
		return mserver_stop();

	exit(EXIT_SUCCESS);
}
