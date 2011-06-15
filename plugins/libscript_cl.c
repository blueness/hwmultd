
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clcommon.h>

#define MAX_CONF_DIR_LEN 1024

#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif

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

#define SCRIPT_BUFFER 4096
#define PATH_BUFFER 4096
char *
act_cl(char *msg)
{
	FILE *f;
	char *buf = (char *)malloc(SCRIPT_BUFFER*sizeof(char));
	memset(buf, 0, SCRIPT_BUFFER);

	char script[PATH_BUFFER];

	memset(script, 0, PATH_BUFFER);
	strncpy(script, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(script, "/scripts");
	strcat(script, "/null.sh");

	f = popen(script, "r");
	fread(buf, sizeof(char), 4096, f);
	pclose(f);

	return buf;
}

int
close_cl()
{
	return 1;
}
