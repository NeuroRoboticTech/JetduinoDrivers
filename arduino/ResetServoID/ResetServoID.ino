#include <DynamixelSerial.h>

#define SERVO_ID 19

DynamixelSerial Dynamixel(&Serial2);

void setup() {
  #define SYS_BOARD_PLLAR (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(39UL) | CKGR_PLLAR_PLLACOUNT(100UL) | CKGR_PLLAR_DIVA(3UL))
  #define SYS_BOARD_MCKR ( PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK)
          
  // Set FWS according to SYS_BOARD_MCKR configuration 
  EFC0->EEFC_FMR = EEFC_FMR_FWS(4); //4 waitstate flash access
  EFC1->EEFC_FMR = EEFC_FMR_FWS(4);
  
  // Initialize PLLA to 114MHz 
  PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
  while (!(PMC->PMC_SR & PMC_SR_LOCKA)) {}
  
  PMC->PMC_MCKR = SYS_BOARD_MCKR;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {} 
  
  Serial.begin(57600);
  
  while(!Serial);
  Serial.println("Starting setup");
  
  pinMode(22, OUTPUT);
  Dynamixel.begin (1000000, 22); 

  delay(20);
  //Dynamixel.reset(SERVO_ID);
  delay(20);
  //Dynamixel.setStatusReturnLevel(SERVO_ID, 1);
  delay(20);
  
  //Dynamixel.setID(19, SERVO_ID);
  //delay(500);
  
  Serial.println("Finished Setup");
}

void loop() {
  Serial.println("Move 450");
  Dynamixel.moveSpeed (SERVO_ID, 712, 150);
  delay(3000);
  Serial.println("Move 650");
  Dynamixel.moveSpeed (SERVO_ID, 312, 150);
  delay(3000);  
}
