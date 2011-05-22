
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <log.h>
#include <plugins.h>
#include <service.h>
#include <signalhandler.h>


void
clean_exit()
{
	write_log(DBUG, "doing a clean_exit()");
	stop_service();
	if( close_hw != NULL )
		(*close_hw)();
	unload_plugins();
	close_pid();
	close_log();
	exit(EXIT_SUCCESS);
}


void
handle_term()
{
	write_log(INFO, "SIGTERM recieved");
	clean_exit();
}


void
handle_hup()
{
	write_log(INFO, "SIGHUP recieved");
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
		write_log(ERRO, "register SIGTERM failed");
		return 0;
	}
	else
		write_log(DBUG, "registered SIGTERM");


	//SIGHUP
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);

	sa.sa_flags = 0;
	sa.sa_handler = handle_hup;

	if(sigaction(SIGHUP, &sa, NULL) < 0)
	{
		write_log(ERRO, "register SIGHUP failed");
		return 0;
	}
	else
		write_log(DBUG, "registered SIGTHUP");

	return 1;
}
