
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include <pwd.h>

#include <cmdargs.h>
#include <log.h>


void
sanity_checks(char source_flag)
{
	struct hostent *host;
	struct in_addr iaddr;

        struct ifaddrs *ifa;
        struct sockaddr_in *saddr;

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
		strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	}
	else
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
		{
			write_log(ERRO, "non-multicast IP %s in %s.  Defaulting to %s",
				multicast_ip, source_name, DEFAULT_MULTICAST_IP );
			strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
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
		strncpy(user_name, DEFAULT_USERNAME, UT_NAMESIZE);

		if( !(pwd = getpwnam(user_name)) )
		{
			write_log(INFO, "No such user name %s in %s.  Defaulting to %s",
				user_name, source_name, FALLBACK_USERNAME);
			strncpy(user_name, FALLBACK_USERNAME, UT_NAMESIZE);
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

	if( host = gethostbyname(interface_ip) )
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_CLASSA(ntohl(iaddr.s_addr)) && !IN_CLASSB(ntohl(iaddr.s_addr))
				&& !IN_CLASSC(ntohl(iaddr.s_addr)) )
			write_log(INFO, "non-class A, B or C IP %s in %s ... skipping");
		else
			write_log(INFO, "interface IP = %s", multicast_ip);
	}
	else
		write_log(INFO, "bad interface IP %s in %s ... skipping",
			interface_ip, source_name );

	if( getifaddrs(&ifa) == 0 )
	{
		while( ifa != NULL )
		{
			saddr = (struct sockaddr_in *) ifa->ifa_addr;
			if(saddr->sin_family == AF_INET && !strcmp(ifa->ifa_name, interface_name))
			{
				strncpy(interface_ip, inet_ntoa(saddr->sin_addr), MAX_IP_LEN);
				write_log(DBUG, "Got IP %s from interface %s", interface_name, interface_name);
			}
			ifa=ifa->ifa_next;
		}
	}
	else
		write_log(ERRO, "Can't get IP for interface %s", interface_name);

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


#define CONF_LINE_BUFFER 1024

void
parse_cfg_file()
{
	// Start with default values and let config file override
	strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	port = DEFAULT_PORT;
	server_mode = DEFAULT_SERVER_MODE;
	strncpy(user_name, DEFAULT_USERNAME, UT_NAMESIZE);
	timing = DEFAULT_TIMING;
	strncpy(interface_ip, DEFAULT_INTERFACE_IP, MAX_IP_LEN);
	strncpy(interface_name, DEFAULT_INTERFACE_NAME, MAX_IF_LEN);
	log_level = DEFAULT_LOG_LEVEL;

	FILE *myfile;
	char conf_line[CONF_LINE_BUFFER];
	char first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];

	strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
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
			strncpy(multicast_ip, second, MAX_IP_LEN);
		if( !strcmp(first,"Port") )
			port = atoi(second);
		if( !strcmp(first,"Server") )
			server_mode = SERVER_MODE;
		if( !strcmp(first,"User") )
			strncpy(user_name, second, UT_NAMESIZE);
		if( !strcmp(first,"Timing") )
			timing = atoi(second);
		if( !strcmp(first,"SourceIP") )
			strncpy(interface_ip, second, MAX_IP_LEN);
		if( !strcmp(first,"Interface") )
			strncpy(interface_name, second, MAX_IF_LEN);
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
				strncpy(multicast_ip, optarg, MAX_IP_LEN);
				break;
			case 'p':
				port = (uint16_t) atoi(optarg);
				break;
			case 's':
				server_mode = 1;
				break;
			case 'u':
				strncpy(user_name, optarg, UT_NAMESIZE);
				break;
			case 't':
				timing = atoi(optarg);
				break;
			case 'a':
				strncpy(interface_ip, optarg, MAX_IP_LEN);
				break;
			case 'i':
				strncpy(interface_name, optarg, MAX_IF_LEN);
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
