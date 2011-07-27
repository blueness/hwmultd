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
 * Log levels
 *
 * ERRO and CRIT are reserved for show stoppers.
 * ERRO is logged at a deeper level and in more detail than CRIT
 * INFO is for normal operations.
 * DBUG is to follow step-by-step the operation of the daemon.
 */
#define DBUG 3
#define INFO 2
#define ERRO 1
#define CRIT 0

#define LOG_FILE "/var/log/hwmultd.log"

int open_log();
int write_log(int, const char *, const char *,...);
int close_log();
