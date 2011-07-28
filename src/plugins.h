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

// These are wrapper functions for loading either
// hardware (ie server) plugins, or client plugins
int load_plugins();
int unload_plugins();

// These are the function names (symbols) as we know them,
// of the functions provided by the plugins

// For the hardware (ie server) plugin
int (*init_hw)();
int (*reset_hw)();
char *(*read_hw)();
int (*close_hw)();

// For the client plugin
int (*init_cl)();
int (*reset_cl)();
char *(*act_cl)(char *);
int (*close_cl)();
