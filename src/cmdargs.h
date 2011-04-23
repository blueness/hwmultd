#include <stdint.h>

#define MIN_PORT 1024
#define MAX_PORT 65535

#define CLIENT_MODE 0
#define SERVER_MODE 1

#define DEFAULT_CONFIG_FILENAME "/etc/tempmond.conf"
#define DEFAULT_PORT 8000
#define EARLY_LOG_LEVEL 0
#define DEFAULT_LOG_LEVEL 2
#define DEFAULT_SERVER_MODE 0

#define CONF_LINE_BUFFER 1000

char server_mode;
int log_level;
uint16_t port;

void parse_cfg_file();
void parse_cmd_args( int, char ** );
