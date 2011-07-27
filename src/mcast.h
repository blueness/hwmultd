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

#define MSG_BUFFER 4096

int mserver_start();
char *rcv_mcast_msg();
int mserver_stop();

int mclient_start();
int snd_mcast_msg();
int mclient_stop();
