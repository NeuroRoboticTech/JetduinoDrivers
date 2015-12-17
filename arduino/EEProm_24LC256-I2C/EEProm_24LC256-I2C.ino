#include <Wire.h>
int data=0;
byte high=0x00, low=0x00l

void setup() {
  Wire.begin();
  Serial.begin(57600);
}

void loop() {

  Wire.beginTransmission(B0101000);
  Wire.send(high);
  Wire.send(low);
  Wire.send(byte(data));
  Wire.endTransmission();
  delay(5);

  Wire.beginTransmission(B0101000);
  Wire.send(high);
  Wire.send(low);
  Wire.endTransmission();
  Wire.requestFrom(B0101000, 1);
  delay(5);
  
  Serial.print(int(Wire.receive()));
  Serial.print(" Address: ");
  Serial.print(int(high));
  Serial.print("   ");
  Serial.println(int(low));
}
