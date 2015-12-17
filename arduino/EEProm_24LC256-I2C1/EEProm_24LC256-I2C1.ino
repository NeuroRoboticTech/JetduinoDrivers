#include "Wire.h"
int data=0x5;
byte high=0x00, low=0x00;

void setup() {
  Wire1.begin();
  Serial.begin(57600);
}

void loop() {

  Wire1.beginTransmission(B1010000);
  Wire1.write(high);
  Wire1.write(low);
  Wire1.write(byte(data));
  Wire1.endTransmission();
  delay(5);

  Wire1.beginTransmission(B1010000);
  Wire1.write(high);
  Wire1.write(low);
  Wire1.endTransmission();
  Wire1.requestFrom(B1010000, 1);
  delay(5);
  
  Serial.print(int(Wire1.read()), HEX);
  Serial.print(" Address: ");
  Serial.print(int(high));
  Serial.print("   ");
  Serial.println(int(low));
  
  data = data +1;
  if(data == 255)
    data = 0;
  
  delay(2000);
}
