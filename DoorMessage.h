#ifndef __DOORMESSAGE__
#define __DOORMESSAGE__
#include <stdint.h>

typedef struct 
{
  uint8_t commandCode;
  uint8_t data[253];
} dmCommand;

typedef struct 
{
  uint8_t magicWord;
  uint8_t size;
  dmCommand command;
} DoorMessage;

typedef struct 
{
  uint8_t pin;
  uint8_t val;
} dmBit;

#endif
