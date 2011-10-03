/*
	This file is part of hwmultd.
	Copyright (C) 2011  Anthony G. Basile

	hwmultd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	hwmultd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <utmp.h>
#include <pwd.h>
#include <stdint.h>



/************************************************************************
 * NETWORK PARAMETERS
 ************************************************************************/

// The maximum number of chars for an IP or network interface
// This is overkill, but we are anticipating ipv6
#define MAX_IP_LEN 64
#define MAX_IF_LEN 64

// For the choice of multicast address in Organization-Local Scope, RFC RFC2365.
// http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xml

// The default multicast IP, if none is given in config file
#define DEFAULT_MULTICAST_IP "239.252.67.151"

//The default port, if none is given in the config file
#define DEFAULT_PORT 8000

// The minimum port allowed before defaulting to DEFAULT_PORT
#define MIN_PORT 1024

// The maximum port allowed before defaulting to DEFAULT_PORT
#define MAX_PORT 65535

// GLOBAL VALUE: The multicast IP and port
char multicast_ip[MAX_IP_LEN];
uint16_t port;

// The default interface IP and name, if none is given in the config file
#define DEFAULT_INTERFACE_IP "0.0.0.0"
#define DEFAULT_INTERFACE_NAME "dummy0"

// GLOBAL VALUE: The interface IP and interface name that we will multicast
// out of, in case we have more than on interface
char interface_ip[MAX_IP_LEN];
char interface_name[MAX_IF_LEN];



/************************************************************************
 * OPERATIONAL MODE PARAMETERS
 ************************************************************************/

// What modes of operation are possible?
// We can run as a server, a client or both
#define CLIENT_MODE 0
#define SERVER_MODE 1
#define BOTH_MODE 2
#define DEFAULT_MODE 0

// GLOBAL VALUE: What operation mode are we running in?
int operation_mode;



/************************************************************************
 * DAEMON PRIVILEGES
 ************************************************************************/

// The default username to try when dropping privileges, if none
// is given in the config file.  If neither the default username,
// nor configuration supplied username exist on a given system,
// we'll fall back on FALLBACK_USERNAME.  This username must be
// set to something which every system is guaranteed to have
#define DEFAULT_USERNAME "hwmultd"
#define FALLBACK_USERNAME "root"

// GLOBAL VALUE: The username to which we drop privileges and passwd struct
char user_name[UT_NAMESIZE];
struct passwd *pwd;


/************************************************************************
 * TIMING PARAMETERS
 ************************************************************************/

// The minimum, maximum and default time to wait between receiving or
// or sending multicast messages, in seconds
#define MIN_TIMING 1
#define MAX_TIMING 300
#define DEFAULT_TIMING 1

// GLOBAL VALUE: The time to wait between receiving or sending
int timing;


/************************************************************************
 * LOG PARAMETERS
 * See log.h for the definition of the log levels
 ************************************************************************/

// DEFAULT_LOGDEST = where do we log to by default?
// "file" = file only, "syslog" = syslog only, "both" = both
#define DEFAULT_LOG_DEST "file"
#define MAX_LOG_DEST 6

// GLOBAL VALUE: The log destination
int log_dest;

// Log level to use before log_level is read from the config file
#define EARLY_LOG_LEVEL 3

// The default log level to use, if none is given in config file
#define DEFAULT_LOG_LEVEL 2

// GLOBAL VALUE: The log level to use
int log_level;



/************************************************************************
 * PLUGINS PARAMETERS
 ************************************************************************/

// The maximum length of a plugin name, not including the path
#define MAX_PLUGIN_LEN 128

// The default name of the server hardware plugin,
// if none is given in the config file
#define DEFAULT_HW_PLUGIN "null"

// The default name of the client hardware plugin,
// if none is given in the config file
#define DEFAULT_CL_PLUGIN "null"

// The default path to the directory which holds the plugins
#ifndef DEFAULT_PLUGIN_PREFIX
#define DEFAULT_PLUGIN_PREFIX "/usr/local/lib/hwmultd/"
#endif

// GOBAL VALUE: The server or client plugin name
char hw_plugin_name[MAX_PLUGIN_LEN];
char cl_plugin_name[MAX_PLUGIN_LEN];


/************************************************************************
 * CONFIGURATION FILE PARAMETERS
 ************************************************************************/

// The maximum length of the path to the directory containing the config file
#define MAX_CONF_DIR_LEN 1024

// The maximum length of the name of the config file
#define MAX_CONF_FILE_LEN 1024

// The default configuration directory
#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif


/************************************************************************/

// Parse the config file and set the above GLOBAL VALUEs
void parse_cfg_file();

// Parse the command line arguments 
void parse_cmd_args( int, char ** );
