#include "SerialLink.h"

int setNonblocking(int fd)
{
  int flags;
  /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
  /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
  if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
    {
      flags = 0;
    }
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
  /* Otherwise, use the old way of doing it */
  flags = 1;
  return ioctl(fd, FIOBIO, &flags);
#endif
}   

int serialport_init(const char* serialport, int baud)
{  
  struct termios toptions;  
  int fd;
  fd = open(serialport, O_RDWR | O_NOCTTY);
  if (fd == -1)  
    {
      perror("init_serialport: Unable to open port ");
      return -1;
    }
  
  if (tcgetattr(fd, &toptions) < 0) 
    {      
      perror("init_serialport: Couldn't get term attributes");      
      return -1;      
    }
  
  speed_t brate = baud; // let you override switch below if needed
  
  switch(baud) 
    {      
    case 4800:   
      brate=B4800;   
      break;      
    case 9600:   
      brate=B9600;   
      break;      
    case 19200:  
      brate=B19200;  
      break;      
    case 38400:  
      brate=B38400;  
      break;      
    case 57600:  
      brate=B57600;  
      break;      
    case 115200: 
      brate=B115200; 
      break;      
    }
  
  cfsetispeed(&toptions, brate);  
  cfsetospeed(&toptions, brate);
  

  
  // 8N1
  toptions.c_cflag &= ~PARENB;
  toptions.c_cflag &= ~CSTOPB;
  toptions.c_cflag &= ~CSIZE;
  toptions.c_cflag |= CS8;
    
  // no flow control    
  toptions.c_cflag &= ~CRTSCTS;
  toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
  toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
  toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
  toptions.c_oflag &= ~OPOST; // make raw

  // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
  toptions.c_cc[VMIN]  = 0;
  toptions.c_cc[VTIME] = 20;

  if( tcsetattr(fd, TCSANOW, &toptions) < 0) 
    {      
      perror("init_serialport: Couldn't set term attributes");      
      return -1;      
    }
  return fd;
}
