


#include "cmdargs.h"
#include "log.h"



#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>



#undef ME
#define ME "log.c"



#define TIME_BUFFER 1000

FILE *log_stream;


int
open_log()
{
	log_level = EARLY_LOG_LEVEL;

	if( !(log_stream = fopen(LOG_FILE, "a+")) )
		return 0;

	write_log(CRIT, ME, "START>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>") ;
	return 1;
}


int
write_log(int level, const char *code, const char *fmt,...)
{
	va_list ap;
	const char *level_name[] = { "CRIT", "ERRO", "INFO", "DBUG" } ;

	time_t t;
	struct tm *tmp;
	char tmstr[TIME_BUFFER];
	char tmstp[TIME_BUFFER];

	if(log_level < level)
		return 0;

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
	fprintf(log_stream, "%s: ", code );

	va_start(ap, fmt);
	vfprintf(log_stream, fmt, ap);
	va_end(ap);

	fprintf(log_stream, "\n");

	if(fflush(log_stream) != 0)
		return 0;

	return 1;
}


int
close_log()
{
	write_log(CRIT, ME, "EXITING<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<") ;
	if(fclose(log_stream))
                return 0 ;
        else
                return 1 ;
}
