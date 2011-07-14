


#include <hwcommon.h>
#include "common.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



char *buf;
char script[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

int
init_hw()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	char script_file[MAX_CONF_FILE_LEN];
	char *script_subdir = "/scripts/";
	int i;

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return HW_MALLOC;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	strncpy(script, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN-strlen(script_subdir));
	strcat(script, "/scripts/");

	//TODO - properly compensate for extras past MAX_CONF_{DIR,FILE}_LEN
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

		if(fclose(myfile))
			return HW_CLOSE_FILE;
	}
	else
		return HW_OPEN_FILE;

	strcat(script, script_file);

	return HW_SUCCESS;
}

int
reset_hw()
{
	return HW_SUCCESS;
}

char *
read_hw()
{
	//TODO - What if this fails?
	FILE *f = popen(script, "r");
	memset(buf, 0, sizeof(buf));
	fread(buf, sizeof(char), 4096, f);
	pclose(f);

	return buf;
}

int
close_hw()
{
	free(buf);
	return HW_SUCCESS;
}
