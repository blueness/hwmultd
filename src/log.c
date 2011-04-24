

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <log.h>
#include <cmdargs.h>

FILE *log_stream;
const char *log_file_name = "/var/log/tempmond/tempmond.log";


int
open_log()
{
	log_level = EARLY_LOG_LEVEL;

	if((log_stream = fopen(log_file_name, "a+")) != NULL)
	{
		write_log(CRIT, "START<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<") ;
		return 1;
	}

	return 0;
}


int
write_log(int level, const char *fmt,...)
{
	va_list ap;
	const char *level_name[] = { "CRIT", "ERRO", "INFO", "DBUG" } ;

	time_t t;
	struct tm *tmp;
	char tmstr[TIME_BUFFER];
	char tmstp[TIME_BUFFER];

	if(log_level < level)
		return 1;

	t = time(NULL);
	tmp = localtime(&t);

	if(tmp != NULL)
	{
		if(strftime(tmstr, sizeof(tmstr), "[%d/%b/%Y:%X %z]", tmp) != 0)
			strcpy( tmstp, tmstr ) ;
	}
	else
		strcpy( tmstp, "[NO TIME]" ) ;

	strcat( tmstp, " " ) ;
	strcat( tmstp, level_name[ level ] ) ;

	fprintf(log_stream, "%s: ", tmstp);

	va_start(ap, fmt);
	vfprintf(log_stream, fmt, ap);
	va_end(ap);

	if(fflush(log_stream) != 0)
		return 0;

	fprintf(log_stream, "\n");

	return 1;
}


int
close_log()
{
	if(fclose(log_stream))
                return 1 ;
        else
                return 0 ;
}

