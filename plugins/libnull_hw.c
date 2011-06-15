
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
	char *buf = (char *)malloc(MSG_BUFFER*sizeof(char)) ;
	memset(buf, 0, MSG_BUFFER*sizeof(char));
	strcpy(buf, "HW NULL PLUGIN");
	return buf;
}

int
close_hw()
{
	return 1;
}
