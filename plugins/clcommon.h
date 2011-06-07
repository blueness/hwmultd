
void __attribute__ ((constructor)) my_init(void);
void __attribute__ ((destructor)) my_fini(void);

int init_cl();
int reset_cl();
char *act_cl(char *);
int close_cl();
