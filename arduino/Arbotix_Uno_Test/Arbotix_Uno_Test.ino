/***************************
 * AXSimpleTest
 * This sketch sends positional commands to the AX servo 
 * attached to it - the servo must set to ID # 1
 * The sketch will send a value, i, to the servo.
 * 'For' loops are used to increment and decrement the value of 'i'
 ***************************/
//#define ARBOTIX_WITH_RX

//import ax12 library to send DYNAMIXEL commands
#include <ax12.h>

void setup()
{
  //pinMode(1, OUTPUT);
  //digitalWrite(1, HIGH);
  
  SetPosition(1,450); //set the position of servo # 1 to '0'
  delay(1000);//wait for servo to move
}

void loop()
{
  SetPosition(1,650);
  delay(1000);//wait for servo to move
  
  SetPosition(1,450);
  delay(1000);//wait for servo to move  
}



