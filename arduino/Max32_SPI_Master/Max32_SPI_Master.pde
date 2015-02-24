#include <SPI.h>
#include <sys\attribs.h>

void setup (void) {
  int rData;

  Serial.begin(38400); // opens serial port, sets data rate to 9600 bps
  Serial.println("Setting up SPI msater.");

  pinMode(53, OUTPUT);     
  digitalWrite(53, 1);

  IEC1CLR=0b11100000; // IEC1 for SPI2, IEC0 for SPI1 etcetra...
  SPI2CON = 0; 
  rData=SPI2BUF; 
  IFS1CLR = 0b11100000; 
  IPC7CLR = 0x1f000000; 
  IPC7SET = 0x0d000000; 
  //IEC1SET = 0b11100000;
  //IEC1SET = 0b10000000; //Only trigger interrupt when data received

  SPI2BRG=0x1;
  SPI2STATCLR=0x40;
  SPI2CON=0x8220; // SPI ON, 8 bits transfer, Master mode mode
  SPI2BUF=0x55;

  Serial.println("Waiting for data.");
}

void loop (void)
{
  digitalWrite(53, 0);      
  Serial.println("Write: 0x55");
  SPI2BUF=0x55;
  delay(20);
  digitalWrite(53, 1);      

  //if(SPI2STAT & 0x01)
  //{
  //  int out = SPI2BUF;
  //  Serial.print("Read: 0x");
  //  Serial.println(out, HEX);  
  //}
  //else
  //  Serial.println("Read Buffer not full");

  delay(1000);
}  
/*
extern "C"
 {
 // SPI interrupt routine
 void __ISR(_SPI_2_VECTOR, ipl7) __SPI2Interrupt(void)
 {
 char c = 0; //SPI2BUF;  // grab byte from SPI Data Register
 
 //if(c != 0)
 // {
 //Serial.print("SPI Interrupt Triggered: ");
 Serial.print("Read: 0x");
 Serial.println(c, HEX);  
 //SPI2BUF = 0;
 //}
 
 IFS1CLR = 0b11100000;
 }  // end of interrupt routine SPI_STC_vect
 }
 */

