/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int d2 = 2;
int d3 = 3;
int d4 = 4;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  pinMode(d2, OUTPUT);     
  pinMode(d3, OUTPUT);     
  pinMode(d4, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(d2, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(d3, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(d4, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(3000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(d2, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(d3, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(d4, LOW);    // turn the LED off by making the voltage LOW
  delay(3000);               // wait for a second
}
