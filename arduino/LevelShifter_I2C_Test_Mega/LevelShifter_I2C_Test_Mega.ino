#include <Wire.h>

void setup()
{
  Wire.begin(1);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
}

void loop()
{
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write("Slave 1"); // respond with message of 6 bytes
                       // as expected by master
}
