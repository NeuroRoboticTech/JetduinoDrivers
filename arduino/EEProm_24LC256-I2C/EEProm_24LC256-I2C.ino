#include "Wire.h"
int data=0x5;
byte high=0x00, low=0x01;

void setup() {
  Wire.begin();
  Serial.begin(57600);
}

void loop() {

  Wire.beginTransmission(B1010000);
  Wire.write(high);
  Wire.write(low);
  Wire.write(byte(data));
  Wire.endTransmission();
  delay(5);

  Wire.beginTransmission(B1010000);
  Wire.write(high);
  Wire.write(low);
  Wire.endTransmission();
  Wire.requestFrom(B1010000, 1);
  delay(5);
  
  Serial.print(int(Wire.read()), HEX);
  Serial.print(" Address: ");
  Serial.print(int(high));
  Serial.print("   ");
  Serial.println(int(low));
  
  data = data +1;
  if(data == 255)
    data = 0;
  
  delay(2000);
}
