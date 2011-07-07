


#include <hwcommon.h>



#include <stdlib.h>
#include <string.h>
#include <time.h>



char *buf;

int
init_hw()
{
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	return HW_SUCCESS;
}


int
reset_hw()
{
	return HW_SUCCESS;
}



char *read_hw()
{
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);

	if(tmp != NULL)
	{
		if(!strftime(buf, MSG_BUFFER*sizeof(char), "[%d/%b/%Y:%X %z]", tmp))
			strcpy(buf, "[NO TIME]" ) ;
	}
	else
		strcpy(buf, "[NO TIME]" ) ;

	return buf;
}

int
close_hw()
{
	free(buf);
	return HW_SUCCESS;
}
