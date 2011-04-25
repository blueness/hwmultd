
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cmdargs.h>
#include <log.h>
#include <pidfile.h>
#include <signalhandler.h>
#include <service.h>

int
main( int argc, char *argv[] )
{
	// Fork into background
	pid_t pid = fork();

	if(pid < 0)
	{
		fprintf( stderr, "Failed to fork.\n") ;
		exit(EXIT_FAILURE);
	}

	if(pid > 0)
		exit(EXIT_SUCCESS);

	pid = getpid() ;

	// Set umask before opening any files
	umask(0);

	// Open log
	if( !open_log() )
	{
		fprintf( stderr, "Failed to open log.\n") ;
		exit(EXIT_FAILURE);
	}

	// Log test before jumping into the big loo
	write_log(CRIT, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(ERRO, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(INFO, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);
	write_log(DBUG, "TESTING - %s %c %d %f", "hello world", 'x', 42, 3.14159);

	// Close open files inherited from parent
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// Create a new SID for the child process
	pid_t sid = setsid();
	if (sid < 0)
	{
		write_log(CRIT, "Failed to set session id.\n") ;
		clean_exit();
	}

        // chdir to root
        if((chdir("/")) < 0)
	{
		write_log(CRIT, "Failed to set session id.\n") ;
		clean_exit();
        }

	// Parse config file to set global parameters
	parse_cfg_file();

	// Parse command line arguments to set global parameters
	parse_cmd_args( argc, argv );

	// Create pid file
	open_pid(pid);

	// Handle signals
	sighandler();

	// Drop privileges
	uid_t uid = pwd->pw_uid ;
	gid_t gid = pwd->pw_gid ;
	setuid(uid) ;
	setgid(gid) ;

	start_service();
}
