
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

char *
read_hw()
{
	return "NULL PLUGIN";
}

int
close_hw()
{
	return 1;
}
