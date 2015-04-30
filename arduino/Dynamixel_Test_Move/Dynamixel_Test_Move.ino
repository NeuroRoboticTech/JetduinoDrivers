#include <DynamixelSerial1.h>

void setup () {
  #define SYS_BOARD_PLLAR (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(39UL) | CKGR_PLLAR_PLLACOUNT(100UL) | CKGR_PLLAR_DIVA(3UL))
  #define SYS_BOARD_MCKR ( PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK)
          
  /* Set FWS according to SYS_BOARD_MCKR configuration */
  EFC0->EEFC_FMR = EEFC_FMR_FWS(4); //4 waitstate flash access
  EFC1->EEFC_FMR = EEFC_FMR_FWS(4);
  
  /* Initialize PLLA to 114MHz */
  PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
  while (!(PMC->PMC_SR & PMC_SR_LOCKA)) {}
  
  PMC->PMC_MCKR = SYS_BOARD_MCKR;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {} 
 
  
  Serial.begin(57600);
  //_pUart->UART_BRGR = (SystemCoreClock / dwBaudRate) >> 4 ;  
  
  while(!Serial);
  Serial.println("Starting setup");
  
  Dynamixel.begin (1000000, 2); 
  //pinMode(2, OUTPUT);
  delay (3000); 
  
  Serial.print(F("F_CPU = "));
  Serial.print(F_CPU,DEC);  
} 

void ReadPos(int servo)
{  
  for(int i=0; i<10; i++)
  {
    int Position = Dynamixel.readPosition(1);       // Request and Print the Position 
    Serial.print("Position: ");
    Serial.print(Position);
    Serial.println(" of 1023 resolution");
    delay(50);
  }
}

void loop () { 
  //digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  //Serial.println("Move 450");
  Dynamixel.move (1,450); 

  //ReadPos(1);
  
  delay (2000); 
  //digitalWrite(2, LOW);   // turn the LED on (HIGH is the voltage level)
  //Serial.println("Move 650");
  Dynamixel.move (1, 650); 

  //ReadPos(1);

  delay (2000); 
} 

