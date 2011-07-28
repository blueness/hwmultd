/*
	This file is part of hwmultd.
	Copyright (C) 2011  Anthony G. Basile

	hwmultd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	hwmultd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/


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


// On SIGTERM, log it and cleanly exit
void
handle_term()
{
	write_log(INFO, __FILE__, "SIGTERM recieved");
	clean_exit();
}


// On SIGHUP, fall out of the little loop and continue the big loop
void
handle_hup()
{
	write_log(INFO, __FILE__, "SIGHUP recieved");
	continue_little_loop = 0;
}


// On SIGUSR1, reset the hardware, server or client
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

	// Mask all signals except for those we pay attention to
	sigfillset(&block_set);
	sigdelset(&block_set, SIGTERM);
	sigdelset(&block_set, SIGHUP);
	sigdelset(&block_set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &block_set, NULL);

	// Register SIGTERM
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


	// Register SIGHUP
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

	// Register SIGUSR1
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
