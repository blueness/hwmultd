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



#include <errno.h>
#include <linux/random.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



// The hardware device which is the source of random data
#define RNDDEVICE "/dev/random"

// Maximum number of bytes to read per TIMING cycle
#define MAX_NBYTES 32

// Microsecond of delay when resetting the device
#define DELAY 1000

int fd;
char *buf, *hex;

struct rand_pool_info *random_pool;
int size_random_pool;

int
init_cl()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char dev[CONF_LINE_BUFFER];
	int i;

	// Allocate a buffer for returning a message from act_cl()
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return CL_MALLOC;

	// Allocate a buffer for hex digit to random bytes conversion
	if( !(hex = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return CL_MALLOC;

	// The actual size of our rand_pool_info <= size of pointer + max bytes of random data
	size_random_pool = sizeof(struct rand_pool_info *) + MAX_NBYTES*sizeof(uint8_t) ;

	// Allocate a buffer for the rand_pool_info
	if( !(random_pool = (struct rand_pool_info *)malloc(size_random_pool)) )
		return CL_MALLOC;

	//TODO - properly compensate for extras past MAX_CONF_{DIR,FILE}_LEN
	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncat(conf_file, __FILE__, strlen(__FILE__) - 2);
	strcat(conf_file, ".conf");

	strcpy(dev, RNDDEVICE);

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

			// The only recognize key is Device
			if( !strcmp(first, "Device") )
				strncpy(dev, second, CONF_LINE_BUFFER);
		}

		// We're done, so close the file
		if(fclose(myfile))
			return CL_CLOSE_FILE;
	}
	else
		return CL_OPEN_FILE;

	// Open the hardware device for non-block writing
	if((fd = open( dev, O_WRONLY | O_NONBLOCK | O_NOCTTY )) < 0)
		return CL_OPEN_DEV;

	return CL_SUCCESS;
}


int
reset_cl()
{
	int ret;

	// Wait DELAY, close, wait DELAY and reopen

	usleep(DELAY);
	if((ret = close_cl()) != CL_SUCCESS)
		return ret;

	usleep(DELAY);
	if((ret = init_cl()) != CL_SUCCESS)
		return ret;

	return CL_SUCCESS;
}


// Convert hex digits to raw bytes
void
hex_decode(uint32_t *data, int rbytes)
{
	int i, j;
	char *digit[] =
	{
		"0", "1", "2", "3", "4", "5", "6", "7", 
		"8", "9", "A", "B", "C", "D", "E", "F"
	};

	for(i = 0; i < rbytes; i++)
	{
		data[i] = 0;
		for(j = 0; j < 16; j++)
		{
			if( hex[2*i] == digit[j][0] )
				data[i] += 16*j;
			if( hex[2*i+1] == digit[j][0] )
				data[i] += j;
		}
	}
}


char *
act_cl(char *msg)
{
	int i;
	int rbytes;
	uint32_t data[MAX_NBYTES];
	char tmp[8];

	// Zero the allocated buffers
	memset(buf, 0, MSG_BUFFER*sizeof(char));
	memset(hex, 0, MSG_BUFFER*sizeof(char));
	memset(random_pool, 0, size_random_pool);

	// The received should be 
	//    1) number of bytes
	//    2) a string of hex digits
	sscanf(msg,"%u %s", &rbytes, hex) ;
	hex_decode(data, rbytes);

	// Pack the random_pool to write to the random number device
	random_pool->buf_size = rbytes;
	random_pool->entropy_count = rbytes * 8;
	memcpy(random_pool->buf, data, rbytes);

	// Add the entropy to the device
	if(ioctl(fd, RNDADDENTROPY, random_pool) == -1)
	{
		sprintf(buf, "RNDADDENTROPY failed: %d", errno);
		return buf;
	}

	// Return the string of hex digits for logging
	for(i = 0; i < rbytes ; i++)
	{
		sprintf(tmp, " %u", data[i]);
		strcat(buf, tmp);
	}

	return buf;
}

int
close_cl()
{
	// Free the allocated buffers
	free(random_pool);
	free(hex);
	free(buf);

	// Close the hardware device
	if(close(fd))
		return CL_CLOSE_DEV;
	else
		return CL_SUCCESS;
}
