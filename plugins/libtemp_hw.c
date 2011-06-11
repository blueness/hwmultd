
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

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
	struct termios ios;

	if( !(fd = open( dev, O_RDWR | O_NONBLOCK | O_NOCTTY )) )
		return -1;

	if( tcgetattr(fd, &ios) < 0 )
	{
		close(fd);
		return -2;
	}

	cfsetispeed(&ios, B9600);
	cfsetospeed(&ios, B9600);
	cfmakeraw  (&ios);
	if( tcsetattr(fd, TCSANOW, &ios) < 0 )
	{
		close(fd);
		return -3;
	}

	if(write(fd, 'X', 1) < 1)
		return -4;
	usleep(DELAY);
	read(fd, data, 1024);

	if(write(fd, 'P', 1) < 1)
		return -5;
	usleep(DELAY);
	read(fd, data, 1024);

	return 1;
}

int
reset_hw()
{
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
	unsigned char data[18];
	double temp;

	if(write(fd, 'R', 1) < 1)
		return "NULL";
	usleep(DELAY);
	if(read(fd, data, 18) < 18)
		return "NULL";

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
		return -1;
	else
		return 1;
}
