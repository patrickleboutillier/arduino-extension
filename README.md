# arduino-extension
Library for extending the capabilities of an Arduino with other identical ones. 

# How To #

# 1- Program your slaves #
Each slave device must be programmed using a unique id (i.e. IC2 address). Here is what you slave code should look like:

````
#include "Extension.h"

void setup(){
  Extension::slave(5) ; // use a different address for each slave
}

void loop(){  
}
````

# 2- Wire your setup
Devices must be wired to create and IC2 bus. Here is an example using 2 slaves:

![](./examples/Extension.png)

You should now be able to power the whole chain through the master.

# 3- Program your master
You can now program the master and extend it's capabilities through the slaves:
````
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
````
