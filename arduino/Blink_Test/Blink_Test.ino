/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */


// the setup function runs once when you press reset or power the board
void setup() {
  for(int i=23; i<30; i++)
    pinMode(i, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  for(int i=23; i<30; i++)
    digitalWrite(i, HIGH);
    
  delay(3000);              // wait for a second

  for(int i=23; i<30; i++)
    digitalWrite(i, LOW);
  
  delay(3000);              // wait for a second
}
