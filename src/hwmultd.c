
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cmdargs.h>
#include <log.h>
#include <pidfile.h>
#include <plugins.h>
#include <service.h>
#include <signalhandler.h>

int
main( int argc, char *argv[] )
{
	// Read the command line opts
	parse_cmd_args( argc, argv );

	pid_t pid, sid;		// pid and sid of demonized process
	uid_t uid;		// uid for drop privileges
	gid_t gid;		// gid for drop privileges

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
	umask(022);

	// Open log
	if( !open_log() )
	{
		fprintf(stderr, "open log failed\n") ;
		exit(EXIT_FAILURE);
	}

	// Close open files inherited from parent
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// Read config file
	parse_cfg_file();

	write_log(DBUG, "set process id %d", (int)pid) ;

	// Create a new SID
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

	// Get my uid and gid
	uid = pwd->pw_uid ;
	gid = pwd->pw_gid ;

	// Create pid directory
	if( !dir_pid(uid, gid) )
	{
		write_log(CRIT, "create pid file dir failed");
		clean_exit();
	}
	else
		write_log(DBUG, "created pid file dir");

	// Drop privileges
	if(getuid() == 0)
	{
		if(setgid(gid))
		{
			write_log(CRIT, "drop to gid %d failed", (int)gid);
			clean_exit();
		}
		else
			write_log(DBUG, "droped to gid %d", (int)gid);

		if(setuid(uid))
		{
			write_log(CRIT, "drop to uid %d failed", (int)uid);
			clean_exit();
		}
		else
			write_log(DBUG, "droped to uid %d", (int)uid);
	}

	// Create pid file
	if( !open_pid(pid) )
	{
		write_log(CRIT, "create pid file failed");
		clean_exit();
	}
	else
		write_log(DBUG, "created pid file");

	// Handle signals
	if( !sighandler() )
	{
		write_log(CRIT, "register signals failed");
		clean_exit();
	}
	else
		write_log(DBUG, "registered signals");

	// Load plugins
	if( !load_plugins() )
	{
		write_log(CRIT, "load plugins failed");
		clean_exit();
	}
	else
		write_log(DBUG, "loaded plugins");

	if( !((*init_hw)()) )
	{
		write_log(CRIT, "initialized hardware failed");
		clean_exit();
	}
	else
		write_log(DBUG, "initialized hardware");

	// The big loop
	while(1)
	{
		if( !start_service() )
		{
			write_log(CRIT, "service start failed");
			clean_exit();
		}
		else
			write_log(DBUG, "service started");

		do_service();

		if( !stop_service() )
		{
			write_log(CRIT, "service stop failed");
			clean_exit();
		}
		else
			write_log(DBUG, "service stopped");

		parse_cfg_file();
	}
}
