
#include <stdlib.h>
#include <signal.h>
#include <log.h>

void
clean_exit()
{
	write_log(CRIT, "EXITING>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	if(server_mode == SERVER_MODE)
		mclient_stop();
	else
		mclient_start();

	close_log();
	exit(0);
}

void
sighandler(int sig)
{
	switch(sig)
	{
		case SIGHUP:
			write_log(INFO, "%s\n", "HUP recieved.");
			break;
		case SIGTERM:
			write_log(INFO, "%s\n", "TERM recieved.");
			clean_exit();
			break;
		case SIGINT:
			write_log(INFO, "%s\n", "INT recieved.");
			clean_exit();
			break;
		default:
			write_log(INFO, "%s\n", "UNKNOWN signal recieved recieved.");
			break;
	}
}
