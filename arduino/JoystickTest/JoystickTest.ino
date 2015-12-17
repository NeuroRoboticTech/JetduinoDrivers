// These constants won't change.  They're used to give names
// to the pins used:
const int x1Pin = A0;  // Analog input pin that the potentiometer is attached to
const int y1Pin = A1;  // Analog input pin that the potentiometer is attached to
const int x2Pin = A2;  // Analog input pin that the potentiometer is attached to
const int y2Pin = A3;  // Analog input pin that the potentiometer is attached to
const int buttonPin = 4;  // Analog input pin that the potentiometer is attached to

int joyX1 = 0;        // value read from the pot
int joyY1 = 0;        // value read from the pot
int joyX2 = 0;        // value read from the pot
int joyY2 = 0;        // value read from the pot
int button = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(57600);
  pinMode(buttonPin, INPUT);
}

void loop() {
  // read the analog in value:
  joyX1 = analogRead(x1Pin);
  joyY1 = analogRead(y1Pin);
  joyX2 = analogRead(x2Pin);
  joyY2 = analogRead(y2Pin);
  button = digitalRead(buttonPin);
  

  // print the results to the serial monitor:
  Serial.print("[");
  Serial.print(joyX1);
  Serial.print(", ");
  Serial.print(joyY1);
  Serial.print("]   ");
  Serial.print("[");
  Serial.print(joyX2);
  Serial.print(", ");
  Serial.print(joyY2);
  Serial.print("],  ");
  Serial.println(button);

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(100);
}
