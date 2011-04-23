
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <signalhandler.h>
#include <log.h>
#include <cmdargs.h>

int
main( int argc, char *argv[] )
{
	char *msg;

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
	write_log(CRIT, "TESTING - %s %c %d %f\n", "hello world", 'x', 42, 3.14159);
	write_log(ERRO, "TESTING - %s %c %d %f\n", "hello world", 'x', 42, 3.14159);
	write_log(INFO, "TESTING - %s %c %d %f\n", "hello world", 'x', 42, 3.14159);
	write_log(DBUG, "TESTING - %s %c %d %f\n", "hello world", 'x', 42, 3.14159);

	// tempmond server is a multicast client and
	// tempmond client is a multicast server
	if(server_mode == SERVER_MODE)
		mclient_start();
	else
		mserver_start();

	// The big loop
	while(1)
	{
		sleep(5);
		if(server_mode == SERVER_MODE)
		{
			if(send_mcast_msg("test"))
				write_log(DBUG, "%s\n", "Server sent message");
			else
				write_log(DBUG, "%s\n", "Server FAILED sent message");

		}
		else
		{
			if(msg = recv_mcast_msg())
				write_log(INFO, "Message received: %s\n", msg);
			else
				write_log(INFO, "No message received\n");
		}
	}

	return 0;
}
