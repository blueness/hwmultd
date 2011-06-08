
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

#define MSG_BUFFER 1000
char *
act_cl(char *msg)
{
	char *rmsg;

	rmsg = (char *)malloc(MSG_BUFFER);
	strncpy( rmsg, msg, MSG_BUFFER);

	return rmsg;

	//return "RECV MSG";
}

int
close_cl()
{
	return 1;
}
