#include "AnimatSerial.h"

AnimatSerial animat(&Serial, 10, 10);

void setup() {
  SerialUSB.begin(57600);
  while(!SerialUSB);
  SerialUSB.println("Starting setup");
  
  animat.begin(38400);  
}

void loop() {

  SerialUSB.println("Adding data");
  animat.addData(1, 22.5);

  SerialUSB.println("Writing data");
  animat.writeMsgs();

  delay(3000);
}
