
/*
 * Log levels
 *
 * ERRO and CRIT are reserved for show stoppers.
 * ERRO is logged at a deeper level and in more detail than CRIT
 * INFO is for normal operations.
 * DBUG is to follow step-by-step the operation of the daemon.
 */
#define DBUG 3
#define INFO 2
#define ERRO 1
#define CRIT 0

#define LOG_FILE "/var/log/hwmultd.log"

int open_log();
int write_log(int, const char *,...);
int close_log();
