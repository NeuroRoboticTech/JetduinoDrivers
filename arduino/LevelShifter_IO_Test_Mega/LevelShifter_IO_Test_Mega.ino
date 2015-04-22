
int val = 0;     

void setup() {
  Serial.begin(57600);
  Serial.println("Starting setup");

  for(int i=4; i<13; i++) 
    pinMode(i, INPUT);
}

void loop() {
  Serial.print("Getting: ");
  for(int i=4; i<13; i++) 
  {
    val = digitalRead(i);
    Serial.print(val);
    Serial.print(", ");
  }
  Serial.println("");
  
  delay(500);
}


