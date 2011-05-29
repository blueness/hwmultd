
int load_plugins();
int unload_plugins();

int already_init_hw;

int (*init_hw)();
int (*reset_hw)();
char *(*read_hw)();
int (*close_hw)();
