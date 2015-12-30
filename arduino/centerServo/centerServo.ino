/***********************************************************************************
 *   ___________
 *  |     __    |           RobotGeek Servo Centering Tool
 *  |   /    \  |
 *  |  |      | |
 *  |   \ __ /  |
 *  |           |
 *  |           |
 *  |___________|
 *
 *  The following sketch send a signal to all 6 of the Geekduino/Arduino's pins to
 *  that sets any attached servo to 90 degrees, centering the servo.
 *    
 *  Wiring
 *    A servo can be connected to any of the PWM ports: 3,5,6,9,10 or 11
 *    Mutiple servos can be centered at one time, but the number of servos you can attach
 *    may be limited by your power supply. For best performance limit 
 *    testing to one servo at a time. 
 *    For the centering process, the power jumpers should be set to '5v' if you are powering
 *    the Geekduino/Arduino from USB. 
 ***********************************************************************************/
#include <Servo.h>   //include the servo library to control the RobotGeek Servos

Servo servo;   
int servoID = 2;
int servoCenter = 90;
 
void setup()
{ 
  servo.attach(servoID);    //attach pan servo object on pin i
  servo.write(servoCenter);               // sets the servo position to 90 degress, the center of the servo. This will also serve as 'stopped' for continous rotation servos
}
 
void loop()
{
  //delay(1000);
  //servo.write(0);
  //delay(1000);
  //servo.write(180);
}
 
 
 
 
 
