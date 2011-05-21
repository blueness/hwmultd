
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <log.h>
#include <pidfile.h>


int
dir_pid(uid_t uid, gid_t gid)
{
	int need_mkdir;
	struct stat buf;

	// Assume we do not need to mkdir PID_DIR until proven otherwise
	need_mkdir = 0;

	if( !stat(PID_DIR, &buf) )
	{
		// something exists at PID_DIR
		if(!S_ISDIR(buf.st_mode))
		{
			// its a file/symlink/device/fifo/socket
			// unlink it and we'll need to mkdir PID_DIR
			need_mkdir = 1;

			if( unlink(PID_DIR) )
			{
				write_log(ERRO,"unlink non-directory at %s failed", PID_DIR);
				return 0;
			}
			else
				write_log(DBUG,"unlinked non-directory at %s", PID_DIR);
		}
		else
			// it is a directory
			write_log(DBUG,"pid file directory already exists: %s", PID_DIR);
	}
	else
		// nothing exists at PID_DIR
		need_mkdir = 1 ;

	if( need_mkdir )
	{
		if( mkdir(PID_DIR, 0755) )
		{
			write_log(ERRO,"make pid dir failed: %s", PID_DIR);
			return 0;
		}
		else
			write_log(DBUG,"made pid dir: %s", PID_DIR);
	}

	if( chown(PID_DIR, uid, gid) )
	{
		write_log(ERRO,"chown %d %d pid dir failed: %s", (int)uid, (int)gid, PID_DIR);
		return 0;
	}
	else
		write_log(DBUG,"chowed %d %d pid dir: %s", (int)uid, (int)gid, PID_DIR);

	return 1;
}


int
open_pid(int pid)
{
	FILE *fd;

	// Whether the pid file exists or not, truncate to zero length
	if( !(fd = fopen(PID_FILE, "w")) )
	{
		write_log(ERRO,"open pid file failed: %s", PID_FILE);
		return 0;
	}
	else
		write_log(DBUG,"opened pid file: %s", PID_FILE);

	fprintf(fd, "%d\n", pid);

	if( fclose(fd) )
	{
		write_log(ERRO,"close pid file failed: %s", PID_FILE);
		return 0;
	}
	else
		write_log(DBUG,"close pid file: %s", PID_FILE);

	return 1;
}


int
close_pid()
{
	if( unlink(PID_FILE) )
	{
		write_log(ERRO,"unlink pid file failed: %s", PID_FILE);
		return 0;
	}
	else
		write_log(DBUG,"unlink pid file: %s", PID_FILE);

	return 1;
}
