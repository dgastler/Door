#ifndef __FDLIST_H__
#define __FDLIST_H__
#include <unistd.h>

extern int fd[100];
void AddFD(int _fd);
void RemoveFD(int _fd);
int GetMaxFD();
void ClearFDs();
#endif
