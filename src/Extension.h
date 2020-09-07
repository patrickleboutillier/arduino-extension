/*
  Extension.h - Library for extending the capabilities of an Arduino using other ones.
  Created by Patrick LeBoutillier, September 2020.
*/
#ifndef Extension_h
#define Extension_h


#include "Arduino.h"


#define PINMODE     1
#define ANALOG_R    4
#define ANALOG_W    5


class Extension {
  public:
    Extension(byte slave) ;
    static void slave(byte i2caddr) ;
    void pinMode(byte pin, byte mode) ;
    int digitalRead(byte pin) ;
    void digitalWrite(byte pin, byte value) ;
    int analogRead(byte pin) ;
    void analogWrite(byte pin, int value) ;
  private:
    byte _slave ;
} ;


#endif
