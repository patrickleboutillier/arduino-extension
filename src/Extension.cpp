/*
  Extension.cpp - Library for extending the capabilities of an Arduino using other ones.
  Created by Patrick LeBoutillier, September 2020.
*/


#include "Arduino.h"
#include "Wire.h"
#include "Extension.h"


byte BUFFER[2] ;
byte BUFFER_LEN = 0 ;
byte MASTER_BEGIN = 0 ;


// When slave receives request
void _extension_on_recv(int nb){
  byte cmd = Wire.read() ;
  byte pin = Wire.read() ;
  switch (cmd) {
    case PINMODE: {
      byte mode = Wire.read() ;
      pinMode(pin, mode) ;
      break ;
    }
    case DIGITAL_R: {
      byte value = digitalRead(pin) ;
      BUFFER[0] = value ;
      BUFFER_LEN = 1 ;
      break ;
    }
    case DIGITAL_W: {
      byte value = Wire.read() ;
      digitalWrite(pin, value) ;
      break ;
    }
    case ANALOG_R: {
      int value = analogRead(pin) ;
      BUFFER[0] = value >> 4 ;
      BUFFER[1] = value & 0x0F ;
      BUFFER_LEN = 2 ;
      break ;
    }
    case ANALOG_W: {
      int value1 = Wire.read() ;
      int value2 = Wire.read() ;
      analogWrite(pin, (value1 << 4) | value2) ;
      break ;
    }
  }
}


// When slave sends response
void _extension_on_req(){
  for (byte i = 0 ; i < BUFFER_LEN ; i++){
    Wire.write(BUFFER[i]) ;
  }
}


// Used by the master
Extension::Extension(byte slave){
  if (! MASTER_BEGIN){
    Wire.begin() ;
    Wire.setClock(400000) ;
    MASTER_BEGIN = 1 ;
  }
  
  _slave = slave ;
  _digital_pin_value_cache = NULL ;
}


void Extension::enableDigitalCache(){
  _digital_pin_value_cache = malloc(MAX_PIN) ;
  for (byte i = 0 ; i < MAX_PIN ; i++){
    _digital_pin_value_cache[i] = 255 ;
  }
}


void Extension::pinMode(byte pin, byte mode) {
  Wire.beginTransmission(_slave) ;
  Wire.write(PINMODE) ;
  Wire.write(pin) ;
  Wire.write(mode) ;
  Wire.endTransmission() ;
}


int Extension::digitalRead(byte pin) {
  Wire.beginTransmission(_slave) ;
  Wire.write(DIGITAL_R) ;
  Wire.write(pin) ;
  Wire.endTransmission() ;
  Wire.requestFrom(_slave, (byte)1) ;
  return Wire.read() ;
}


void Extension::digitalWrite(byte pin, byte value) {
  if ((_digital_pin_value_cache == NULL)||(_digital_pin_value_cache[pin] != value)){
    Wire.beginTransmission(_slave) ;
    Wire.write(DIGITAL_W) ;
    Wire.write(pin) ;
    Wire.write(value) ;
    Wire.endTransmission() ;
    _digital_pin_value_cache[pin] = value ; 
  }
}


int Extension::analogRead(byte pin) {
  Wire.beginTransmission(_slave) ;
  Wire.write(ANALOG_R) ;
  Wire.write(pin) ;
  Wire.endTransmission() ;
  Wire.requestFrom(_slave, (byte)2) ;
  int value1 = Wire.read() ;
  int value2 = Wire.read() ;

  return (value1 << 4) | value2 ;
}


void Extension::analogWrite(byte pin, int value) {
  Wire.beginTransmission(_slave) ;
  Wire.write(ANALOG_W) ;
  Wire.write(pin) ;
  Wire.write(value >> 4) ;
  Wire.write(value & 0x0F) ;
  Wire.endTransmission() ;
}

// Used by a slave
static void Extension::slave(byte i2caddr){
  Wire.begin(i2caddr) ;
  Wire.onReceive(_extension_on_recv) ;
  Wire.onRequest(_extension_on_req) ;
}
