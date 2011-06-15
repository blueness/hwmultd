
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clcommon.h>

#define MAX_CONF_DIR_LEN 1024
#define MAX_CONF_FILE_LEN 1024

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


char *buf;
char script[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

int
init_cl()
{
	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	strncpy(script, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(script, "/scripts");
	strcat(script, "/null.sh");

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
	FILE *f = popen(script, "r");
	fread(buf, sizeof(char), 4096, f);
	pclose(f);

	return buf;
}

int
close_cl()
{
	free(buf);
	return 1;
}
