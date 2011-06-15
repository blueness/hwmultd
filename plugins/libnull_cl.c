
#include <string.h>

#include <clcommon.h>

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



char *buf;

int
init_cl()
{
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;
	
	memset(buf, 0, MSG_BUFFER*sizeof(char));
	strcpy(buf, "CL NULL PLUGIN");

	return 1;
}

int
reset_cl()
{
	return 1;
}

char *
act_cl(char *msg)
{
	return buf;
}

int
close_cl()
{
	free(buf);
	return 1;
}
