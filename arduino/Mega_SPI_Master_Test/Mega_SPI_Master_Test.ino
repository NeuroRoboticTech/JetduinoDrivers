 
#include "SPI.h" // necessary library
int ss=53; // using digital pin 10 for SPI slave select
int del=200; // used for various delays
 
void setup()
{
  pinMode(ss, OUTPUT); // we use this for SS pin
  SPI.begin(); // wake up the SPI bus.
  //SPI.setBitOrder(MSBFIRST);
}
 
void setValue(int value)
{
  digitalWrite(ss, LOW);
  SPI.transfer(0); // send command byte
  SPI.transfer(value); // send value (0~255)
  SPI.transfer(value+1); // send value (0~255)
  SPI.transfer(value+2); // send value (0~255)
  SPI.transfer(value+3); // send value (0~255)
  digitalWrite(ss, HIGH);
}
 
void loop()
{
  for (int a=0; a<256; a++)
  {
    setValue(a);
    delay(del);
  }
  for (int a=255; a>=0; --a)
  {
    setValue(a);
    delay(del);
  }
}
