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
