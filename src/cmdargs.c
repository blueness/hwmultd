
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

	if( !(host = gethostbyname(multicast_ip)) )
	{
		write_log(ERRO, "bad multicast IP %s in %s.  Defaulting to %s",
			multicast_ip, source_name, DEFAULT_MULTICAST_IP );
		strcpy(multicast_ip, DEFAULT_MULTICAST_IP);
	}
	else
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
		{
			write_log(ERRO, "non-multicast IP %s in %s.  Defaulting to %s",
				multicast_ip, source_name, DEFAULT_MULTICAST_IP );
			strcpy(multicast_ip, DEFAULT_MULTICAST_IP);
		}
		else
			write_log(INFO, "mulitcast IP = %s", multicast_ip);
	}

	if(port < MIN_PORT || MAX_PORT < port)
	{
		write_log(ERRO, "bad port %d in %s.  Defaulting to %d",
			port, source_name, DEFAULT_PORT );
		port = DEFAULT_PORT;
	}
	else
		write_log(INFO, "Port         = %d", port);

	if((server_mode != SERVER_MODE ) && (server_mode != CLIENT_MODE))
	{
		write_log(ERRO, "bad server mode %d in %s.  Defaulting to %d",
			server_mode, source_name, DEFAULT_SERVER_MODE);
		server_mode = DEFAULT_SERVER_MODE;
	}
	else
		write_log(INFO, "Server Mode  = %d", server_mode);

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
			pwd = getpwnam(user_name);
		}
	}
	else
		write_log(INFO, "User name    = %s", user_name);

	if( (timing < MIN_TIMING) || (MAX_TIMING < timing) )
	{
		write_log(ERRO, "bad time %d in %s.  Defaulting to %d",
			timing, source_name, DEFAULT_TIMING);
		timing = DEFAULT_TIMING;
	}
	else
		write_log(INFO, "Timing       = %d", timing);

	if( !(host = gethostbyname(interface_ip)) )
		write_log(INFO, "bad interface IP %s in %s ... skipping",
			interface_ip, source_name );
	else
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_CLASSA(ntohl(iaddr.s_addr)) && !IN_CLASSB(ntohl(iaddr.s_addr))
				&& !IN_CLASSC(ntohl(iaddr.s_addr)) )
			write_log(INFO, "non-class A, B or C IP %s in %s ... skipping");
		else
			write_log(INFO, "interface IP = %s", multicast_ip);
	}

	//TODO - sanity check for interface

	if(log_level < CRIT || DBUG < log_level)
	{
		write_log(ERRO, "bad log level %d in %s.  Defaulting to %d",
			log_level, source_name, DEFAULT_LOG_LEVEL);
		log_level = DEFAULT_LOG_LEVEL;
	}
	else
		write_log(INFO, "Log Level    = %d", log_level);

	write_log(INFO, "<<<<< Parameters from %s", source_name);
}


void
parse_cfg_file()
{
	// Start with default values and let config file override
	strcpy(multicast_ip, DEFAULT_MULTICAST_IP);
	port = DEFAULT_PORT;
	server_mode = DEFAULT_SERVER_MODE;
	strcpy(user_name, DEFAULT_USERNAME);
	timing = DEFAULT_TIMING;
	strcpy(interface_ip, DEFAULT_INTERFACE_IP);
	//strcpy(interface_name, DEFAULT_INTERFACE_NAME);
	log_level = DEFAULT_LOG_LEVEL;

	FILE *myfile;
	char conf_line[CONF_LINE_BUFFER];
	char first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];

	strcpy(multicast_ip, DEFAULT_MULTICAST_IP);
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

		if( !strcmp(first,"MulticastIP") )
			strcpy(multicast_ip, second);
		if( !strcmp(first,"Port") )
			port = atoi(second);
		if( !strcmp(first,"Server") )
			server_mode = SERVER_MODE;
		if( !strcmp(first,"User") )
			strcpy(user_name, second);
		if( !strcmp(first,"Timing") )
			timing = atoi(second);
		if( !strcmp(first,"SourceIP") )
			strcpy(interface_ip, second);
		if( !strcmp(first,"Interface") )
			strcpy(interface_name, second);
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
	while( ( oc = getopt( argc, argv, ":m:p:su:t:i:a:d:") ) != -1 )
	{
		switch(oc)
		{
			case 'm':
				strcpy(multicast_ip, optarg);
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
			case 'a':
				strcpy(interface_ip, optarg);
				break;
			case 'i':
				strcpy(interface_name, optarg);
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
