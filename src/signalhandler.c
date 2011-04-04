
#include <stdlib.h>
#include <signal.h>

void
clean_exit()
{
	exit(0) ;
}

void
sighandler(int sig)
{
	switch(sig)
	{
		case SIGHUP:
			break ;
		case SIGTERM:
			break ;
		case SIGINT:
			clean_exit();
			break ;
		default:
			break;
	}
}

