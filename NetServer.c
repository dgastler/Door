#include "NetServer.h"

const ssize_t clientFDMaxSize=100;
int clientFD[100]; 
//int clientFD[clientFDMaxSize]; ///why error?
ssize_t clientFDSize = 0;

void AddClientFD(int _fd)
{
  if(clientFDSize<clientFDMaxSize)
    {
      clientFD[clientFDSize] = _fd;
      clientFDSize++;
    }  
}
void RemoveClientFD(int _fd)
{
  //find fd and swap it with the last one
  ssize_t i;
  for(i= 0; i < clientFDSize;i++)
    {
      if(clientFD[i] == _fd)
	{
	  clientFD[i] = clientFD[clientFDSize];
	  clientFDSize--;
	}
    }
}
int GetMaxClientFD()
{
  ssize_t i;
  int _fd = -1;
  for(i= 0; i < clientFDSize;i++)
    {
      if(clientFD[i] > _fd)
	{
	  _fd = clientFD[i];
	}
    }
  return _fd;
}

int ClearClientFDs()
{
  clientFDSize = 0;
}


void ProcessNewConnection(int serverFD)
{
  int clientFD;
  struct sockaddr_in connectingAddr;
  socklen_t connectingAddrSize;
  clientFD = accept(serverFD,&connectingAddr,&connectingAddrSize);  
  if(clientFD != -1)
    {
      AddClientFD(clientFD);
      setNonblocking(clientFD);  //no one's breaking the door by not listening to it!
      char str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(connectingAddr.sin_addr), str, INET_ADDRSTRLEN);      
      printf("New client from %s\n",str);
    }
}
void SendOut(char * buffer,ssize_t bufferSize)
{
  int i,netRet;
  for(i = 0; i < clientFDSize;i++)
    {
      netRet = write(clientFD[i],buffer,bufferSize);
      if(netRet == -1)
	{
	  if((errno == EAGAIN) ||
	     (errno == EWOULDBLOCK))
	    {
	      printf("Someone didn't get a full message and they can deal with it.\n");
	    }
	  else if(errno == EPIPE)
	    {
	      printf("Removing a bad connection\n");
	      close(clientFD[i]);
	      RemoveClientFD(clientFD[i]);
	      //Now the last fd is now in the current position, so we want to re-check it. 
	      i--;
	    }
	}
    }
}

void CloseClients()
{
  int i;
  for(i = 0; i < clientFDSize;i++)
    {
      close(clientFD[i]);
    }
  ClearClientFDs();
}

