
#include <dlfcn.h>
#include <string.h>

#include <cmdargs.h>
#include <log.h>
#include <plugins.h>


#undef ME
#define ME "plugins.c"

void *handle_hw, *handle_cl;

int
load_plugins()
{
	char lib_name[MAX_PLUGIN_LEN+9];		// Add "lib" + hw_plugin_name + {"_hw","_cl"} + ".so"

	write_log(DBUG, ME, "loading plugins");

	if(server_mode == SERVER_MODE)
	{
		strcpy(lib_name, "lib");
		strncat(lib_name, hw_plugin_name, MAX_PLUGIN_LEN);
		strcat(lib_name, "_hw.so");
	
		if( !(handle_hw = dlopen(lib_name, RTLD_LAZY)) )
		{
			write_log(ERRO, ME, "failed dlopen hw plugin %s -> %s", hw_plugin_name, lib_name);
			return 0;
		}
		else
			write_log(DBUG, ME, "dlopened hw plugin %s -> %s", hw_plugin_name, lib_name);


		init_hw = dlsym(handle_hw, "init_hw");
		reset_hw = dlsym(handle_hw, "reset_hw");
		read_hw = dlsym(handle_hw, "read_hw");
		close_hw = dlsym(handle_hw, "close_hw");

		if( init_hw == NULL || reset_hw == NULL || read_hw == NULL || close_hw == NULL )
		{
			write_log(ERRO, ME, "failed to register hw funcs init_hw || reset_hw || read_hw || close_hw");
			return 0;
		}
		else
			write_log(DBUG, ME, "registered hw funcs init_hw && reset_hw && read_hw && close_hw");
	}
	else
	{
		strcpy(lib_name, "lib");
		strncat(lib_name, cl_plugin_name, MAX_PLUGIN_LEN);
		strcat(lib_name, "_cl.so");

		if( !(handle_cl = dlopen(lib_name, RTLD_LAZY)) )
		{
			write_log(ERRO, ME, "failed dlopen cl plugin %s -> %s", cl_plugin_name, lib_name);
			return 0;
		}
		else
			write_log(DBUG, ME, "dlopened hw plugin %s -> %s", cl_plugin_name, lib_name);

		init_cl = dlsym(handle_cl, "init_cl");
		reset_cl = dlsym(handle_cl, "reset_cl");
		act_cl = dlsym(handle_cl, "act_cl");
		close_cl = dlsym(handle_cl, "close_cl");

		if( init_cl == NULL || reset_cl == NULL || act_cl == NULL || close_cl == NULL )
		{
			write_log(ERRO, ME, "failed to register cl funcs init_cl || reset_cl || act_cl || close_cl");
			return 0;
		}
		else
			write_log(DBUG, ME, "registered cl funcs init_cl && reset_cl && act_cl && close_cl");
	}

	return 1;
}


int
unload_plugins()
{
	if(server_mode == SERVER_MODE)
	{
		if(handle_hw)
		{
			if( dlclose(handle_hw) )
			{
				write_log(ERRO, ME, "failed unload hw plugin");
				return 0;
			}
			else
				write_log(DBUG, ME, "unloaded hw plugin");
		}
		else
			write_log(DBUG, ME, "no hw plugin to unload");
	}
	else
	{
		if(handle_cl)
		{
			if( dlclose(handle_cl) )
			{
				write_log(ERRO, ME, "failed unload cl plugin");
				return 0;
			}
			else
				write_log(DBUG, ME, "unloaded cl plugin");
		}
		else
			write_log(DBUG, ME, "no cl plugin to unload");
	}

	return 1;
}
