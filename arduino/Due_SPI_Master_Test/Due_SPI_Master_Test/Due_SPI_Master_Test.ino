#include <SPI.h>

void setup() {
  Serial.begin(57600);
  while(!Serial);
  Serial.println("Setting up");

  SPI.begin(10);
  //SPI.setClockDivider(10, 84);
}

void loop() {
  Serial.println("Starting Transfer");
  SPI.transfer(10, 0xAA, SPI_CONTINUE);
  SPI.transfer(10, 0xBB, SPI_CONTINUE);
  byte response1 = SPI.transfer(10, 0xCC);
  delay(1000);
}
