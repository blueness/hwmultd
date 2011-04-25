
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pwd.h>

#include <cmdargs.h>
#include <log.h>

void
sanity_checks(char source_flag)
{
	struct hostent *host;
	struct in_addr iaddr;

	char source_name[30];

	if(source_flag == CMDLINE)
		strcpy(source_name, "command line");
	if(source_flag == CFGFILE)
		strcpy(source_name, DEFAULT_CONFIG_FILENAME);
	

	if( !(host = gethostbyname(site_ip)) )
	{
		strcpy(site_ip, DEFAULT_SITE_LOCAL_IP);
		write_log(ERRO, "Garbled IP in %s.  Defaulting to %s",
			source_name, DEFAULT_SITE_LOCAL_IP );
	}

	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		strcpy(site_ip, DEFAULT_SITE_LOCAL_IP);
		write_log(ERRO, "Non-multicast IP in %s.  Defaulting to %s",
			source_name, DEFAULT_SITE_LOCAL_IP );
	}
	else
		write_log(INFO, "IP          = %s", site_ip);

	if(port < MIN_PORT || MAX_PORT < port)
	{
		port = DEFAULT_PORT;
		write_log(ERRO, "Garbled port in %s.  Defaulting to %d",
			source_name, DEFAULT_PORT );
	}
	else
		write_log(INFO, "Port        = %d", port);

	if(log_level < CRIT || DBUG < log_level)
	{
		log_level = DEFAULT_LOG_LEVEL;
		write_log(ERRO, "Garbled log level in %s.  Defaulting to %d",
			source_name, DEFAULT_LOG_LEVEL);
	}
	else
		write_log(INFO, "Log Level   = %d", log_level);

	if((server_mode != SERVER_MODE ) && (server_mode != CLIENT_MODE))
	{
		server_mode = DEFAULT_SERVER_MODE;
		write_log(ERRO, "Garbled server mode in %s.  Defaulting to %d",
			source_name, DEFAULT_SERVER_MODE);
	}
	else
		write_log(INFO, "Server Mode = %d", server_mode);

	if( !(pwd = getpwnam(user_name)) )
	{
		strcpy(user_name, DEFAULT_USERNAME);
		write_log(ERRO, "No such user name in %s.  Defaulting to %s",
			source_name, DEFAULT_USERNAME);

		if( !(pwd = getpwnam(user_name)) )
		{
			strcpy(user_name, FALLBACK_USERNAME);
			write_log(ERRO, "No such user name in %s.  Defaulting to %s",
				source_name, FALLBACK_USERNAME);
		}
	}
	else
		write_log(INFO, "User name = %s", user_name);
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

	if( !(myfile = fopen(DEFAULT_CONFIG_FILENAME,"r")) )
	{
		write_log(INFO, "No config file found");
		return ;
	}

	while( fgets(conf_line,CONF_LINE_BUFFER,myfile) )
	{
		sscanf(conf_line,"%s %s", first, second ) ;

		if(strcmp(first,"IP") == 0)
			strcpy(site_ip, second);
		if(strcmp(first,"Port") == 0)
			port = atoi(second);
		if(strcmp(first,"Debug") == 0)
			log_level = atoi(second);
		if(strcmp(first,"Server") == 0)
			server_mode = SERVER_MODE;
		if(strcmp(first,"User") == 0)
			strcpy(user_name, second);
	}

	fclose(myfile) ;

	sanity_checks(CFGFILE);
}


void
parse_cmd_args( int argc, char *argv[] )
{
	int oc ;
	while( ( oc = getopt( argc, argv, ":a:p:d:su:") ) != -1 )
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
			case 'u':
				strcpy(user_name, optarg);
				break;
			case ':':
				fprintf( stderr, "%s: option -%c requires argument\n", argv[0], optopt ) ;
				break ;
			case '?':
			default:
				fprintf( stderr, "%s: option -%c is invalid: ignored\n", argv[0], optopt ) ;
		}
	}

	sanity_checks(CMDLINE);
}
