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

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	//TODO - properly compensate for extras past MAX_CONF_{DIR,FILE}_LEN
	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncat(conf_file, __FILE__, strlen(__FILE__) - 2);
	strcat(conf_file, ".conf");

	strcpy(dev, "/dev/ttyUSB0");

	if(myfile = fopen(conf_file, "r"))
	{
		while(fgets(conf_line, CONF_LINE_BUFFER, myfile))
		{
			for(i = 0; i < strlen(conf_line); i++)
				if(conf_line[i] == '#')
				{
					conf_line[i] = 0;
					break;
				}

			if(sscanf(conf_line, "%s %s", first, second ) != 2)
				continue;

			if( !strcmp(first,"Device") )
				strncpy(dev, second, CONF_LINE_BUFFER);
		}

		if(fclose(myfile))
			return HW_CLOSE_FILE;
	}
	else
		return HW_OPEN_FILE;

	if((fd = open( dev, O_RDWR | O_NONBLOCK | O_NOCTTY )) < 0)
		return HW_OPEN_DEV;

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

	usleep(DELAY);
	if(write(fd, "R", 1) < 1)
		return "NULL WRITE";

	usleep(DELAY);
	if(read(fd, data, 18) < 18)
		return "NULL READ";

	for(i = 0; i < 18; i++)
		if(data[i])
			break;

	temp = data[i] + 256.0 * data[i+1];
	temp /= 16.0;

	sprintf(buf, "%.0lf", temp);

	return buf;
}

int
close_hw()
{
	free(buf);
	if(close(fd))
		return HW_CLOSE_DEV;
	else
		return HW_SUCCESS;
}
