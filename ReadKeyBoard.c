#include "ReadKeyBoard.h"

//Running values of the capslock/shift status
static int shiftKeyPressed = 0;
static int capsLockPressed = 0;


int ReadKeyboard(int fd,char * buffer,int bufferSize)
{
  //Check for NULL pointers and zero sized strings
  if((buffer == NULL))
    {
      return RKB_NULL_POINTER;
    }
  else if(bufferSize == 0)
    {
      return RKB_ZERO_SIZE;
    }

  //read structure for keyboard event
  struct input_event ev;
  ssize_t charsRead = 0;
  
  fd_set rdSet,rdCheckSet;
  struct timeval wait = {3,0};  //read can't take more than 3 seconds
  FD_ZERO(&rdSet);
  FD_SET(fd,&rdSet);
  do
    {
      //Listen for fd
      rdCheckSet = rdSet;
      int selectRet = select(fd+1,&rdCheckSet,NULL,NULL,&wait);
      if(selectRet > 0)
	{
	  int readReturn = -1;
	  //read one event and fail on error
	  if((readReturn = read(fd,&ev,sizeof(ev))) != sizeof(ev))
	    {
	      return RKB_BAD_READ;
	    }
	  //Pass back errors from read (maybe set errno in the future? )
	  else if(readReturn == -1)
	    {
	      return -1;
	    }
	  //If this is a key event
	  else
	    {
	      //Process the event and if it ends up being a new char, move forward in buffer
	      if(ProcessEvent(&ev,buffer+charsRead) == 1)
		{	  
		  //if the read char was '\n' break out of the read loop
		  //incriment charsRead in either case
		  if(buffer[charsRead++] == '\n')
		    {
		      break;
		    }
		}	  
	    }
	}
      else if(selectRet == 0)
	{
	  printf("Keyboard timeout.\n");
	  return 0;
	}
      else
	{
	  printf("Keyboard error(%d):%s\n",errno,strerror(errno));
	  return -1;
	}
    }while(charsRead < bufferSize);
  
  return charsRead;
}

//Process a keyboard event.
//Return 1 if a valid ASCII character was read
//Return 0 otherwise. 
int ProcessEvent(struct input_event * ev, char * retChar)
{
  int ret = 0;
  if(ev->type == EV_KEY)
    {
      //Process key presses for shift/capslock and alphanumerics
      if ((ev->value == KEY_PRESS) || 
	  (ev->value == KEY_KEEPING_PRESSED)) 
	{
	  switch (ev->code)
	    {
	    case KEY_RIGHTSHIFT:
	    case KEY_LEFTSHIFT:
	      shiftKeyPressed = 1;
	      break;
	    case KEY_CAPSLOCK:
	      shiftKeyPressed = 1;
	      break;	      
	    case KEY_ENTER:   
	      //Enter key -> '\n'
	      *retChar = '\n';
	      ret = 1;
	      break;
	    default:	      
	      //try to convert the key to ASCII
	      *retChar = ConvertKeyToASCII(ev);
	      //only say we have a new char if we managed to convert it to an alphanumeric+(shift numeric)
	      if(*retChar != 0)
		ret = 1;
	      break;
	    }
	}
      //Process keyboard releases to look for shift/caps-lock
      else if (ev->value == KEY_RELEASE)
	{
	  switch (ev->code)
	    {
	    case KEY_RIGHTSHIFT:
	    case KEY_LEFTSHIFT:
	      shiftKeyPressed = 0;
	      break;
	    case KEY_CAPSLOCK:
	      shiftKeyPressed = 0;
	      break;
	    default:
	      break;
	    }
	}
    }
  return ret;
}


