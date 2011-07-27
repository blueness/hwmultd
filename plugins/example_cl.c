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


#include <clcommon.h>
#include "common.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



char *buf;

int
init_cl()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	int i;

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return CL_MALLOC;

	memset(buf, 0, MSG_BUFFER*sizeof(char));
	strcpy(buf, "CL_EXAMPLE_PLUGIN_DEFAULT_MSG");

	//TODO - properly compensate for extras past MAX_CONF_{DIR,FILE}_LEN
	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncat(conf_file, __FILE__, strlen(__FILE__) - 2);
	strcat(conf_file, ".conf");

	if(myfile = fopen(conf_file, "r"))
	{
		while(fgets(conf_line, CONF_LINE_BUFFER, myfile))
		{
			for(i = 0; i < strlen(conf_line); i++)
				if( conf_line[i] == '#' )
				{
					conf_line[i] = 0;
					break;
				}

			if(sscanf(conf_line, "%s %s", first, second ) != 2)
				continue;

			if( !strcmp(first,"Message") )
				strncpy(buf, second, MSG_BUFFER);
		}

		if(fclose(myfile))
			return CL_CLOSE_FILE;
	}
	else
		return CL_OPEN_FILE;

	return CL_SUCCESS;
}

int
reset_cl()
{
	return CL_SUCCESS;
}

char *
act_cl(char *msg)
{
	return buf;
}

int
close_cl()
{
	free(buf);
	return CL_SUCCESS;
}
