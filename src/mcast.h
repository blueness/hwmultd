/*
 * See http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xml
 * for the choice of multicast address in Site Local Scope, RFC RFC2365.
 */

#include <netinet/in.h>

#define MSG_BUFFER 1000

int sd, cd;
struct sockaddr_in saddr, caddr;

int mserver_start();
char *rcv_mcast_msg();
int mserver_stop();

int mclient_start();
int snd_mcast_msg();
int mclient_stop();
