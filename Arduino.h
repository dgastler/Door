#ifndef __ARDUINO_H__
#define __ARDUINO_H__

#include "SerialLink.h"
#include "DoorMessage.h"

void OpenDoor(int fd);
void CloseDoor(int fd);
int ProcessArduino(int arduinoFD);


#endif
