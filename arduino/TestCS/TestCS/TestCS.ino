
static byte cs_state = 0;

#define SS 10

void setup() {
  Serial.begin(57600);
  while(!Serial);
  Serial.println("Starting setup");
  
  // put your setup code here, to run once:
  pinMode(10, INPUT);
  
  cs_state = digitalRead(SS);
  Serial.print("CS: ");
  Serial.println(cs_state);
}

void loop() {
  byte cs_val = digitalRead(SS);
  
  if(cs_val != cs_state)
  {
    if(cs_val == 1)
      Serial.println("CS: 1");
    else
      Serial.println("CS: 0");
      
    cs_state = cs_val;
  }
}
