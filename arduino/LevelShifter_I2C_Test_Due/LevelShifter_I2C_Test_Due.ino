#include <Wire.h>

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void ReadData(int addr)
{
  Serial.print("Requesting from Slave ");
  Serial.println(addr);

  Wire.requestFrom(addr, 7);    // request 6 bytes from slave device #2

  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  Serial.println("");  
}

void loop()
{  
  ReadData(1);
  ReadData(2);
  
  delay(500);
}
