#include <stdint.h>

#define DEFAULT_CONFIG_FILENAME "/etc/tempmond.conf"
#define DEFAULT_SERVER_MODE 0 ;
#define DEFAULT_PORT 8000;

#define CONF_LINE_BUFFER 1000

char server_mode;
uint16_t port;

void parse_cfg_file();
void parse_cmd_args( int, char ** );
