
#include <stdio.h>
#include <stdarg.h>

#include <log.h>

FILE *log_stream;
const char *log_file_name = "/var/log/tempmond/tempmond.log";


int
open_log()
{

	if((log_stream = fopen(log_file_name, "a+")) != NULL)
		write_log("%s\n", "Daemon started: hello world") ;
	else
		return 0;
}


int
write_log(const char *fmt,...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);

	if(vfprintf(log_stream, fmt, ap) == 0)
		ret = 0;
	else
		ret = 1;

	if(fflush(log_stream) != 0)
		ret = 0;

	va_end(ap);
	return ret;
}

int
close_log()
{
	if(fclose(log_stream))
                return 1 ;
        else
                return 0 ;
}

