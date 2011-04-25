
#include <stdio.h>
#include <unistd.h>

#include <log.h>
#include <pidfile.h>

void
pid_create(int pid)
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
pid_destroy()
{
	unlink(PID_FILE) ;
}
