
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <cmdargs.h>

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
