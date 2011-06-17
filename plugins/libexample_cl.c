
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clcommon.h>

#undef ME
#define ME "libexample_cl"

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



#define MAX_CONF_DIR_LEN 1024
#define MAX_CONF_FILE_LEN 1024
#define CONF_LINE_BUFFER 4096

#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif


char *buf;

int
init_cl()
{
	FILE *myfile;
	char config_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char config_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));
	strcpy(buf, "CL_EXAMPLE_PLUGIN_DEFAULT_MSG");

	strncpy(config_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(config_file, "/");
	strcat(config_file, ME);
	strcat(config_file, ".conf");

	if(myfile = fopen(config_file, "r"))
	{
		while(fgets(config_line, CONF_LINE_BUFFER, myfile))
		{
			sscanf(config_line,"%s %s", first, second ) ;
			if( !strcmp(first,"Message") )
				strncpy(buf, second, MSG_BUFFER);
		}

		fclose(myfile);
	}


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
	return buf;
}

int
close_cl()
{
	free(buf);
	return 1;
}
