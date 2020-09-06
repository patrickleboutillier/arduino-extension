#include "Extension.h"

Extension sl1(5) ;
Extension sl2(6) ;

void setup(){
  Serial.begin(9600) ;
  sl1.pinMode(2, INPUT) ;
  sl2.pinMode(2, INPUT) ;
}

void loop(){
  byte v = sl1.digitalRead(2) ;
  Serial.print("Pin 2 of slave 1 is: ") ;
  Serial.println(v) ;
  v = sl2.digitalRead(2) ;
  Serial.print("Pin 2 of slave 2 is: ") ;
  Serial.println(v) ;
  
  // ...
}
