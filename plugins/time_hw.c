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


#include <hwcommon.h>



#include <stdlib.h>
#include <string.h>
#include <time.h>



char *buf;

int
init_hw()
{
	// Allocate a buffer for returning a message from read_hw()
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	// Zero the message buffer
	memset(buf, 0, MSG_BUFFER*sizeof(char));

	return HW_SUCCESS;
}


int
reset_hw()
{
	return HW_SUCCESS;
}


char *read_hw()
{
	time_t t;
	struct tm *tmp;

	// Construct a well formed time stamp to be returned for multicasting
	t = time(NULL);
	tmp = localtime(&t);

	if(tmp != NULL)
	{
		if(!strftime(buf, MSG_BUFFER*sizeof(char), "[%d/%b/%Y:%X %z]", tmp))
			strcpy(buf, "[NO TIME]" ) ;
	}
	else
		strcpy(buf, "[NO TIME]" ) ;

	return buf;
}

int
close_hw()
{
	// Free the allocated buffer
	free(buf);

	return HW_SUCCESS;
}
