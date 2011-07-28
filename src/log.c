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



#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>



#define TIME_BUFFER 1000

FILE *log_stream;


int
open_log()
{
	// We may not know our log level, but we need one, so default
	log_level = EARLY_LOG_LEVEL;

	// Open the log file, which may already exit, for writing
	// Append, do not truncate
	if( !(log_stream = fopen(LOG_FILE, "a+")) )
		return 0;

	write_log(CRIT, __FILE__, "START>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>") ;
	return 1;
}


int
write_log(int level, const char *code, const char *fmt,...)
{
	// TODO - level_name should probably go into the log.h
	va_list ap;
	const char *level_name[] = { "CRIT", "ERRO", "INFO", "DBUG" } ;

	time_t t;
	struct tm *tmp;
	char tmstr[TIME_BUFFER];
	char tmstp[TIME_BUFFER];

	// Are we being asked to log beyond our log level?  If so, log nothing
	if(log_level < level)
		return 0;

	// Prepare time stamp
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

	// Write the timestamp + the source code file to the log
	fprintf(log_stream, "%s: ", tmstp);
	fprintf(log_stream, "%s: ", code );

	// Write the variadic message parameters, formatted according to fmt
	va_start(ap, fmt);
	vfprintf(log_stream, fmt, ap);
	va_end(ap);

	// We need to guarantee that each log line is new line terminated
	// otherwise the log becomes a mess.  Also, we don't want to put
	// "\n" in ever write_log throughout the code
	fprintf(log_stream, "\n");

	// We want to see the log line written immediately, don't wait for the OS
	if(fflush(log_stream) != 0)
		return 0;

	return 1;
}


int
close_log()
{
	write_log(CRIT, __FILE__, "EXITING<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<") ;
	if(fclose(log_stream))
                return 0 ;
        else
                return 1 ;
}
