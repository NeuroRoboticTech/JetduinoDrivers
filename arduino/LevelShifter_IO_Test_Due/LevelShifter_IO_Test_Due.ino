
int val = 0;     

void setup() {
  Serial.begin(57600);
  while(!Serial);
  Serial.println("Starting setup");

  for(int i=22; i<40; i++) 
    pinMode(i, OUTPUT);
}

void loop() {
  Serial.print("Setting: ");
  Serial.println(val);
  for(int i=22; i<40; i++) 
    digitalWrite(i, val);    // sets the LED to the button's value

  val = ~val;
  
  delay(2000);
}


