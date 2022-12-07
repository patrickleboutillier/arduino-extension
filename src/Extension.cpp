/*
  Extension.cpp - Library for extending the capabilities of an Arduino using other ones.
  Created by Patrick LeBoutillier, September 2020.
*/


#include "Arduino.h"
#include "Wire.h"
#include "Extension.h"


// Pre-declare some function prototypes.
void _extension_on_recv(int nb) ;
void _extension_on_req() ;
byte _read_byte() ;
void _process_request() ;


// Buffer to store return values
volatile byte REQBUF[8] ;
volatile byte REQBUF_LEN = 0 ;
volatile byte RESPBUF[8] ;
volatile byte RESPBUF_LEN = 0 ;
const byte *PGM_DATA = NULL ;
bool MASTER_BEGIN = false ;


// Used by the master
Extension::Extension(byte slave, const char *name, byte max_pin){
  if (! MASTER_BEGIN){
    Wire.begin() ;
    Wire.setClock(400000) ;
    MASTER_BEGIN = true ;
  }
  
  _name[0] = '\0' ;
  if (name != NULL){
    strncpy(_name, name, 15) ;
    _name[15] = '\0' ;
  }

  _slave = slave ;
  _max_pin = max_pin ;
  _digital_pin_value_cache = NULL ;
  _analog_pin_value_cache = NULL ;
}


// Used by a slave
void Extension::slave(byte i2caddr, const byte *pgm_data){
  PGM_DATA = pgm_data ;
  Wire.begin(i2caddr) ;
  Wire.onReceive(_extension_on_recv) ;
  Wire.onRequest(_extension_on_req) ;
}


// Used to check if a slave is alive. 
const char *Extension::name(){
  return _name ;
}


// Used to check if a slave is alive. 
bool Extension::ping(){
  Wire.beginTransmission(_slave) ;
  byte error = Wire.endTransmission() ;
  return !error ;
}


void Extension::enableDigitalCache(){
  _digital_pin_value_cache = (byte *)malloc(_max_pin * sizeof(byte)) ;
  for (byte i = 0 ; i < MAX_PIN ; i++){
    _digital_pin_value_cache[i] = 255 ;
  }
}


void Extension::enableAnalogCache(){
  _analog_pin_value_cache = (int *)malloc(_max_pin * sizeof(int)) ;
  for (byte i = 0 ; i < MAX_PIN ; i++){
    _analog_pin_value_cache[i] = -1 ;
  }
}


void Extension::wait(){
  // delay(1) ;
}


void Extension::pinMode(byte pin, byte mode) {
  Wire.beginTransmission(_slave) ;
  Wire.write(PINMODE) ;
  Wire.write(pin) ;
  Wire.write(mode) ;
  wait() ;
  Wire.endTransmission() ;
}


bool Extension::digitalRead(byte pin) {
  Wire.beginTransmission(_slave) ;
  Wire.write(DIGITAL_R) ;
  Wire.write(pin) ;
  Wire.endTransmission() ;
  wait() ;
  Wire.requestFrom(_slave, (byte)1) ;
  return (bool)_read_byte() ;
}


void Extension::digitalWrite(byte pin, bool value) {
  if ((_digital_pin_value_cache == NULL)||(_digital_pin_value_cache[pin] != value)){
    Wire.beginTransmission(_slave) ;
    Wire.write(DIGITAL_W) ;
    Wire.write(pin) ;
    Wire.write(value) ;
    Wire.endTransmission() ;
    wait() ;   
    if (_digital_pin_value_cache != NULL){
      _digital_pin_value_cache[pin] = value ; 
    }
  }
}


int Extension::analogRead(byte pin) {
  Wire.beginTransmission(_slave) ;
  Wire.write(ANALOG_R) ;
  Wire.write(pin) ;
  Wire.endTransmission() ;
  wait() ;
  Wire.requestFrom(_slave, (byte)2) ;
  int value1 = _read_byte() ;
  int value2 = _read_byte() ;

  return (value1 << 4) | value2 ;
}


void Extension::analogWrite(byte pin, int value) {
  if ((_analog_pin_value_cache == NULL)||(_analog_pin_value_cache[pin] != value)){
    Wire.beginTransmission(_slave) ;
    Wire.write(ANALOG_W) ;
    Wire.write(pin) ;
    Wire.write(value >> 8) ;
    Wire.write(value & 0xFF) ;
    Wire.endTransmission() ;
    wait() ;
    if (_analog_pin_value_cache != NULL){
      _analog_pin_value_cache[pin] = value ; 
    }
  }
}


byte Extension::pgm_read_byte_(int addr) {
  Wire.beginTransmission(_slave) ;
  Wire.write(PGM_READ) ;
  Wire.write(0) ; // pin
  Wire.write(addr >> 8) ;
  Wire.write(addr & 0xFF) ;
  Wire.endTransmission() ;
  wait() ;
  Wire.requestFrom(_slave, (byte)2) ;
  int data = _read_byte() ;

  return data ;
}


// When slave receives request
void _extension_on_recv(int nb){
  for (byte i = 0 ; i < nb ; i++){
    REQBUF[i] = _read_byte() ;
  }
  REQBUF_LEN = nb ;
  _process_request() ;
}


// When slave sends response
void _extension_on_req(){
  if (RESPBUF_LEN){
    for (byte i = 0 ; i < RESPBUF_LEN ; i++){
      Wire.write(RESPBUF[i]) ;
    }
    RESPBUF_LEN = 0 ;
  }
}


byte _read_byte(){
  while (Wire.available() < 1){}
  return Wire.read() ;
}


void _process_request(){
  byte cmd = REQBUF[0] ;
  byte pin = REQBUF[1] ;
  switch (cmd) {
    case PINMODE: {
      byte mode = REQBUF[2] ;
      pinMode(pin, mode) ;
      break ;
    }
    case DIGITAL_R: {
      byte value = digitalRead(pin) ;
      RESPBUF[0] = value ;
      RESPBUF_LEN = 1 ;
      break ;
    }
    case DIGITAL_W: {
      byte value = REQBUF[2] ;
      digitalWrite(pin, value) ;
      break ;
    }
    case ANALOG_R: {
      int value = analogRead(pin) ;
      RESPBUF[0] = value >> 8 ;
      RESPBUF[1] = value & 0xFF ;
      RESPBUF_LEN = 2 ;
      break ;
    }
    case ANALOG_W: {
      int value1 = REQBUF[2] ;
      int value2 = REQBUF[3] ;
      analogWrite(pin, (value1 << 4) | value2) ;
      break ;
    }
    case PGM_READ: {
      byte data = 0 ;
      if (PGM_DATA != NULL){
        byte addrh = REQBUF[2] ;
        byte addrl = REQBUF[3] ;
        data = pgm_read_byte(PGM_DATA + (addrh << 8 | addrl)) ;
      }
      RESPBUF[0] = data ;
      RESPBUF_LEN = 1 ;
      break ;
    }
  }
}





