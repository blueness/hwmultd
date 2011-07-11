


#include "log.h"
#include "plugins.h"
#include "service.h"
#include "signalhandler.h"



#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>



void
clean_exit()
{
	write_log(DBUG, __FILE__, "doing a clean_exit()");
	close_pid();
	close_log();
	exit(EXIT_SUCCESS);
}


void
handle_term()
{
	write_log(INFO, __FILE__, "SIGTERM recieved");
	clean_exit();
}


void
handle_hup()
{
	write_log(INFO, __FILE__, "SIGHUP recieved");
	continue_big_loop = 0;
}


void
handle_usr1()
{
	write_log(INFO, __FILE__, "SIGUSR1 recieved");
	if( !reset_plugins() )
	{
		write_log(CRIT, __FILE__, "reset hardware failed");
		clean_exit();
	}
	else
		write_log(DBUG, __FILE__, "reset hardware");
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
	sigdelset(&block_set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &block_set, NULL);

	//SIGTERM
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGUSR1);

	sa.sa_flags = 0;
	sa.sa_handler = handle_term;

	if (sigaction(SIGTERM, &sa, NULL) < 0)
	{
		write_log(ERRO, __FILE__, "register SIGTERM failed");
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "registered SIGTERM");


	//SIGHUP
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGUSR1);

	sa.sa_flags = 0;
	sa.sa_handler = handle_hup;

	if(sigaction(SIGHUP, &sa, NULL) < 0)
	{
		write_log(ERRO, __FILE__, "register SIGHUP failed");
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "registered SIGHUP");

	//SIGUSR1
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGTERM);

	sa.sa_flags = 0;
	sa.sa_handler = handle_usr1;

	if(sigaction(SIGUSR1, &sa, NULL) < 0)
	{
		write_log(ERRO, __FILE__, "register SIGUSR1 failed");
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "registered SIGUSR1");

	return 1;
}
