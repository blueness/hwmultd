
#include <stdlib.h>
#include <signal.h>

#include <cmdargs.h>
#include <log.h>
#include <service.h>

void
clean_exit()
{
	stop_service();
	write_log(CRIT, "EXITING>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	close_log();
	exit(0);
}

void
sighandler(int sig)
{
	switch(sig)
	{
		case SIGHUP:
			write_log(INFO, "HUP recieved.");
			parse_cfg_file();
			stop_service();
			start_service();
			break;
		case SIGTERM:
			write_log(INFO, "TERM recieved.");
			clean_exit();
			break;
		case SIGINT:
			write_log(INFO, "INT recieved.");
			clean_exit();
			break;
		default:
			write_log(INFO, "UNKNOWN signal recieved recieved.");
			break;
	}
}
