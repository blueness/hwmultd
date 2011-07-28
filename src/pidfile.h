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


#include <unistd.h>

#define PID_DIR  "/var/run/hwmultd"
#define PID_FILE "/var/run/hwmultd/hwmultd.pid"

// Make sure the pid directory exists and belongs to us
int dir_pid(uid_t, gid_t);

// Create the pidfile and write the PID to it
int open_pid(int);

// Remove the pidfile
int close_pid();
