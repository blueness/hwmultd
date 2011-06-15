
#define MSG_BUFFER 4096

void __attribute__ ((constructor)) clplugin_init(void);
void __attribute__ ((destructor)) clplugin_fini(void);

int init_cl();
int reset_cl();
char *act_cl(char *);
int close_cl();
