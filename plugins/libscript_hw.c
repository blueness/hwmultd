
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hwcommon.h>

#undef ME
#define ME "libscript_hw"

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



#define MAX_CONF_DIR_LEN 1024
#define MAX_CONF_FILE_LEN 1024
#define CONF_LINE_BUFFER 4096

#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif

char *buf;
char script[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

int
init_hw()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char script_file[MAX_CONF_FILE_LEN];

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	strncpy(script, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(script, "/scripts/");

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strcat(conf_file, ME);
	strcat(conf_file, ".conf");

	strcpy(script_file, "null.sh");
	if(myfile = fopen(conf_file, "r"))
	{
		while(fgets(conf_line, CONF_LINE_BUFFER, myfile))
		{
			sscanf(conf_line,"%s %s", first, second ) ;
			if( !strcmp(first,"Script") )
				strncpy(script_file, second, MAX_CONF_FILE_LEN);
		}

		fclose(myfile);
	}

	strcat(script, script_file);

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
	FILE *f = popen(script, "r");
	fread(buf, sizeof(char), 4096, f);
	pclose(f);

	return buf;
}

int
close_hw()
{
	free(buf);
	return 1;
}
