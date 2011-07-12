#ifndef HWCOMMON_H_INCLUDED
#define HWCOMMON_H_INCLUDED

#define MSG_BUFFER 4096

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
#define HW_READ_DEV		-5
#define HW_WRITE_DEV		-6
#define HW_CLOSE_DEV		-7
#define HW_OPEN_FILE		-8
#define HW_CLOSE_FILE		-9

#endif
