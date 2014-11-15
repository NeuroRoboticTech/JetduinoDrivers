#include <SPI.h>
#include <sys\attribs.h>

void setup (void) {
  int rData;
  
  Serial.begin(38400); // opens serial port, sets data rate to 9600 bps
  Serial.println("Setting up SPI slave.");

  IEC1CLR=0b11100000; // IEC1 for SPI2, IEC0 for SPI1 etcetra...
  SPI2CON = 0; 
  rData=SPI2BUF; 
  IFS1CLR = 0b11100000; 
  IPC7CLR = 0x1f000000; 
  IPC7SET = 0x0d000000; 
  IEC1SET = 0b11100000;
  //IEC1SET = 0b10000000; //Only trigger interrupt when data received
  SPI2STATCLR=0x40;
  SPI2CON=0x8000; // SPI ON, 8 bits transfer, SLAVE mode
  SPI2BUF=66;

  Serial.println("Waiting for data.");
}

void loop (void)
{
  delay(100);
  Serial.println("Loop");
}  

extern "C"
{
  // SPI interrupt routine
  void __ISR(_SPI_2_VECTOR, ipl7) __SPI2Interrupt(void)
  {
    char c = SPI2BUF;  // grab byte from SPI Data Register
    
    if(c != 0)
    {
      Serial.print("SPI Interrupt Triggered: ");
      Serial.println(c);
      SPI2BUF = 0;
    }
    
    IFS1CLR = 0b11100000;
  }  // end of interrupt routine SPI_STC_vect
}
