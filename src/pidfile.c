
#include <stdio.h>
#include <unistd.h>

#include <log.h>
#include <pidfile.h>


void
open_pid(int pid)
{
	FILE *fd = fopen(PID_FILE, "w");
	if(!fd)
	{
		write_log(CRIT, "Can't open pid file, bailing.");
		clean_exit();
	}
        fprintf(fd, "%d\n", pid);
        fclose(fd) ;
}


void
close_pid()
{
	unlink(PID_FILE) ;
}
