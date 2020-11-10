/*
  Extension.h - Library for extending the capabilities of an Arduino using other ones.
  Created by Patrick LeBoutillier, September 2020.
*/
#ifndef Extension_h
#define Extension_h


#include "Arduino.h"


#define PINMODE     1
#define DIGITAL_R   2
#define DIGITAL_RN  3
#define DIGITAL_W   4
#define ANALOG_R    5
#define ANALOG_W    6

#define MAX_PIN     128


class Extension {
  public:
    Extension(byte slave, byte max_pin = MAX_PIN) ;
    static void slave(byte i2caddr) ;
    static void loop() ;
    void pinMode(byte pin, byte mode) ;
    int digitalRead(byte pin) ;
    int digitalReadN(byte start_pin, byte nb_pins, int up_down = 0) ;
    void digitalWrite(byte pin, byte value) ;
    void digitalWriteN(byte start_pin, byte nb_pins, byte value, int up_down = 0) ;
    int analogRead(byte pin) ;
    void analogWrite(byte pin, int value) ;
    void enableDigitalCache() ;
    void enableAnalogCache() ;
  private:
    byte _slave ;
    byte _max_pin ;
    byte *_digital_pin_value_cache ;
    int *_analog_pin_value_cache ;
} ;


#endif
