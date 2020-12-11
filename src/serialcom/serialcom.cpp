#include "serialcom.h"

void poll_serial(double tc_temperature)
{
  char commandBuffer[51];
  
  //Check if a serial communication is requested
  while((bool)Serial.available())
  {
    //Read bytes until the start character (:) is encounterd
    if(Serial.read() == ':')
    {
      //Read up to 50 bytes into the command buffer until a line feed is encountered
      memset(commandBuffer, 0, 50);
      Serial.readBytesUntil(0x0A, commandBuffer, 50);

      //Check if a data read is requested
      if(!(bool)strncmp(commandBuffer, "read?", 5))
      {
        char printString[15];
        dtostre(tc_temperature, printString, 7, DTOSTR_UPPERCASE | DTOSTR_PLUS_SIGN);
        Serial.println(printString);
      }
    }
  }
}