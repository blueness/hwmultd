
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

	fprintf( stdout, "I am stdout\n") ;
	fprintf( stderr, "I am stderr\n") ;

	/* Create a new SID for the child process */
	pid_t sid = setsid();
	if (sid < 0)
	{
		exit(EXIT_FAILURE);
	}

	for(;;);

	return 0;
}
