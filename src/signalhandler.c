
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <log.h>
#include <service.h>
#include <signalhandler.h>


void
handle_int_worker()
{
	write_log(INFO, "worker: SIGINT recieved");
	write_log(DBUG, "worker: doing a stop_service()");
	stop_service();
}


int
sighandler_worker()
{
	struct sigaction sa;
	sigset_t block_set;

	// mask all signals except for those we pay attention to
	sigfillset(&block_set);
	sigdelset(&block_set, SIGINT);
	sigprocmask(SIG_BLOCK, &block_set, NULL);

	//SIGINT
	memset(&sa, 0, sizeof(struct sigaction));

	sa.sa_flags = 0;
	sa.sa_handler = handle_int_worker;

	if (sigaction(SIGINT, &sa, NULL) < 0)
	{
		write_log(ERRO, "worker: register SIGINT failed");
		return 0;
	}
	else
		write_log(DBUG, "worker: registered SIGINT");
}




void
clean_exit()
{
	write_log(DBUG, "pid %d: doing clean_exit() ...", (int)getpid());
	close_pid();
	close_log();
	exit(EXIT_SUCCESS);
}


// Shutdown the session lead and worker
void
handle_term_session_leader()
{
	write_log(INFO, "leader: SIGTERM recieved");

	if( kill(wpid,SIGINT) )
	{
		write_log(CRIT, "leader: SIGINT sent to worker failed");
		clean_exit();
	}
	else
		write_log(DBUG, "leader: SIGINT sent to worker");

	write_log(DBUG,"leader: doing a clean_exit()");
	clean_exit();
}




// Reread the config file and stop/start the worker
void
handle_hup_session_leader()
{
	write_log(INFO, "leader: SIGHUP recieved");

	if( kill(wpid,SIGINT) )
	{
		write_log(CRIT, "leader: SIGINT sent to worker failed");
		clean_exit();
	}
	else
		write_log(DBUG, "leader: SIGINT sent to worker");

	parse_cfg_file();

	if( !start_service() )
	{
		write_log(CRIT, "leader: start service failed");
		clean_exit();
	}
	else
		write_log(DBUG, "leader: started service");
}


// Reap the child
void
handle_chld_session_leader()
{
	write_log(INFO, "leader: SIGCHLD received");
}


int
sighandler_session_leader()
{
	struct sigaction sa;
	sigset_t block_set;

	// mask all signals except for those we pay attention to
	sigfillset(&block_set);
	sigdelset(&block_set, SIGTERM);
	sigdelset(&block_set, SIGHUP);
	sigdelset(&block_set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &block_set, NULL);

	//SIGTERM
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGCHLD);

	sa.sa_flags = 0;
	sa.sa_handler = handle_term_session_leader;

	if (sigaction(SIGTERM, &sa, NULL) < 0)
	{
		write_log(ERRO, "leader: register SIGTERM failed");
		return 0;
	}
	else
		write_log(DBUG, "leader: registered SIGTERM");


	//SIGHUP
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGCHLD);

	sa.sa_flags = 0;
	sa.sa_handler = handle_hup_session_leader;

	if(sigaction(SIGHUP, &sa, NULL) < 0)
	{
		write_log(ERRO, "leader: register SIGHUP failed");
		return 0;
	}
	else
		write_log(DBUG, "leader: registered SIGTHUP");


	//SIGCHLD
	memset(&sa, 0, sizeof(struct sigaction));

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGTERM);

	sa.sa_flags = SA_NOCLDWAIT;
	sa.sa_handler = handle_chld_session_leader;

	if(sigaction(SIGCHLD, &sa, NULL) < 0)
	{
		write_log(ERRO, "leader: register SIGCHLD failed");
		return 0;
	}
	else
		write_log(DBUG, "leader: registered SIGTCHLD");

	return 1;
}
