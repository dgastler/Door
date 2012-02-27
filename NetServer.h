#ifndef __NETSERVER_H__
#define __NETSERVER_H__
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "SerialLink.h"

void ProcessNewConnection(int serverFD);
void SendOut(char * buffer,ssize_t bufferSize);
void AddClientOAFD(int _fd);
void RemoveClientFD(int _fd);
int GetMaxClientFD();
int ClearClientFDs();
void CloseClients();

#endif
