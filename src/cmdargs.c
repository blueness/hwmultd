
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

#define CONF_LINE_BUFFER 1000


void
sanity_checks(char source_flag)
{
	struct hostent *host;
	struct in_addr iaddr;

	char source_name[30];

	if(source_flag == CMDLINE)
		strcpy(source_name, "command line");

	if(source_flag == CFGFILE)
		strcpy(source_name, CONFIG_FILE);

	write_log(INFO, ">>>>> Parameters from %s", source_name);

	if( !(host = gethostbyname(site_ip)) )
	{
		write_log(ERRO, "Garbled IP  %s in %s.  Defaulting to %s",
			site_ip, source_name, DEFAULT_SITE_LOCAL_IP );
		strcpy(site_ip, DEFAULT_SITE_LOCAL_IP);
	}

	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		write_log(ERRO, "Non-multicast IP %s in %s.  Defaulting to %s",
			site_ip, source_name, DEFAULT_SITE_LOCAL_IP );
		strcpy(site_ip, DEFAULT_SITE_LOCAL_IP);
	}
	else
		write_log(INFO, "IP          = %s", site_ip);

	if(port < MIN_PORT || MAX_PORT < port)
	{
		write_log(ERRO, "Garbled port %d in %s.  Defaulting to %d",
			port, source_name, DEFAULT_PORT );
		port = DEFAULT_PORT;
	}
	else
		write_log(INFO, "Port        = %d", port);

	if((server_mode != SERVER_MODE ) && (server_mode != CLIENT_MODE))
	{
		write_log(ERRO, "Garbled server mode %d in %s.  Defaulting to %d",
			server_mode, source_name, DEFAULT_SERVER_MODE);
		server_mode = DEFAULT_SERVER_MODE;
	}
	else
		write_log(INFO, "Server Mode = %d", server_mode);

	if( !(pwd = getpwnam(user_name)) )
	{
		write_log(INFO, "No such user name %s in %s.  Defaulting to %s",
			user_name, source_name, DEFAULT_USERNAME);
		strcpy(user_name, DEFAULT_USERNAME);

		if( !(pwd = getpwnam(user_name)) )
		{
			write_log(INFO, "No such user name %s in %s.  Defaulting to %s",
				user_name, source_name, FALLBACK_USERNAME);
			strcpy(user_name, FALLBACK_USERNAME);
		}
	}
	else
		write_log(INFO, "User name   = %s", user_name);

	if( (timing < MIN_TIMING) || (MAX_TIMING < timing) )
	{
		write_log(ERRO, "Garbled time %d in %s. Defaulting to %d",
			timing, source_name, DEFAULT_TIMING);
		timing = DEFAULT_TIMING;
	}
	else
		write_log(INFO, "Timing      = %d", timing);

	if(log_level < CRIT || DBUG < log_level)
	{
		write_log(ERRO, "Garbled log level %d in %s.  Defaulting to %d",
			log_level, source_name, DEFAULT_LOG_LEVEL);
		log_level = DEFAULT_LOG_LEVEL;
	}
	else
		write_log(INFO, "Log Level   = %d", log_level);

	write_log(INFO, "<<<<< Parameters from %s", source_name);
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

	if( !(myfile = fopen(CONFIG_FILE, "r")) )
	{
		write_log(INFO, "No config file found");
		return ;
	}

	while( fgets(conf_line, CONF_LINE_BUFFER, myfile) )
	{
		sscanf(conf_line,"%s %s", first, second ) ;

		if( !strcmp(first,"IP") )
			strcpy(site_ip, second);
		if( !strcmp(first,"Port") )
			port = atoi(second);
		if( !strcmp(first,"Server") )
			server_mode = SERVER_MODE;
		if( !strcmp(first,"User") )
			strcpy(user_name, second);
		if( !strcmp(first,"Timing") )
			timing = atoi(second);
		if(strcmp(first,"Debug") == 0)
			log_level = atoi(second);
	}

	fclose(myfile) ;

	sanity_checks(CFGFILE);
}


void
parse_cmd_args( int argc, char *argv[] )
{
	int oc ;
	while( ( oc = getopt( argc, argv, ":a:p:su:t:d:") ) != -1 )
	{
		switch(oc)
		{
			case 'a':
				strcpy(site_ip, optarg);
				break;
			case 'p':
				port = (uint16_t) atoi(optarg);
				break;
			case 's':
				server_mode = 1;
				break;
			case 'u':
				strcpy(user_name, optarg);
				break;
			case 't':
				timing = atoi(optarg);
				break;
			case 'd':
				log_level = atoi(optarg);
				break;
			case ':':
				fprintf(stderr, "%s: option -%c requires argument\n", argv[0], optopt);
				break;
			case '?':
			default:
				fprintf(stderr, "%s: option -%c is invalid: ignored\n", argv[0], optopt);
		}
	}

	sanity_checks(CMDLINE);
}
