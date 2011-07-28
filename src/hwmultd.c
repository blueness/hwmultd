/*
	This file is part of hwmultd.
	Copyright (C) 2011  Anthony G. Basile

	hwmultd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	hwmultd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "cmdargs.h"
#include "log.h"
#include "pidfile.h"
#include "plugins.h"
#include "service.h"
#include "signalhandler.h"



#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



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

	write_log(DBUG, __FILE__, "set process id %d", (int)pid) ;

	// Create a new SID
	if( (sid = setsid()) < 0)
	{
		write_log(CRIT, __FILE__, "set session id failed") ;
		clean_exit();
	}
	else
		write_log(DBUG, __FILE__, "set session id %d", (int)sid) ;

        // chdir to root
        if(chdir("/") < 0)
	{
		write_log(CRIT, __FILE__, "chdir / failed");
		clean_exit();
        }
	else
		write_log(DBUG, __FILE__, "chdir /");

	// Get my uid and gid
	uid = pwd->pw_uid ;
	gid = pwd->pw_gid ;

	// Create pid directory
	if( !dir_pid(uid, gid) )
	{
		write_log(CRIT, __FILE__, "create pid file dir failed");
		clean_exit();
	}
	else
		write_log(DBUG, __FILE__, "created pid file dir");

	// Drop privileges
	if(getuid() == 0)
	{
		if(setgid(gid))
		{
			write_log(CRIT, __FILE__, "drop to gid %d failed", (int)gid);
			clean_exit();
		}
		else
			write_log(DBUG, __FILE__, "droped to gid %d", (int)gid);

		if(setuid(uid))
		{
			write_log(CRIT, __FILE__, "drop to uid %d failed", (int)uid);
			clean_exit();
		}
		else
			write_log(DBUG, __FILE__, "droped to uid %d", (int)uid);
	}

	// Create pid file
	if( !open_pid(pid) )
	{
		write_log(CRIT, __FILE__, "create pid file failed");
		clean_exit();
	}
	else
		write_log(DBUG, __FILE__, "created pid file");

	// Handle signals
	if( !sighandler() )
	{
		write_log(CRIT, __FILE__, "register signals failed");
		clean_exit();
	}
	else
		write_log(DBUG, __FILE__, "registered signals");

	// The "big" loop, the main daemon loop which cycles
	// through a full startup and shutdown of the service.
	// Each cycle reparse the config file so the daemon can
	// change behavior.
	while(1)
	{
		// Load plugins
		if( !load_plugins() )
		{
			write_log(CRIT, __FILE__, "load plugins failed");
			clean_exit();
		}
		else
			write_log(DBUG, __FILE__, "loaded plugins");

		// Start the daemon service
		if( !start_service() )
		{
			write_log(CRIT, __FILE__, "service start failed");
			clean_exit();
		}
		else
			write_log(DBUG, __FILE__, "service started");

		// Do the service as long as continue_little_loop=1
		// Else we fall out of the loop, or clean_exit
		// This is signal driven
		do_service();

		// Stop the daemon service
		if( !stop_service() )
		{
			write_log(CRIT, __FILE__, "service stop failed");
			clean_exit();
		}
		else
			write_log(DBUG, __FILE__, "service stopped");

		// Unload plugins
		if( !unload_plugins() )
		{
			write_log(CRIT, __FILE__, "unload plugins failed");
			clean_exit();
		}
		else
			write_log(DBUG, __FILE__, "unloaded plugins");

		// Reread the config file, which may have changed
		parse_cfg_file();
	}
}
