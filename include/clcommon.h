#ifndef CLCOMMON_H_INCLUDED
#define CLCOMMON_H_INCLUDED

#define MSG_BUFFER 4096

void __attribute__ ((constructor)) clplugin_init(void);
void __attribute__ ((destructor)) clplugin_fini(void);

int init_cl();
int reset_cl();
char *act_cl(char *);
int close_cl();


// Return errors
#define CL_SUCCESS		 1
#define CL_MALLOC		-1
#define CL_OPEN_DEV		-2
#define CL_GET_DEV_ATTR		-3
#define CL_SET_DEV_ATTR		-4
#define CL_READ_DEV		-5
#define CL_WRITE_DEV		-6
#define CL_CLOSE		-7

#endif
