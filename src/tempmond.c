
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <signalhandler.h>

int
main( int argc, char *argv[] )
{
	/* Fork into background */
	pid_t pid = fork();

	if (pid < 0)
	{
		fprintf( stderr, "Failed to fork\n") ;
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
		exit(EXIT_SUCCESS);

	pid = getpid() ;

	/* Close open files inherited from parent. */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Create a new SID for the child process */
	pid_t sid = setsid();
	if (sid < 0)
	{
		exit(EXIT_FAILURE);
	}

	/* Handle signals */
	int i ;
	const int caught_signal[] = { SIGTERM, SIGINT, SIGHUP, 0 };

	for (i = 0; caught_signal[i] != 0; i++)
		signal(caught_signal[i],sighandler);

	/* The big loop */
	while(1)
	{
		sleep(30);
	}

	return 0;
}
