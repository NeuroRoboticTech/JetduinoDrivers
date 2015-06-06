/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 
 
Servo servos[12];  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  for(int i=0, s=2; i<12; i++, s++)
    servos[i].attach(s);  // attaches the servo on pin 9 to the servo object 
} 
 
void loop() 
{ 
  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    for(int i=0; i<12; i++)
      servos[i].write(pos);
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    for(int i=0; i<12; i++)
      servos[i].write(pos);
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
} 

