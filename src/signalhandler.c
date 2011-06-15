
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <log.h>
#include <plugins.h>
#include <service.h>
#include <signalhandler.h>


#undef ME
#define ME "signalhandler.c"


void
clean_exit()
{
	write_log(DBUG, ME, "doing a clean_exit()");
	close_pid();
	close_log();
	exit(EXIT_SUCCESS);
}


void
handle_term()
{
	write_log(INFO, ME, "SIGTERM recieved");
	clean_exit();
}


void
handle_hup()
{
	write_log(INFO, ME, "SIGHUP recieved");
	continue_big_loop = 0;
}


int
sighandler()
{
	struct sigaction sa;
	sigset_t block_set;

	// mask all signals except for those we pay attention to
	sigfillset(&block_set);
	sigdelset(&block_set, SIGTERM);
	sigdelset(&block_set, SIGHUP);
	sigprocmask(SIG_BLOCK, &block_set, NULL);

	//SIGTERM
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);

	sa.sa_flags = 0;
	sa.sa_handler = handle_term;

	if (sigaction(SIGTERM, &sa, NULL) < 0)
	{
		write_log(ERRO, ME, "register SIGTERM failed");
		return 0;
	}
	else
		write_log(DBUG, ME, "registered SIGTERM");


	//SIGHUP
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);

	sa.sa_flags = 0;
	sa.sa_handler = handle_hup;

	if(sigaction(SIGHUP, &sa, NULL) < 0)
	{
		write_log(ERRO, ME, "register SIGHUP failed");
		return 0;
	}
	else
		write_log(DBUG, ME, "registered SIGTHUP");

	return 1;
}
