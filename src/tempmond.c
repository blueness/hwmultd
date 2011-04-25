
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cmdargs.h>
#include <log.h>
#include <signalhandler.h>
#include <service.h>

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
	sighandler();

	// Log test before jumping into the big loo
	write_log(CRIT, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(ERRO, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(INFO, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(DBUG, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);

	start_service();
}
