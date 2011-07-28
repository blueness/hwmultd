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


#include "cmdargs.h"
#include "log.h"
#include "plugins.h"



#include <dlfcn.h>
#include <string.h>



void *handle_hw, *handle_cl;

int
load_plugins()
{
	// Add DEFAULT_PLUGIN_PREFIX + "/" + hw_plugin_name + {"_hw","_cl"} + ".so"
	char plugin_name[MAX_PLUGIN_LEN+strlen(DEFAULT_PLUGIN_PREFIX)+4];

	write_log(DBUG, __FILE__, "loading plugins");

	// Load hardware plugin if we are in server mode or both
	if(operation_mode == SERVER_MODE || operation_mode == BOTH_MODE)
	{
		// Construct plugin name: /path/to/plugin + /myplugin + _hw.so
		strcpy(plugin_name, DEFAULT_PLUGIN_PREFIX);
		strcat(plugin_name, "/");
		strncat(plugin_name, hw_plugin_name, MAX_PLUGIN_LEN);
		strcat(plugin_name, "_hw.so");

		// Dlopen the hardware plugin lazily
		if( !(handle_hw = dlopen(plugin_name, RTLD_LAZY)) )
		{
			write_log(ERRO, __FILE__, "failed dlopen hw plugin %s -> %s", hw_plugin_name, plugin_name);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "dlopened hw plugin %s -> %s", hw_plugin_name, plugin_name);


		// Assign the plugin symbols to our symbols
		// For simplicity, try to avoid changing symbol name
		init_hw = dlsym(handle_hw, "init_hw");
		reset_hw = dlsym(handle_hw, "reset_hw");
		read_hw = dlsym(handle_hw, "read_hw");
		close_hw = dlsym(handle_hw, "close_hw");

		// We expect all four to get assigned.
		// If any are supposed to "do nothing",
		// then the plugin must provide a "do nothing" function
		if( init_hw == NULL || reset_hw == NULL || read_hw == NULL || close_hw == NULL )
		{
			write_log(ERRO, __FILE__, "failed to register hw funcs init_hw || reset_hw || read_hw || close_hw");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "registered hw funcs init_hw && reset_hw && read_hw && close_hw");
	}

	if(operation_mode == CLIENT_MODE || operation_mode == BOTH_MODE)
	{
		// Construct plugin name: /path/to/plugin + /myplugin + _hw.so
		strcpy(plugin_name, DEFAULT_PLUGIN_PREFIX);
		strcat(plugin_name, "/");
		strncat(plugin_name, cl_plugin_name, MAX_PLUGIN_LEN);
		strcat(plugin_name, "_cl.so");

		// Dlopen the client plugin lazily
		if( !(handle_cl = dlopen(plugin_name, RTLD_LAZY)) )
		{
			write_log(ERRO, __FILE__, "failed dlopen cl plugin %s -> %s", cl_plugin_name, plugin_name);
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "dlopened cl plugin %s -> %s", cl_plugin_name, plugin_name);

		// Assign the plugin symbols to our symbols
		// For simplicity, try to avoid changing symbol name
		init_cl = dlsym(handle_cl, "init_cl");
		reset_cl = dlsym(handle_cl, "reset_cl");
		act_cl = dlsym(handle_cl, "act_cl");
		close_cl = dlsym(handle_cl, "close_cl");

		// We expect all four to get assigned
		// If any are supposed to "do nothing",
		// then the plugin must provide a "do nothing" function
		if( init_cl == NULL || reset_cl == NULL || act_cl == NULL || close_cl == NULL )
		{
			write_log(ERRO, __FILE__, "failed to register cl funcs init_cl || reset_cl || act_cl || close_cl");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "registered cl funcs init_cl && reset_cl && act_cl && close_cl");
	}

	return 1;
}


int
reset_plugins()
{
	// Reset the hardware if we are in server mode or both
	if(operation_mode == SERVER_MODE || operation_mode == BOTH_MODE)
	{
		if( !reset_hw )
		{
			write_log(ERRO, __FILE__, "failed server hardware reset ");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "server hardware reset");
	}

	// Reset the cleint if we are in client mode or both
	if(operation_mode == CLIENT_MODE || operation_mode == BOTH_MODE)
	{
		if( !reset_cl )
		{
			write_log(ERRO, __FILE__, "failed client hardware reset ");
			return 0;
		}
		else
			write_log(DBUG, __FILE__, "client hardware reset");
	}

	return 1;
}


int
unload_plugins()
{
	// Unload hardware plugin if we are in server mode or both
	if(operation_mode == SERVER_MODE || operation_mode == BOTH_MODE)
	{
		if(handle_hw)
		{
			if( dlclose(handle_hw) )
			{
				write_log(ERRO, __FILE__, "failed unload hw plugin");
				return 0;
			}
			else
				write_log(DBUG, __FILE__, "unloaded hw plugin");
		}
		else
			write_log(DBUG, __FILE__, "no hw plugin to unload");
	}

	// Unload client plugin if we are in client mode or both
	if(operation_mode == CLIENT_MODE || operation_mode == BOTH_MODE)
	{
		if(handle_cl)
		{
			if( dlclose(handle_cl) )
			{
				write_log(ERRO, __FILE__, "failed unload cl plugin");
				return 0;
			}
			else
				write_log(DBUG, __FILE__, "unloaded cl plugin");
		}
		else
			write_log(DBUG, __FILE__, "no cl plugin to unload");
	}

	return 1;
}
