
#include <stdlib.h>
#include <signal.h>
#include <log.h>

void
clean_exit()
{
	write_log("%s\n", "Ciao, adios, I'm out of here!");
	close_log();
	exit(0);
}

void
sighandler(int sig)
{
	switch(sig)
	{
		case SIGHUP:
			write_log("%s\n", "HUP recieved.");
			break;
		case SIGTERM:
			write_log("%s\n", "TERM recieved.");
			clean_exit();
			break;
		case SIGINT:
			write_log("%s\n", "INT recieved.");
			clean_exit();
			break;
		default:
			write_log("%s\n", "UNKNOWN signal recieved recieved.");
			break;
	}
}

