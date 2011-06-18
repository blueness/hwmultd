
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <hwcommon.h>

#undef ME
#define ME "libentropy_hw"



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



#define MAX_CONF_DIR_LEN 1024
#define MAX_CONF_FILE_LEN 1024
#define CONF_LINE_BUFFER 4096

#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif

#define MAX_NBYTES 32
#define DEFAULT_NBYTES 4
#define DELAY 1000

int fd ;
char *buf;
int nbytes;

int
init_hw()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char dev[CONF_LINE_BUFFER];

	unsigned char data[1024];

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strcat(conf_file, ME);
	strcat(conf_file, ".conf");

	strcpy(dev, "/dev/urandom");
	nbytes = DEFAULT_NBYTES;

	if(myfile = fopen(conf_file, "r"))
	{
		while(fgets(conf_line, CONF_LINE_BUFFER, myfile))
		{
			sscanf(conf_line,"%s %s", first, second ) ;
			if( !strcmp(first, "Device") )
				strncpy(dev, second, CONF_LINE_BUFFER);
			if( !strcmp(first, "Bytes") )
			{
				nbytes = atoi(second);
				if(nbytes > MAX_NBYTES)
					nbytes = MAX_NBYTES;
			}
		}

		fclose(myfile);
	}

	if((fd = open( dev, O_RDONLY | O_NONBLOCK | O_NOCTTY )) < 0)
		return -1;

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

uint32_t
dec_encode(unsigned char *data)
{
	int i ;
	uint32_t value = 0, place = 1;

	for(i = 0; i < nbytes; i++ )
	{
		value += place*data[i] ;
		place <<= 1;
	}
	return value;
}

char *
read_hw()
{
	unsigned int i, rbytes;
	unsigned char data[MAX_NBYTES];
	uint32_t value;

	memset( data, 0, MAX_NBYTES*sizeof(char) );

	rbytes = 0;
	for(;;)
	{
		usleep(DELAY);
		if((rbytes = read(fd, data, nbytes)) != -1)
			break;
	}

	value = dec_encode(data);

	sprintf(buf, "%u %u", rbytes, value);

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
