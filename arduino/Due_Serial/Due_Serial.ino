int incomingByte = 0;   // for incoming serial data

void setup() {
  Serial.begin(57600);  // start serial for output
  while(!Serial);
  Serial.println("Starting setup");
  
  Serial1.begin(1000000);     // opens serial port, sets data rate to 9600 bps
  while(!Serial1);
}

void loop() {

  delay(2000);
  Serial1.print("ABC");
        // send data only when you receive data:
        //Serial.print("Avail: ");
        //Serial.println(Serial1.available());
        
        //if (Serial1.available() > 0) {
        //        // read the incoming byte:
        //        incomingByte = Serial1.read();
        //        Serial.println(incomingByte, DEC);
        //}
}
