
#include <dlfcn.h>
#include <string.h>

#include <cmdargs.h>
#include <log.h>
#include <plugins.h>

void *handle;

int
load_plugins()
{
	char lib_name[MAX_PLUGIN_LEN+6];		// Add "lib" + hw_plugin_name + ".so"

	strcpy(lib_name, "lib");
	strncat(lib_name, hw_plugin_name, MAX_PLUGIN_LEN);
	strcat(lib_name, ".so");
	
	if( !(handle = dlopen(lib_name, RTLD_LAZY)) )
	{
		write_log(ERRO,"failed dlopen %s", hw_plugin_name);
		return 0;
	}
	else
		write_log(DBUG,"dlopened %s", hw_plugin_name);

	init_hw = dlsym(handle, "init_hw");
	reset_hw = dlsym(handle, "reset_hw");
	read_hw = dlsym(handle, "read_hw");
	close_hw = dlsym(handle, "close_hw");
}

int
unload_plugins()
{
	dlclose(handle);
}
