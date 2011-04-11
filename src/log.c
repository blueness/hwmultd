
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <log.h>

int log_fd;
const char *log_file_name = "/var/log/tempmond.log";


int
open_log()
{
	log_fd = open( log_file_name, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
	if( log_fd >= 0 )
	{
		write_log("Daemon started: hello world\n") ;
		return 1 ;
	}
	else
	{
		return 0 ;
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