char ConvertKeyToASCII(struct input_event * ev)
{  
  //Process the numeric keys
  if((ev->code >= KEY_1) &&
     (ev->code <= KEY_0))
    {
      return(ConvertNumerics(ev->code));
    }
  //process the alpha keys
  else if(
	  ((ev->code >= KEY_Q) &&
	   (ev->code <= KEY_P)) ||
	  ((ev->code >= KEY_A) &&
	   (ev->code <= KEY_L)) ||
	  ((ev->code >= KEY_Z) &&
	   (ev->code <= KEY_M)))
    {
      return(ConvertAlphas(ev->code));
    }     	  
  //return 0 char if not recogized
  return 0;
}

char ConvertNumerics(int value)
{
  //Kindof hacky, but I couldn't find an elegant way of applying shift to the numerics, 
  //so it is hard coded

  if(!((capsLockPressed^shiftKeyPressed)&0x1))  //If the shift key is effectively pressed
    {
      switch (value)
	{
	case KEY_1:
	  return '1';
	  break;	 
	case KEY_2:
	  return '2';
	  break;	 
	case KEY_3:
	  return '3';
	  break;	 
	case KEY_4:
	  return '4';
	  break;	 
	case KEY_5:
	  return '5';
	  break;	 
	case KEY_6:
	  return '6';
	  break;	 
	case KEY_7:
	  return '7';
	  break;	 
	case KEY_8:
	  return '8';
	  break;	 
	case KEY_9:
	  return '9';
	  break;	 
	case KEY_0:
	default:
	  return '0';
	  break;	 
	}
    }
  else
    {
      switch (value)
	{
	case KEY_1:
	  return '!';
	  break;	 
	case KEY_2:
	  return '@';
	  break;	 
	case KEY_3:
	  return '#';
	  break;	 
	case KEY_4:
	  return '$';
	  break;	 
	case KEY_5:
	  return '%';
	  break;	 
	case KEY_6:
	  return '^';
	  break;	 
	case KEY_7:
	  return '&';
	  break;	 
	case KEY_8:
	  return '*';
	  break;	 
	case KEY_9:
	  return '(';
	  break;	 
	case KEY_0:
	default:
	  return ')';
	  break;	 
	}
    }
  return 0;
}

char ConvertAlphas(int value)
{
  //luckily, the lower and upper case alphas are 32 apart in the ascii table. 
  int shift = ((capsLockPressed^shiftKeyPressed)&0x1)*32;
  switch (value)
    {
    case KEY_A:
      return ('a' - shift);
      break;	   
    case KEY_B:
      return ('b' - shift);
      break;	   
    case KEY_C:
      return ('c' - shift);
      break;	   
    case KEY_D:
      return ('d' - shift);
      break;	   
    case KEY_E:
      return ('e' - shift);
      break;	   
    case KEY_F:
      return ('f' - shift);
      break;	   
    case KEY_G:
      return ('g' - shift);
      break;	   
    case KEY_H:
      return ('h' - shift);
      break;	   
    case KEY_I:
      return ('i' - shift);
      break;	   
    case KEY_J:
      return ('j' - shift);
      break;	   
    case KEY_K:
      return ('k' - shift);
      break;	   
    case KEY_L:
      return ('l' - shift);
      break;	   
    case KEY_M:
      return ('m' - shift);
      break;	   
    case KEY_N:
      return ('n' - shift);
      break;	   
    case KEY_O:
      return ('o' - shift);
      break;	   
    case KEY_P:
      return ('p' - shift);
      break;	   
    case KEY_Q:
      return ('q' - shift);
      break;	   
    case KEY_R:
      return ('r' - shift);
      break;	   
    case KEY_S:
      return ('s' - shift);
      break;	   
    case KEY_T:
      return ('t' - shift);
      break;	   
    case KEY_U:
      return ('u' - shift);
      break;	   
    case KEY_V:
      return ('v' - shift);
      break;	   
    case KEY_W:
      return ('w' - shift);
      break;	   
    case KEY_Y:
      return ('x' - shift);
      break;	   
    case KEY_X:
      return ('y' - shift);
      break;	   
    case KEY_Z:
      return ('z' - shift);
      break;	   
    }
  return 0;
}

