
#include <stdlib.h>
#include <signal.h>

#include <cmdargs.h>
#include <log.h>
#include <service.h>

void
clean_exit()
{
	stop_service();
	close_pid();
	close_log();
	exit(0);
}


void
handle_term()
{
	write_log(INFO, "TERM recieved.");
	clean_exit();
}


void
handle_hup()
{
	write_log(INFO, "HUP recieved.");
	parse_cfg_file();
	stop_service();
	start_service();
}


int
sighandler()
{
	struct sigaction sa;

	sa.sa_handler = handle_term;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sa.sa_flags = 0;
	if (sigaction(SIGTERM, &sa, NULL) < 0)
	{
		write_log(ERRO, "register SIGTERM failed");
		return 0;
	}
	else
		write_log(DBUG, "registered SIGTERM");

	sa.sa_handler = handle_hup;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0)
	{
		write_log(ERRO, "register SIGHUP failed");
		return 0;
	}
	else
		write_log(DBUG, "registered SIGTHUP");

	return 1;
}
