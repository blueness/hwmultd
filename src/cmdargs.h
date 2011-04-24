#include <stdint.h>

/*
 * See http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xml
 * for the choice of multicast address in Site Local Scope, RFC RFC2365.
 */

#define DEFAULT_SITE_LOCAL_IP "239.252.67.151"
#define DEFAULT_PORT 8000
#define MIN_PORT 1024
#define MAX_PORT 65535

char site_ip[20];
uint16_t port;



// Log level before log_level is read from command line or config file
#define EARLY_LOG_LEVEL 0
#define DEFAULT_LOG_LEVEL 2

int log_level;



#define CLIENT_MODE 0
#define SERVER_MODE 1
#define DEFAULT_SERVER_MODE 0

char server_mode;


#define DEFAULT_CONFIG_FILENAME "/etc/tempmond.conf"
#define CONF_LINE_BUFFER 1000

void parse_cfg_file();
void parse_cmd_args( int, char ** );
