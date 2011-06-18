
//#include <linux/random.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <clcommon.h>

#undef ME
#define ME "libentropy_cl"



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



#define MAX_CONF_DIR_LEN 1024
#define MAX_CONF_FILE_LEN 1024
#define CONF_LINE_BUFFER 4096

#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif

#define MAX_NBYTES 32
#define DELAY 1000

char *buf;

int
init_cl()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char dev[CONF_LINE_BUFFER];

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strcat(conf_file, ME);
	strcat(conf_file, ".conf");

	strcpy(dev, "/dev/random");

	if(myfile = fopen(conf_file, "r"))
	{
		while(fgets(conf_line, CONF_LINE_BUFFER, myfile))
		{
			sscanf(conf_line,"%s %s", first, second ) ;
			if( !strcmp(first, "Device") )
				strncpy(dev, second, CONF_LINE_BUFFER);
		}

		fclose(myfile);
	}

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
dec_dencode(unsigned int rbytes, uint32_t value, unsigned char *data)
{
	int i ;

	for(i = 0; i < rbytes; i++)
	{
		data[i] = value % 256;
		value /= 256;
	}
}

char *
act_cl(char *msg)
{
/*	struct rand_pool_info output;
 *	output.buf_size = nbytes;
 *	output.entropy_count = nbytes * 8;
 *	memcpy(output.buf, buffer_rand_datat_source, output.buf_size);
 *	if(ioctl(random_fd, RNDADDENTROPY, output) == -1)
 *		error_exit("RNDADDENTROPY failed!");
 */
	int i;
	unsigned int rbytes;
	uint32_t value;
	unsigned char data[MAX_NBYTES];
	char tmp[100];

	sscanf(msg,"%u %u", &rbytes, &value) ;
	dec_dencode(rbytes, value, data);

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	for(i = 0; i < rbytes; i++)
	{
		sprintf(tmp, " %u", data[i]);
		strcat(buf, tmp);
	}

	return buf;
}

int
close_cl()
{
	free(buf);
	return 1;
}
