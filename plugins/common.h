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


// Maximum length of the path name where plugin config files are stored
#define MAX_CONF_DIR_LEN 1024

// Maximum length of a plugin config file name
#define MAX_CONF_FILE_LEN 1024

// Maximum length of a config file line
#define CONF_LINE_BUFFER 4096

// Default path to a pluging config file
#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif
