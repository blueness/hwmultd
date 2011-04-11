
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <log.h>

int log_fd;
const char *log_file_name = "/var/log/tempmond.log";


/*
 * Open log before you close stderr otherwise there is not message for the user to read.
 */
int
open_log()
{
	if( log_fd = open( log_file_name, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) )
	{
		write_log("Daemon started: hello world\n") ;
		return 1 ;
	}
	else
	{
		fprintf(stderr, "Can't open log file, bailing.");
		exit(EXIT_FAILURE);
	}
}


int
write_log(const char * msg)
{
	if( write(log_fd, msg, strlen(msg)) )
	{
		return 1 ;
	}
	else
	{
		return 0 ;
	}
}

int
close_log()
{
	if( close(log_fd) )
        {
                return 1 ;
        }
        else
        {
                return 0 ;
        }
}

