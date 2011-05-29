
#include <hwcommon.h>

void my_init()
{
	printf("Initialize libprint.\n"); 
}

void my_fini()
{
	printf("Finalize libprint.\n"); 
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
	return "NULL PLUGIN";
}

int
close_hw()
{
	return 1;
}
