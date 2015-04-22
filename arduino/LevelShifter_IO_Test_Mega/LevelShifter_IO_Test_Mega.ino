
int val = 0;     

void setup() {
  Serial.begin(57600);
  while(!Serial);
  Serial.println("Starting setup");

  for(int i=22; i<35; i++) 
    pinMode(i, INPUT);
}

void loop() {
  Serial.print("Getting: ");
  for(int i=22; i<35; i++) 
  {
    val = digitalRead(i);
    Serial.print(val);
    Serial.print(", ");
  }
  Serial.println("");
  
  delay(500);
}


