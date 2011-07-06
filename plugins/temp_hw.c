


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



void
hwplugin_init()
{
	return ;
}

void
hwplugin_fini()
{
	return ;
}



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
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncpy(conf_file, __FILE__, strlen(__FILE__) - 2);
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

		fclose(myfile);
	}

	if((fd = open( dev, O_RDWR | O_NONBLOCK | O_NOCTTY )) < 0)
		return -1;

	if(tcgetattr(fd, &ios) < 0)
	{
		close(fd);
		return -2;
	}

	cfsetispeed(&ios, B9600);
	cfsetospeed(&ios, B9600);
	cfmakeraw  (&ios);
	if(tcsetattr(fd, TCSANOW, &ios) < 0)
	{
		close(fd);
		return -3;
	}

	usleep(DELAY);
	if(write(fd, "X", 1) < 1)
		return -4;

	usleep(DELAY);		// Don't check for error on read since
	read(fd, data, 1024);	// errno=EAGAIN may result from O_NONBLOCK

	usleep(DELAY);
	if(write(fd, "P", 1) < 1)
		return -6;

	usleep(DELAY);
	read(fd, data, 1024);

	usleep(DELAY);
	if(write(fd, "R", 1) < 1)
		return -6;

	usleep(DELAY);
	read(fd, data, 1024);

	return 1;
}

int
reset_hw()
{
	usleep(DELAY);
	if(close_hw() != 1)
		return -1;

	usleep(DELAY);
	if(init_hw() != 1)
		return -2;

	return 1;
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

	sprintf(buf, "%lf", temp);

	/*
	buf = (char *)malloc(MSG_BUFFER*sizeof(char));
	sprintf(buf, "%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %lf",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15],
		data[16], data[17], temp);
	*/

	return buf;
}

int
close_hw()
{
	free(buf);
	if(close(fd))
		return -1;
	else
		return 1;
}
