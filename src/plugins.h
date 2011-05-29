
int load_plugins();
int unload_plugins();

int (*init_hw)();
int (*reset_hw)();
char *(*read_hw)();
int (*close_hw)();
