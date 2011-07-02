


#include <hwcommon.h>
#include "common.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#undef ME
#define ME "example_hw"

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
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	int i;

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));
	strcpy(buf, "HW_EXAMPLE_PLUGIN_DEFAULT_MSG");

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strcat(conf_file, ME);
	strcat(conf_file, ".conf");

	if(myfile = fopen(conf_file, "r"))
	{
		while(fgets(conf_line, CONF_LINE_BUFFER, myfile))
		{
			for(i = 0; i < strlen(conf_line); i++)
				if(conf_line[i] == '#')
				{
					conf_line[i] = 0;
					break;
				}

			if(sscanf(conf_line, "%s %s", first, second ) != 2)
				continue;

			if( !strcmp(first,"Message") )
				strncpy(buf, second, MSG_BUFFER);
		}

		fclose(myfile);
	}

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