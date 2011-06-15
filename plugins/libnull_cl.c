
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

int
init_cl()
{
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
	char *buf = (char *)malloc(MSG_BUFFER*sizeof(char)) ;
	memset(buf, 0, MSG_BUFFER*sizeof(char));
	strcpy(buf, "CL NULL PLUGIN");
	return buf;
}

int
close_cl()
{
	return 1;
}
