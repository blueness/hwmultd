
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <hwcommon.h>

#define TIME_BUFFER 1000


void my_init()
{
	return ;
}

void my_fini()
{
	return ;
}



char *tmstr ;

int
init_hw()
{
	tmstr = (char *)malloc(TIME_BUFFER*sizeof(char));
	return 1;
}


int
reset_hw()
{
	return 1;
}



char *read_hw()
{
	time_t t;
	struct tm *tmp;

	memset(tmstr, 0, TIME_BUFFER*sizeof(char));

	t = time(NULL);
	tmp = localtime(&t);

	if(tmp != NULL)
	{
		if(!strftime(tmstr, TIME_BUFFER*sizeof(char), "[%d/%b/%Y:%X %z]", tmp))
			strcpy( tmstr, "[NO TIME]" ) ;
	}
	else
		strcpy( tmstr, "[NO TIME]" ) ;

	return tmstr;
}

int
close_hw()
{
	free(tmstr);
	return 1;
}
