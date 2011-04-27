
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

#include <log.h>
#include <service.h>
#include <signalhandler.h>

void
clean_exit()
{
	if( !worker_mode )
	{
		close_pid();
		close_log();
		exit(EXIT_SUCCESS);
	}
	else
		write_log(ERRO,"worker shouldn't try to clean_exit()");
}


// Shutdown the session lead and worker
void
handle_term()
{
	write_log(INFO, "SIGTERM recieved");

	// The worker should stop_service() and exit()
	if( worker_mode )
	{
		write_log(DBUG,"worker is doing a stop_service()");
		stop_service();
	}
	else
	// The session lead should clean_exit()
	{
		write_log(DBUG,"session leader is doing a clean_exit()");
		kill(wpid,SIGINT);
		clean_exit();
	}
}


// Stop only the worker
void
handle_int()
{
	write_log(INFO, "SIGINT recieved");

	if( worker_mode )
	{
		write_log(DBUG,"worker is doing a stop_service()");
		stop_service();
	}
	else
		write_log(ERRO,"session leader should not act on a SIGINT");
}


// Reread the config file and stop/start the worker
void
handle_hup()
{
	write_log(INFO, "SIGHUP recieved");
	if( !worker_mode )
	{
		parse_cfg_file();
		kill(wpid,SIGINT);
		if( !start_service() )
		{
			write_log(CRIT, "start service failed");
			clean_exit();
		}
		else
			write_log(DBUG, "started service");
	}
	else
		write_log(ERRO,"worker should not act on a SIGHUP");
}


// Reap the child
void
handle_chld()
{
	write_log(INFO, "SIGCHLD received");
}


int
sighandler()
{
	struct sigaction sa;
	sigset_t block_set;

	// mask all signals except for those we pay attention to
	sigfillset(&block_set);
	sigdelset(&block_set, SIGINT);
	sigdelset(&block_set, SIGTERM);
	sigdelset(&block_set, SIGHUP);
	sigdelset(&block_set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &block_set, NULL);

	// SIGINT
	memset(&sa, 0, sizeof(struct sigaction));

	// block other signals while we take care of SIGINT
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGCHLD);

	sa.sa_flags = 0;
	sa.sa_handler = handle_term;

	if (sigaction(SIGTERM, &sa, NULL) < 0)
	{
		write_log(ERRO, "register SIGINT failed");
		return 0;
	}
	else
		write_log(DBUG, "registered SIGINT");

	//SIGTERM
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGCHLD);

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
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGCHLD);

	sa.sa_flags = 0;
	sa.sa_handler = handle_hup;

	if(sigaction(SIGHUP, &sa, NULL) < 0)
	{
		write_log(ERRO, "register SIGHUP failed");
		return 0;
	}
	else
		write_log(DBUG, "registered SIGTHUP");


	//SIGCHLD
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGTERM);

	sa.sa_flags = 0;
	sa.sa_handler = handle_chld;

	if(sigaction(SIGCHLD, &sa, NULL) < 0)
	{
		write_log(ERRO, "register SIGCHLD failed");
		return 0;
	}
	else
		write_log(DBUG, "registered SIGTCHLD");

	return 1;
}
