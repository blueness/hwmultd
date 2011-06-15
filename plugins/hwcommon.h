
#define MSG_BUFFER 4096

void __attribute__ ((constructor)) hwplugin_init(void);
void __attribute__ ((destructor)) hwplugin_fini(void);

int init_hw();
int reset_hw();
char *read_hw();
int close_hw();
