
#include <unistd.h>

#define PID_DIR  "/var/run/tempmond"
#define PID_FILE "/var/run/tempmond/tempmond.pid"

int dir_pid(uid_t, gid_t);
int open_pid(int);
int close_pid();
