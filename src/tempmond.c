
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
	/* Fork into background */
	pid_t pid = fork();

	if (pid < 0)
	{
		fprintf( stderr, "Failed to fork.\n") ;
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
		exit(EXIT_SUCCESS);

	pid = getpid() ;

	/* Create a new SID for the child process */
	pid_t sid = setsid();
	if (sid < 0)
	{
		fprintf( stderr, "Failed to set session id.\n") ;
		exit(EXIT_FAILURE);
	}

	/* Open log. */
	if( !open_log() )
	{
		fprintf( stderr, "Failed to open log.\n") ;
		exit(EXIT_FAILURE);
	}

	/* Parse config file to set global parameters */
	parse_cfg_file();

	/* Parse command line arguments to set global parameters */
	parse_cmd_args( argc, argv );

	/* Close open files inherited from parent. */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Handle signals */
	int i ;
	const int caught_signal[] = { SIGTERM, SIGINT, SIGHUP, 0 };

	for (i = 0; caught_signal[i] != 0; i++)
		signal(caught_signal[i],sighandler);

	/* The big loop */
	while(1)
	{
		sleep(30);
		write_log("PING\n");
		if( server_mode )
			write_log("\trunning in server mode\n");
		else
			write_log("\trunning in client mode\n");
	}

	return 0;
}
