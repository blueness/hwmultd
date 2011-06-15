
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <hwcommon.h>


void my_init()
{
	return ;
}

void my_fini()
{
	return ;
}




int
init_hw()
{
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

	char *tmstr = (char *)malloc(MSG_BUFFER*sizeof(char));
	memset(tmstr, 0, MSG_BUFFER*sizeof(char));

	t = time(NULL);
	tmp = localtime(&t);

	if(tmp != NULL)
	{
		if(!strftime(tmstr, MSG_BUFFER*sizeof(char), "[%d/%b/%Y:%X %z]", tmp))
			strcpy( tmstr, "[NO TIME]" ) ;
	}
	else
		strcpy( tmstr, "[NO TIME]" ) ;

	return tmstr;
}

int
close_hw()
{
	return 1;
}
