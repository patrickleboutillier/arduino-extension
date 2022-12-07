/*
  Extension.h - Library for extending the capabilities of an Arduino using other ones.
  Created by Patrick LeBoutillier, September 2020.
*/
#ifndef Extension_h
#define Extension_h


#include "Arduino.h"


#define PINMODE     1
#define DIGITAL_R   2
#define DIGITAL_W   3
#define ANALOG_R    4
#define ANALOG_W    5
#define PGM_READ    6

#define MAX_PIN     32


class Extension {
  public:
    Extension(byte slave, const char *name = NULL, byte max_pin = MAX_PIN) ;
    static void slave(byte i2caddr, byte *pgm_data = NULL) ;
    static void loop() ;
    void pinMode(byte pin, byte mode) ;
    bool digitalRead(byte pin) ;
    void digitalWrite(byte pin, bool value) ;
    int analogRead(byte pin) ;
    void analogWrite(byte pin, int value) ;
    byte pgm_read_byte_(int addr) ;
    void enableDigitalCache() ;
    void enableAnalogCache() ;
    bool ping() ;
    const char *name() ;
  private:
    void wait() ;
    char _name[16] ;
    byte _slave ;
    byte _max_pin ;
    byte *_digital_pin_value_cache ;
    int *_analog_pin_value_cache ;
} ;


#endif
