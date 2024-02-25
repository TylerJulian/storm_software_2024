#ifndef ERROR_H
#define ERROR_H
#include "Arduino.h"

extern SerialUSB Serial;
void report_error(String error) {

  if (Serial) 
  {
    Serial.println(error);
  }
}

#endif