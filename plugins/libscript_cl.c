
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clcommon.h>

#ifndef DEFAULT_SCRIPTS_DIR
#define DEFAULT_SCRIPTS_DIR "/usr/local/etc/hwmultd/scripts"
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
	//char *script = (char *)malloc(SCRIPT_BUFFER*sizeof(char));
	//memset(buf, 0, PATH_BUFFER);

	memset(script, 0, PATH_BUFFER);
	strncpy(script, DEFAULT_SCRIPTS_DIR, strlen(DEFAULT_SCRIPTS_DIR));
	strncat(script, "/", 1);
	strncat(script, "null.sh", 7);

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
