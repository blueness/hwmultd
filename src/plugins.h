
int load_plugins();
int unload_plugins();

int (*init_hw)();
int (*reset_hw)();
char *(*read_hw)();
int (*close_hw)();

int (*init_cl)();
int (*reset_cl)();
char *(*act_cl)(char *);
int (*close_cl)();
