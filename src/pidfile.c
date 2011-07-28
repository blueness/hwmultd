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


#include "log.h"
#include "pidfile.h"



#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>



int
dir_pid(uid_t uid, gid_t gid)
{
	int need_mkdir;
	struct stat buf;

	// Assume we do not need to mkdir PID_DIR until proven otherwise
	need_mkdir = 0;

	// Is there anything at PID_DIR, directory or otherwise?
	if( !stat(PID_DIR, &buf) )
	{
		// Is the something at PID_DIR a directory?
		if(!S_ISDIR(buf.st_mode))
		{
			// No it isn't, but this name belongs to us!
			// We'll unlink it and note that we need to mkdir PID_DIR
			need_mkdir = 1;

			if( unlink(PID_DIR) )
			{
				write_log(ERRO, __FILE__, "unlink non-directory at %s failed", PID_DIR);
				return 0;
			}
			else
				write_log(DBUG, __FILE__, "unlinked non-directory at %s", PID_DIR);
		}
		else
			// The directory is already there, so there is nothing to be done
			write_log(DBUG, __FILE__, "pid file directory already exists: %s", PID_DIR);
	}
	else
		// There is nothing at PID_DIR, so we note that we need to mkdir PID_DIR
		need_mkdir = 1 ;

	// Do we need to make the directory?
	if( need_mkdir )
	{
		// If so, make it
		if( mkdir(PID_DIR, 0755) )
		{
			write_log(ERRO, __FILE__, "make pid dir failed: %s", PID_DIR);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "made pid dir: %s", PID_DIR);
	}

	// Whether we just made the directory or it was already there, let's make sure it belongs to us
	if( chown(PID_DIR, uid, gid) )
	{
		write_log(ERRO, __FILE__, "chown %d %d pid dir failed: %s", (int)uid, (int)gid, PID_DIR);
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "chowed %d %d pid dir: %s", (int)uid, (int)gid, PID_DIR);

	return 1;
}


int
open_pid(int pid)
{
	FILE *fd;

	// Whether the pid file exists or not, truncate to zero length
	if( !(fd = fopen(PID_FILE, "w")) )
	{
		write_log(ERRO, __FILE__, "open pid file failed: %s", PID_FILE);
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "opened pid file: %s", PID_FILE);

	// Write your PID
	// TODO - should I check if this fails?  Unlikely but possible
	fprintf(fd, "%d\n", pid);

	// We're done for now so close the file
	if( fclose(fd) )
	{
		write_log(ERRO, __FILE__, "close pid file failed: %s", PID_FILE);
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "close pid file: %s", PID_FILE);

	return 1;
}


int
close_pid()
{
	// The daemon is exiting, so unlink the pidfile
	if( unlink(PID_FILE) )
	{
		write_log(ERRO, __FILE__, "unlink pid file failed: %s", PID_FILE);
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "unlink pid file: %s", PID_FILE);

	return 1;
}
