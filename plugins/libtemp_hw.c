
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <hwcommon.h>


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




int fd ;

#define DELAY 15000

int
init_hw()
{
	unsigned char data[1024];
	const char *dev = "/dev/ttyUSB0" ;

	if( !(fd = open( dev, O_RDWR | O_NONBLOCK | O_NOCTTY )) )
		return 0;

	if(write(fd, 'X', 1) < 1)
		return 0;
	usleep(DELAY);
	read(fd, data, 1024);

	if(write(fd, 'P', 1) < 1)
		return 0;
	usleep(DELAY);
	read(fd, data, 1024);

	return 1;
}

int
reset_hw()
{
	if(!close_hw())
		return 0;
	usleep(DELAY);
	if(!init_hw())
		return 0;
	return 1;
}

char *
read_hw()
{
	unsigned char data[18];
	double temp;

	if(write(fd, 'R', 1) < 1)
		return 0;
	usleep(DELAY);
	if(read(fd, data, 18) < 18)
		return 0;

	temp = data[9] + 256 * data[10];
	temp /= 16.0;

	char *stemp = (char *)malloc(32);
	sprintf(stemp, "%lf", temp);

	return stemp;
}

int
close_hw()
{
	if(close(fd))
		return 0;
	else
		return 1;
}
