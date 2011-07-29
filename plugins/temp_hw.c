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
#include <termios.h>
#include <unistd.h>


// This delay seems to work
#define DELAY 15000

int fd ;
char *buf;

int
init_hw()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char dev[CONF_LINE_BUFFER];
	int i;

	uint8_t data[1024];
	struct termios ios;

	// Allocate a buffer for returning a message from read_hw()
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	// Zero the message buffer
	memset(buf, 0, MSG_BUFFER*sizeof(char));

	//TODO - properly compensate for extras past MAX_CONF_{DIR,FILE}_LEN
	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncat(conf_file, __FILE__, strlen(__FILE__) - 2);
	strcat(conf_file, ".conf");

	// Default to ttyUSB0 if no Device is found in the plugin config file
	strcpy(dev, "/dev/ttyUSB0");

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

			// The only recognized keys is Device
			//    Device = the hardware device source of the temperature
			if( !strcmp(first,"Device") )
				strncpy(dev, second, CONF_LINE_BUFFER);
		}

		// We're done, so close the file
		if(fclose(myfile))
			return HW_CLOSE_FILE;
	}
	else
		return HW_OPEN_FILE;

	// Open the hardware device for non-block read/write
	if((fd = open( dev, O_RDWR | O_NONBLOCK | O_NOCTTY )) < 0)
		return HW_OPEN_DEV;

	// This is a USB device that presents itself to usrland as
	// a serial device via the magic of the ftdi_sio module.
	// So we treat it here as if it were an old school modem.
	// See the oldie but goodie: http://www.easysw.com/~mike/serial/serial.html
	if(tcgetattr(fd, &ios) < 0)
	{
		close(fd);
		return HW_GET_DEV_ATTR;
	}

	cfsetispeed(&ios, B9600);
	cfsetospeed(&ios, B9600);
	cfmakeraw  (&ios);
	if(tcsetattr(fd, TCSANOW, &ios) < 0)
	{
		close(fd);
		return HW_SET_DEV_ATTR;
	}

	// Fuzzed this sequence out.  Not sure what "X" or "P" do.
	// "R" seems to mean "Read".
	usleep(DELAY);
	if(write(fd, "X", 1) < 1)
		return HW_WRITE_DEV;

	usleep(DELAY);		// Don't check for error on read since
	read(fd, data, 1024);	// errno=EAGAIN may result from O_NONBLOCK

	usleep(DELAY);
	if(write(fd, "P", 1) < 1)
		return HW_WRITE_DEV;

	usleep(DELAY);
	read(fd, data, 1024);

	usleep(DELAY);
	if(write(fd, "R", 1) < 1)
		return HW_WRITE_DEV;

	usleep(DELAY);
	read(fd, data, 1024);

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


char *
read_hw()
{
	int i;
	uint8_t data[18];
	double temp;

	// Send the "Read" command
	usleep(DELAY);
	if(write(fd, "R", 1) < 1)
		return "NULL WRITE";

	// Read 18 byte response
	usleep(DELAY);
	if(read(fd, data, 18) < 18)
		return "NULL READ";

	// The temp is in the first two non-zero bytes
	// Why does this thing work this way?!
	for(i = 0; i < 18; i++)
		if(data[i])
			break;

	// Put the two bytes together into a temperature
	// The factor 16 appears to correctly scale the temperature
	temp = data[i] + 256.0 * data[i+1];
	temp /= 16.0;

	// Only return the integer part for multicating
	sprintf(buf, "%.0lf", temp);

	return buf;
}


int
close_hw()
{
	// Free the allocated buffer
	free(buf);

	// Close the hardware device
	if(close(fd))
		return HW_CLOSE_DEV;
	else
		return HW_SUCCESS;
}
