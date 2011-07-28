
#include <unistd.h>

// This boolean determine if we stay in the "little" loop.
// That is the loop in which we continue to do_service()
int continue_little_loop;

// These function act as wrappers for the daemon's services
// The service may be either as client, or server, or both
int start_service();
int do_service();
int stop_service();
