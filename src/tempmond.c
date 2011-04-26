
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
	pid_t pid; // pid of demonized process
	pid_t sid; // sid of demonized process

	// Fork into background
	if( (pid = fork()) < 0)
	{
		fprintf(stderr, "fork failed\n") ;
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
		fprintf( stderr, "open log failed\n") ;
		exit(EXIT_FAILURE);
	}

	// Close open files inherited from parent
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// Create a new SID for the child process
	if( (sid = setsid()) < 0)
	{
		write_log(CRIT, "set session id failed") ;
		clean_exit();
	}
	else
		write_log(DBUG, "set session id %d", (int)sid) ;

        // chdir to root
        if(chdir("/") < 0)
	{
		write_log(CRIT, "chdir / failed");
		clean_exit();
        }
	else
		write_log(DBUG, "chdir /");

	// Parse config file to set global parameters
	parse_cfg_file();

	// Parse command line arguments to set global parameters
	parse_cmd_args( argc, argv );

	// Handle signals
	if( !sighandler() )
	{
		write_log(CRIT, "register signals failed");
		clean_exit();
	}
	else
		write_log(DBUG, "registered signals");

	// Get my uid and gid
	uid_t uid = pwd->pw_uid ;
	gid_t gid = pwd->pw_gid ;

	// Create pid directory
	if( !dir_pid(uid, gid) )
	{
		write_log(CRIT, "create pid file dir failed");
		clean_exit();
	}
	else
		write_log(DBUG, "created pid file dir");

	// Drop privileges
	setuid(uid) ;
	setgid(gid) ;

	// Create pid file
	if( !open_pid(pid) )
	{
		write_log(CRIT, "create pid file failed");
		clean_exit();
	}
	else
		write_log(DBUG, "created pid file");

	start_service();
}
