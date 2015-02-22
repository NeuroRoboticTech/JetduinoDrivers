#include "SPI.h" // necessary library

void setup(){
  // initialize the bus for the device on pin 4
  SPI.begin();
  // Set clock divider on pin 4 to 4
  SPI.setClockDivider(2); 
}

void loop(){
  byte response = SPI.transfer(0x55);
  delay(1000);
}
