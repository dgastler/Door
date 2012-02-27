#include "Arduino.h"

void OpenDoor(int fd)
{
  char o[2] = "o\n";
  write(fd,o,sizeof(o));
}
void CloseDoor(int fd)
{
  char c[2] = "c\n";
  write(fd,c,sizeof(c));
}

int ProcessArduino(int arduinoFD)
{
  const unsigned bufferSize = 256;
  char buffer[bufferSize+1];  
  //  unsigned readSize = read(arduinoFD,buffer,bufferSize);
  return 1;
}
