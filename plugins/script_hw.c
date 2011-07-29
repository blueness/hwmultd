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
#include "common.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



char *buf;
char script[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

int
init_hw()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char script_file[MAX_CONF_FILE_LEN];
	char *script_subdir = "/scripts/";
	int i;

	// Allocate a buffer for returning a message from read_hw()
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	// Zero the message buffer
	memset(buf, 0, MSG_BUFFER*sizeof(char));

	// Prepare the path the the script directory
	strncpy(script, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN-strlen(script_subdir));
	strcat(script, "/scripts/");

	//TODO - properly compensate for extras past MAX_CONF_{DIR,FILE}_LEN
	// Construct the full path to the plugin config file
	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncat(conf_file, __FILE__, strlen(__FILE__) - 2);
	strcat(conf_file, ".conf");

	// Default to null.sh if no Script is found in the plugin config file
	strcpy(script_file, "null.sh");

	// Open the plugin config file for reading
	if(myfile = fopen(conf_file, "r"))
	{
		// Read one line at a time
		while(fgets(conf_line, CONF_LINE_BUFFER, myfile))
		{
			// Don't parse anything past #, so we'll just zero it
			for(i = 0; i < strlen(conf_line); i++)
				if(conf_line[i] == '#')
				{
					conf_line[i] = 0;
					break;
				}

			// Read the key-value pairs
			if(sscanf(conf_line, "%s %s", first, second ) != 2)
				continue;

			// The only recognize key is Script which names the script to use
			if( !strcmp(first,"Script") )
				strncpy(script_file, second, MAX_CONF_FILE_LEN);
		}

		// We're done, so close the file
		if(fclose(myfile))
			return HW_CLOSE_FILE;
	}
	else
		return HW_OPEN_FILE;

	// Append the script file to the full path
	strcat(script, script_file);

	return HW_SUCCESS;
}


// Resetting is automatic.  Or should we should have some
// stronger standard for what scripts do?
int
reset_hw()
{
	return HW_SUCCESS;
}


char *
read_hw()
{
	// Run the script and pipe back the output to be returned for multicasting
	//TODO - What if this fails?
	FILE *f = popen(script, "r");
	memset(buf, 0, sizeof(buf));
	fread(buf, sizeof(char), 4096, f);
	pclose(f);

	return buf;
}


int
close_hw()
{
	// Free the allocated buffer
	free(buf);

	return HW_SUCCESS;
}
