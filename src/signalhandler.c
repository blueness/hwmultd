
#include <stdlib.h>
#include <signal.h>
#include <log.h>

void
clean_exit()
{
	close_log();
	exit(0);
}

void
sighandler(int sig)
{
	switch(sig)
	{
		case SIGHUP:
			break;
		case SIGTERM:
			clean_exit();
			break;
		case SIGINT:
			clean_exit();
			break;
		default:
			break;
	}
}

