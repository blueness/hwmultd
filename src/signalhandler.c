
#include <stdlib.h>
#include <signal.h>
#include <log.h>

void
clean_exit()
{
	write_log("Ciao, adios, I'm out of here!\n");
	close_log();
	exit(0);
}

void
sighandler(int sig)
{
	switch(sig)
	{
		case SIGHUP:
			write_log("HUP recieved.\n");
			break;
		case SIGTERM:
			write_log("TERM recieved.\n");
			clean_exit();
			break;
		case SIGINT:
			write_log("INT recieved.\n");
			clean_exit();
			break;
		default:
			write_log("UNKNOWN signal recieved recieved.\n");
			break;
	}
}

