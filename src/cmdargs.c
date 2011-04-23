
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <log.h>
#include <cmdargs.h>

void
sanity_checks()
{
	// No sanity check for IP.  That's done in mserver.c
	if(port < MIN_PORT || MAX_PORT < port)
	{
		port = DEFAULT_PORT;
		write_log(ERRO, "Garbled port in %s.  Defaulting to %d.\n",
			DEFAULT_CONFIG_FILENAME, DEFAULT_PORT );
	}

	if(log_level < CRIT || DBUG < log_level)
	{
		log_level = DEFAULT_LOG_LEVEL;
		write_log(ERRO, "Garbled log level in %s.  Defaulting to %d.\n",
			DEFAULT_CONFIG_FILENAME, DEFAULT_LOG_LEVEL);
	}

	if((server_mode != SERVER_MODE ) && (server_mode != CLIENT_MODE))
	{
		server_mode = DEFAULT_SERVER_MODE;
		write_log(ERRO, "Garbled server mode in %s.  Defaulting to %d.\n",
			DEFAULT_CONFIG_FILENAME, DEFAULT_SERVER_MODE);
	}
}


void
parse_cfg_file()
{
	FILE *myfile;
	char conf_line[CONF_LINE_BUFFER];
	char first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];

	strcpy(site_ip, DEFAULT_SITE_LOCAL_IP);
	port          = DEFAULT_PORT;
	log_level     = DEFAULT_LOG_LEVEL;
	server_mode   = DEFAULT_SERVER_MODE ;

	if( (myfile = fopen(DEFAULT_CONFIG_FILENAME,"r")) == NULL )
	{
		write_log(INFO, "No config file found.\n");
		return ;
	}

	while( fgets(conf_line,CONF_LINE_BUFFER,myfile) != NULL )
	{
		sscanf(conf_line,"%s %s", first, second ) ;

		if(strcmp(first,"IP") == 0)
			strcpy(site_ip, second);
		if(strcmp(first,"Port") == 0)
			port = atoi(second);
		if(strcmp(first,"Debug") == 0)
			log_level = atoi(second);
		if(strcmp(first,"Server") == 0)
			server_mode = DEFAULT_SERVER_MODE;
	}

	fclose(myfile) ;

	sanity_checks();
}


void
parse_cmd_args( int argc, char *argv[] )
{
	int oc ;
	while( ( oc = getopt( argc, argv, ":p:d:s") ) != -1 )
	{
		switch(oc)
		{
			case 'a':
				strcpy(site_ip, optarg);
				break;
			case 'p':
				port = (uint16_t) atoi(optarg);
				break;
			case 'd':
				log_level = atoi(optarg);
				break;
			case 's':
				server_mode = 1;
				break;
			case ':':
				fprintf( stderr, "%s: option -%c requires argument\n", argv[0], optopt ) ;
				break ;
			case '?':
			default:
				fprintf( stderr, "%s: option -%c is invalid: ignored\n", argv[0], optopt ) ;
		}
	}

	sanity_checks();
}
