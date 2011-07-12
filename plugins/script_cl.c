


#include <clcommon.h>
#include "common.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



char *buf;
char script[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

int
init_cl()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char script_file[MAX_CONF_FILE_LEN];
	int i;

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return CL_MALLOC;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	strncpy(script, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(script, "/scripts/");

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strncat(conf_file, __FILE__, strlen(__FILE__) - 2);
	strcat(conf_file, ".conf");

	strcpy(script_file, "null.sh");
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

			if( !strcmp(first,"Script") )
				strncpy(script_file, second, MAX_CONF_FILE_LEN);
		}

		fclose(myfile);
	}

	strcat(script, script_file);

	return CL_SUCCESS;
}

int
reset_cl()
{
	return CL_SUCCESS;
}

char *
act_cl(char *msg)
{
	char script_instance[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

	strcpy(script_instance, script);
	strcat(script_instance, " ");
	strncat(script_instance, msg, MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN-strlen(script)-strlen(msg));

	FILE *f = popen(script, "r");
	fread(buf, sizeof(char), 4096, f);
	pclose(f);

	return buf;
//	return script_instance;
}

int
close_cl()
{
	free(buf);
	return CL_SUCCESS;
}
