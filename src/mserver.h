
/*
 * See http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xml
 * for the choice of multicast address in Site Local Scope, RFC RFC2365.
 */

#define MSG_BUFFER 1000

int sd;
struct sockaddr_in saddr;

int mserver_start();
char *recv_mcast_msg();
