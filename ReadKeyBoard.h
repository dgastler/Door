#ifndef __READKEYBOARD_H__
#define __READKEYBOARD_H__

#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>

#define RKB_NULL_POINTER -1
#define RKB_ZERO_SIZE -2
#define RKB_BAD_READ -3

#define KEY_RELEASE 0
#define KEY_PRESS 1
#define KEY_KEEPING_PRESSED 2

char ConvertNumerics(int value);
char ConvertAlphas(int value);
char ConvertKeyToASCII(struct input_event * ev);
int ReadKeyboard(int fd,char * buffer,int bufferSize);
int ProcessEvent(struct input_event * ev, char * retChar);
#endif
