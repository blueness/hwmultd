


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



#define MAX_NBYTES 32
#define DEFAULT_NBYTES 4
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

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	if( !(hex = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	//TODO - properly compensate for extras past MAX_CONF_{DIR,FILE}_LEN
	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncat(conf_file, __FILE__, strlen(__FILE__) - 2);
	strcat(conf_file, ".conf");

	strcpy(dev, "/dev/urandom");
	nbytes = DEFAULT_NBYTES;

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

			if(sscanf(conf_line,"%s %s", first, second ) != 2)
				continue;

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

		if(fclose(myfile))
			return HW_CLOSE_FILE;
	}
	else
		return HW_OPEN_FILE;

	if((fd = open( dev, O_RDONLY | O_NONBLOCK | O_NOCTTY )) < 0)
		return HW_OPEN_DEV;

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

	rbytes = 0;
	for(;;)
	{
		usleep(DELAY);
		if((rbytes = read(fd, data, nbytes)) != -1)
			break;
	}

	hex_encode(data, rbytes);

	sprintf(buf, "%u %s", rbytes, hex);

	return buf;
}

int
close_hw()
{
	free(hex);
	free(buf);
	if(close(fd))
		return HW_CLOSE_DEV;
	else
		return HW_SUCCESS;
}
