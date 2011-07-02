


#include <hwcommon.h>



#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



#undef ME
#define ME "entropy_hw"



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
		return -1;

	if( !(hex = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return -1;

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

		fclose(myfile);
	}

	if((fd = open( dev, O_RDONLY | O_NONBLOCK | O_NOCTTY )) < 0)
		return -2;

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
		return -1;
	else
		return 1;
}
