
#define DBUG 3
#define INFO 2
#define ERRO 1
#define CRIT 0

#define TIME_BUFFER 1000

int open_log();
int write_log(int, const char *,...);
int close_log();

