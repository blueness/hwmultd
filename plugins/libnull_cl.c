
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
	return "CL NULL PLUGIN";
}

int
close_cl()
{
	return 1;
}
