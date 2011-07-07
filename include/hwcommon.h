#ifndef HWCOMMON_H_INCLUDED
#define HWCOMMON_H_INCLUDED

#define MSG_BUFFER 4096

void __attribute__ ((constructor)) hwplugin_init(void);
void __attribute__ ((destructor)) hwplugin_fini(void);

int init_hw();
int reset_hw();
char *read_hw();
int close_hw();

// Return errors
#define HW_SUCCESS		 1
#define HW_MALLOC		-1
#define HW_OPEN_DEV		-2
#define HW_GET_DEV_ATTR		-3
#define HW_SET_DEV_ATTR		-4
#define HW_WRITE_DEV		-5
#define HW_CLOSE		-6

#endif
