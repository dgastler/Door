#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "Arduino.h"
#include "Swipe.h"
#include "MySQL.h"
#include "NetServer.h"
#include "FDlist.h"

volatile int activeRun = 1;
volatile int mainRun = 1;

MYSQL conn;
int arduinoFD = -1;
int swipeFD = -1;        
int serverFD = -1;

void signal_handler(int sig)
{
  if(sig == SIGINT)
    {
      //Stop the active loop if running
      if(activeRun == 1)
	{
	  activeRun = 0;
	}
      else
	{
	  //If we get ctrl-c and the active loop is off, die
	  mainRun = 0;
	}
    }
  else if(sig == SIGALRM)
    {
      CloseDoor(arduinoFD);      
    }
  else if(sig == SIGPIPE)
    {
      printf("ERROR SIGPIPE!\n");
    }
}

int main(int argc, char ** argv)
{
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGALRM,&sa, NULL);
  sigaction(SIGPIPE,&sa, NULL);

  //Main loop
  //sets up MYSQL connection, arduino connection and keyboard connection
  //a inner loop ends when any of these fail and the main loop closes and 
  //sets up everything again.
  while(mainRun)
    {
      //Clear all FDs for listening
      ClearFDs();
     
      //===========================================================
      //Socket server for clients
      //===========================================================
      struct sockaddr_in addr;
      bzero(&addr,sizeof(addr));
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = htonl(INADDR_ANY);
      addr.sin_port = htons(1337);

      serverFD = socket(AF_INET,SOCK_STREAM,0);

      int bindRet = -1;
      do
	{
	  bindRet = bind(serverFD,(struct sockaddr *) &addr,sizeof(addr));
	  if(bindRet < 0)
	    sleep(1);
	}while(bindRet < 0); //check for a failed bind

      int listenRet = -1;
      do
	{
	  listenRet = listen(serverFD,14); //(#14 for linux)  P98 Unix Network Programming 
	  if(listenRet < 0)
	    sleep(1);
	}while(listenRet < 0); //check for a failed listen

      AddFD(serverFD);

      //===========================================================

      //===========================================================
      //Setup SQL connection
      //===========================================================
      MYSQL * connCheck;       
      mysql_init(&conn);
      do
	{
	  connCheck = mysql_real_connect(&conn,
					 "localhost",
					 "door",
					 "buildsdoor",
					 "door",
					 0, NULL, 0);
	  if(connCheck == NULL)
	    sleep(1);      
	}while(connCheck == NULL);  //check for a failed connection
      
      //===========================================================
      

      //===========================================================
      //Setup Arduino interface
      //===========================================================
      while(arduinoFD == -1)
	{
	  arduinoFD = serialport_init("/dev/ttyUSB0",9600);
	  if(arduinoFD < 0)
	    sleep(1);
	}
      //make sure door is locked  (simple door hack)
      CloseDoor(arduinoFD);
      CloseDoor(arduinoFD);
      AddFD(arduinoFD);
      //===========================================================


      //===========================================================
      //Setup Keyboard interface
      //===========================================================
      while(swipeFD == -1)
	{
	  swipeFD = open("/dev/input/by-id/usb-Unitech_America_Inc._Unitech_USB_Keyboard-event-kbd",
			 O_RDONLY);  
	  if(swipeFD < 0)
	    sleep(1);
	}
      AddFD(swipeFD);
      //===========================================================
            
      
      //===========================================================
      //Setup parameters for select
      //===========================================================
      //fd sets for select
      fd_set rdSet,wrSet,exSet;  //for use by select
      fd_set rdCheckSet,wrCheckSet,exCheckSet; //base fd sets to not be molested by select
      //timeout for select with molest and no molest versions
      struct timeval wait,waitCheck;
      
      //Since we are only reading stdin(fd = 0), the maxFD plus one is 1            
      int fdPlusOne = GetMaxFD()+1;//(swipeFD > arduinoFD) ? swipeFD + 1 : arduinoFD +1;
      //Setup defaults for select parameters
      FD_ZERO(&rdSet);
      FD_ZERO(&wrSet);
      FD_ZERO(&exSet);
      //Add the arduino and swipe FDs
      FD_SET(swipeFD,&rdSet);
      FD_SET(arduinoFD,&rdSet);
      FD_SET(serverFD,&rdSet);
      //timeout select 10 times a second
      wait.tv_sec = 100;
      wait.tv_usec = 0;//100000;
      waitCheck = wait;
      //===========================================================      

      
      //===========================================================
      //Active Loop
      //===========================================================
      printf("Starting Active Loop\n");
      activeRun = 1; //Allows for this loop to end if there is an critical error
      while(activeRun)
	{
	  //Copy the unmolested to the molested parameters for select               
	  rdCheckSet = rdSet;
	  wrCheckSet = wrSet;
	  exCheckSet = exSet;
	  wait = waitCheck;
	  int selectRet = select(fdPlusOne, 
				 &rdCheckSet,
				 &wrCheckSet,
				 &exCheckSet,
				 &wait);

	  
	  //Something happened in our select sets
	  if(selectRet > 0)
	    {	      
	      if(FD_ISSET(swipeFD,&rdCheckSet)!=0)
		{
		  if(ProcessCardSwipe(swipeFD,arduinoFD,&conn) == 0)
		    {
		      activeRun = 0;
		    }
		}
	      if(FD_ISSET(arduinoFD,&rdCheckSet)!=0)
		{
		  if(ProcessArduino(arduinoFD) == 0)
		    {
		      activeRun = 0;
		    }
		}	  
	      if(FD_ISSET(serverFD,&rdCheckSet)!=0)
		{
		  ProcessNewConnection(serverFD);
		}
	    }
	  else if(selectRet == 0)
	    {
	      //timeout!
	    }
	  else if(selectRet == -1)
	    {
	      //Reset things if something went terribly terribly wrong
	      if(
		 (errno == ENODEV) ||
		 (errno == EBADF)
		 )
		{
		  //This stops the active loop and drops back to the main loop
		  activeRun = 0;
		}
	    }
	}
      //===========================================================
      
      //Clean up all of the things!
      close(swipeFD);
      swipeFD = -1;

      close(arduinoFD);
      arduinoFD = -1;

      CloseClients();
      close(serverFD);
      serverFD = -1;

      mysql_close(&conn);
      
      ClearFDs();
    }
  return 0;
}
