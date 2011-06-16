
#include <stdlib.h>
#include <string.h>

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



char *buf;

int
init_hw()
{
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));
	strcpy(buf, "HW EXAMPLE PLUGIN");

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
	return buf;
}

int
close_hw()
{
	free(buf);
	return 1;
}
