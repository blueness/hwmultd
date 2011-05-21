
void __attribute__ ((constructor)) my_init(void);
void __attribute__ ((destructor)) my_fini(void);

int init_hw();
int reset_hw();
char *read_hw();
int close_hw();
