#include "FDlist.h"

const ssize_t fdMaxSize = 100;
int fd[100];
ssize_t fdSize = 0;

void AddFD(int _fd)
{
  if(fdSize<fdMaxSize)
    {
      fd[fdSize] = _fd;
      fdSize++;
    }  
}
void RemoveFD(int _fd)
{
  //find fd and swap it with the last one
  ssize_t i;
  for(i= 0; i < fdSize;i++)
    {
      if(fd[i] == _fd)
	{
	  fd[i] = fd[fdSize];
	  fdSize--;
	}
    }
}
int GetMaxFD()
{
  ssize_t i;
  int _fd = -1;
  for(i= 0; i < fdSize;i++)
    {
      if(fd[i] > _fd)
	{
	  _fd = fd[i];
	}
    }
  return _fd;
}

void ClearFDs()
{
  fdSize = 0;
}

