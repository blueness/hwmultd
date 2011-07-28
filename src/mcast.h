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


/*
 * See http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xml
 * for the choice of multicast address in Site Local Scope, RFC RFC2365.
 */

// Maximum size of the received message
#define MSG_BUFFER 4096


// Prepare the listening socket and message buffer for the server
int mserver_start();

// Block until a message is received and return a pointer to that message
char *rcv_mcast_msg();

// Free the message buffer and tear down the server (listening) socket
int mserver_stop();


// Prepare the client socket
int mclient_start();

// Receive a pointer to a buffer and multicast out the null terminated string
int snd_mcast_msg(const char *);

// Tear down the client (sending) socket
int mclient_stop();
