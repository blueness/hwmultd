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


#ifndef CLCOMMON_H_INCLUDED
#define CLCOMMON_H_INCLUDED

#define MSG_BUFFER 4096

int init_cl();
int reset_cl();
char *act_cl(char *);
int close_cl();


// Return errors
#define CL_SUCCESS		 1
#define CL_MALLOC		-1
#define CL_OPEN_DEV		-2
#define CL_GET_DEV_ATTR		-3
#define CL_SET_DEV_ATTR		-4
#define CL_READ_DEV		-5
#define CL_WRITE_DEV		-6
#define CL_CLOSE_DEV		-7
#define CL_OPEN_FILE		-8
#define CL_CLOSE_FILE		-9

#endif
