
#include <unistd.h>

pid_t wpid;
int worker_mode;

void clean_exit();
int sighandler();
