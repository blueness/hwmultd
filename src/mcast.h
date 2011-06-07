/*
 * See http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xml
 * for the choice of multicast address in Site Local Scope, RFC RFC2365.
 */

#define MSG_BUFFER 1000

int mserver_start();
char *rcv_mcast_msg();
int mserver_stop();

int mclient_start();
int snd_mcast_msg();
int mclient_stop();
