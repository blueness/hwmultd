
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <cmdargs.h>

void
parse_cfg_file()
{
	FILE *myfile;
	char conf_line[CONF_LINE_BUFFER];
	char first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];

	if( (myfile = fopen(DEFAULT_CONFIG_FILENAME,"r")) == NULL )
	{
		write_log("No config file found");
		port = DEFAULT_PORT ;
		server_mode = DEFAULT_SERVER_MODE ;
		return ;
	}

	while( fgets(conf_line,CONF_LINE_BUFFER,myfile) != NULL )
	{
		sscanf(conf_line,"%s %s", first, second ) ;

		if( strcmp(first,"Port") == 0 )
		{
			port = atoi(second );
		}
		if( strcmp(first,"Server") == 0 )
		{
			server_mode = 1 ;
		}
	}

	fclose(myfile) ;

	if( port == 0 )
	{
		port = 8000;
		write_log("Garbled port found, defaulting.");
	}
}

void
parse_cmd_args( int argc, char *argv[] )
{
	int oc ;
	while( ( oc = getopt( argc, argv, ":p:s") ) != -1 )
	{
		switch(oc)
		{
			case 'p':
				port = (uint16_t) atoi(optarg) ;
				break ;
			case 's':
				server_mode = 1 ;
				break ;
			case ':':
				fprintf( stderr, "%s: option -%c requires argument\n", argv[0], optopt ) ;
				break ;
			case '?':
			default:
				fprintf( stderr, "%s: option -%c is invalid: ignored\n", argv[0], optopt ) ;
		}
	}
}
