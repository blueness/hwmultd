


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



#undef ME
#define ME "entropy_cl"



void
clplugin_init()
{
	return ;
}

void
clplugin_fini()
{
	return ;
}



#define RNDDEVICE "/dev/random"
#define MAX_NBYTES 32
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

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return -1;

	if( !(hex = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return -1;

	size_random_pool = sizeof(struct rand_pool_info *) + MAX_NBYTES*sizeof(uint8_t) ;

	if( !(random_pool = (struct rand_pool_info *)malloc(size_random_pool)) )
		return -1;

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strcat(conf_file, ME);
	strcat(conf_file, ".conf");

	strcpy(dev, RNDDEVICE);

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

			if( !strcmp(first, "Device") )
				strncpy(dev, second, CONF_LINE_BUFFER);
		}

		fclose(myfile);
	}

	if((fd = open( dev, O_WRONLY | O_NONBLOCK | O_NOCTTY )) < 0)
		return -2;

	return 1;
}

int
reset_cl()
{
	usleep(DELAY);
	if(close_cl() != 1)
		return -1;

	usleep(DELAY);
	if(init_cl() != 1)
		return -2;

	return 1;
}

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

	memset(buf, 0, MSG_BUFFER*sizeof(char));
	memset(hex, 0, MSG_BUFFER*sizeof(char));
	memset(random_pool, 0, size_random_pool);

	sscanf(msg,"%u %s", &rbytes, hex) ;
	hex_decode(data, rbytes);

	random_pool->buf_size = rbytes;
	random_pool->entropy_count = rbytes * 8;
	memcpy(random_pool->buf, data, rbytes);

	if(ioctl(fd, RNDADDENTROPY, random_pool) == -1)
	{
		sprintf(buf, "RNDADDENTROPY failed: %d", errno);
		return buf;
	}

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
	free(random_pool);
	free(hex);
	free(buf);
	if(close(fd))
		return -1;
	else
		return 1;
}
