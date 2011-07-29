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



#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// Maximum number of bytes to read per TIMING cycle
#define MAX_NBYTES 32

// The default number of bytes to read, if none is given in config file
#define DEFAULT_NBYTES 4

// Microsecond of delay when resetting the device
#define DELAY 1000

int fd ;
char *buf, *hex;
int nbytes;

int
init_hw()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char dev[CONF_LINE_BUFFER];
	int i, tmp_int;

	// Allocate a buffer for returning a message from read_hw()
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	// Allocate a buffer for random bytes to hex digit conversion
	if( !(hex = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	// Zero the message buffer
	memset(buf, 0, MSG_BUFFER*sizeof(char));

	//TODO - properly compensate for extras past MAX_CONF_{DIR,FILE}_LEN
	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncat(conf_file, __FILE__, strlen(__FILE__) - 2);
	strcat(conf_file, ".conf");

	// Default to urandom if no Device is found in the plugin config file
	strcpy(dev, "/dev/urandom");
	nbytes = DEFAULT_NBYTES;

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
			if(sscanf(conf_line,"%s %s", first, second ) != 2)
				continue;

			// The recognized keys is
			//    Device = the hardware device source of random data
			//    Bytes = number of bytes to read from the device per TIMING cycle

			if( !strcmp(first, "Device") )
				strncpy(dev, second, CONF_LINE_BUFFER);

			if( !strcmp(first, "Bytes") )
			{
				//nbytes = atoi(second);
				if(sscanf(second, "%d", &tmp_int) == 1)
				{
					nbytes = tmp_int;
					if(nbytes > MAX_NBYTES)
						nbytes = MAX_NBYTES;
					if(nbytes < 1)
						nbytes = DEFAULT_NBYTES;
				}
			}
		}

		// We're done, so close the file
		if(fclose(myfile))
			return HW_CLOSE_FILE;
	}
	else
		return HW_OPEN_FILE;

	// Open the hardware device for non-block reading
	if((fd = open( dev, O_RDONLY | O_NONBLOCK | O_NOCTTY )) < 0)
		return HW_OPEN_DEV;

	return HW_SUCCESS;
}


int
reset_hw()
{
	int ret;

	// Wait DELAY, close, wait DELAY and reopen

	usleep(DELAY);
	if((ret = close_hw()) != HW_SUCCESS)
		return ret;

	usleep(DELAY);
	if((ret = init_hw()) != HW_SUCCESS)
		return ret;

	return HW_SUCCESS;
}


// Convert raw bytes to hex digits
void
hex_encode(uint8_t *data, int rbytes)
{
	int i ;
	uint8_t low, high;
	char *digit[] =
	{
		"0", "1", "2", "3", "4", "5", "6", "7", 
		"8", "9", "A", "B", "C", "D", "E", "F"
	};

	memset(hex, 0, MSG_BUFFER*sizeof(char));

	for(i = 0; i < rbytes; i++ )
	{
		low = data[i] % 16;
		high = data[i] / 16;
		strcat(hex, digit[low]);
		strcat(hex, digit[high]);
	}
}


char *
read_hw()
{
	int i, rbytes;
	uint8_t data[MAX_NBYTES];

	memset( data, 0, MAX_NBYTES*sizeof(char) );

	// Repeatedly do a non-blocking read until we get some bytes
	rbytes = 0;
	for(;;)
	{
		usleep(DELAY);
		if((rbytes = read(fd, data, nbytes)) != -1)
			break;
	}

	// Encode these raw bytes as hex digits
	hex_encode(data, rbytes);

	// Return the string of hex digits for multicasting
	sprintf(buf, "%u %s", rbytes, hex);

	return buf;
}


int
close_hw()
{
	// Free the allocated buffers
	free(hex);
	free(buf);

	// Close the hardware device
	if(close(fd))
		return HW_CLOSE_DEV;
	else
		return HW_SUCCESS;
}
