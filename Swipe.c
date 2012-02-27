#include "Swipe.h"

int ProcessCardSwipe(int swipeFD,int arduinoFD, MYSQL * conn)
{
  int ret = 0;
  const unsigned bufferSize = 256;
  char buffer[bufferSize+1];
  buffer[bufferSize] = '\0';
  bzero(buffer,bufferSize);

  unsigned readSize = ReadKeyboard(swipeFD,buffer,bufferSize);
  if(readSize == 10)
    {
      int opendoor = 0;
      buffer[readSize] = '\0';
      //              0123456789012345678901234567890123 45678901
      char query[] = "select * from users where swipe = \"XXXXXXXXX\" limit 0,1 ";      
      memcpy(query+35,buffer,9); 
      //      printf("%s (%d)\n",query,sizeof(query));
      
      opendoor = RunSQLquery(conn,query,sizeof(query));
      if(opendoor == 1)
	{
	  alarm(0); //Cancel any previous alarms
	  OpenDoor(arduinoFD);
	  alarm(5);  //schedule an alarm to go off in 5 seconds to close the door
	}

      memcpy(query+35,"XXXXXXXXX",9); 
      ret = 1;
    }
  else if(readSize == 0)
    {
      ret = 1;
    }
  else
    {
      ret = 0;
    }
  return ret;
}
