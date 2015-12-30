/***************************
 * AXSimpleTest
 * This sketch sends positional commands to the AX servo 
 * attached to it - the servo must set to ID # 1
 * The sketch will send a value, i, to the servo.
 * 'For' loops are used to increment and decrement the value of 'i'
 ***************************/

//import ax12 library to send DYNAMIXEL commands
#include <ax12.h>
#include <SoftwareSerial.h>

#define COMMANDER_RX_PIN 2	 
#define COMMANDER_TX_PIN 3

int pos = -1;
SoftwareSerial debugSerial = SoftwareSerial(COMMANDER_RX_PIN, COMMANDER_TX_PIN); // RX, TX

void setup()
{
  debugSerial.begin(57600);
  debugSerial.println("Starting setup");

  //SetCWLimit(1, 0);
  //delay(100);
  //SetCCWLimit(1, 1023);
  //delay(100);

  SetReturnDelayTime(1, 100);
  delay(100);

  int ret_delay = GetReturnDelayTime(1);
  delay(100);
  
  //int cw_limit = GetCWLimit(1);
  //delay(100);

  //int ccw_limit = GetCCWLimit(1);
    
  debugSerial.print("ret_delay: ");
  debugSerial.println(ret_delay);
  //debugSerial.print("CW Limit: ");
  //debugSerial.println(cw_limit);
  //debugSerial.print("CCW Limit: ");
  //debugSerial.println(ccw_limit);
  
  
  SetPosition(1, 512); //set the position of servo # 1 to '0'
  //delay(100);//wait for servo to move
}

void loop()
{
  delay(2000);
  pos = GetPosition(1);
  debugSerial.print("Pos: ");
  debugSerial.println(pos);

  /*
  //increment from 0 to 1023
  for(int i=0;i<1024;i++)
  {
    SetPosition(1,i); //set the position of servo #1 to the current value of 'i'
    delay(10);//wait for servo to move
  }
  //decrement from 1024 to 0
  for(int i=1024;i>0;i--)
  {
    SetPosition(1,i);//set the position of servo #1 to the current value of 'i'
    delay(10);//wait for servo to move
  }
  */
}



