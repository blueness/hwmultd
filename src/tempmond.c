
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <signalhandler.h>
#include <log.h>
#include <cmdargs.h>


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
			if(snd_mcast_msg("test"))
				write_log(DBUG, "server sent message");
			else
				write_log(CRIT, "server message send failed");
		}
		else
		{
			if(msg = rcv_mcast_msg())
				write_log(DBUG, "message received: %s", msg);
			else
				write_log(CRIT, "message message receive failed");
		}
	}
}


void
stop_service()
{
	if(server_mode == SERVER_MODE)
	{
		if( !mclient_stop() )
			write_log(CRIT, "mclient_stop() failed");
	}
	else
	{
		if( !mserver_stop() )
			write_log(CRIT, "mserver_start() failed");
	}
}


int
main( int argc, char *argv[] )
{
	// Fork into background
	pid_t pid = fork();

	if (pid < 0)
	{
		fprintf( stderr, "Failed to fork.\n") ;
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
		exit(EXIT_SUCCESS);

	pid = getpid() ;

	// Create a new SID for the child process
	pid_t sid = setsid();
	if (sid < 0)
	{
		fprintf( stderr, "Failed to set session id.\n") ;
		exit(EXIT_FAILURE);
	}

	// Open log
	if( !open_log() )
	{
		fprintf( stderr, "Failed to open log.\n") ;
		exit(EXIT_FAILURE);
	}

	// Parse config file to set global parameters
	parse_cfg_file();

	// Parse command line arguments to set global parameters
	parse_cmd_args( argc, argv );

	// Close open files inherited from parent
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// Handle signals
	int i ;
	const int caught_signal[] = { SIGTERM, SIGINT, SIGHUP, 0 };

	for (i = 0; caught_signal[i] != 0; i++)
		signal(caught_signal[i],sighandler);

	// Log test before jumping into the big loo
	write_log(CRIT, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(ERRO, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(INFO, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(DBUG, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);

	start_service();
}
