
#include <unistd.h>

pid_t wpid;

int sighandler_worker();

void clean_exit();
int sighandler_session_leader();
