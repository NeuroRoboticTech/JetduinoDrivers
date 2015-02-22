// SPI slave for DUE, hack
//  master will lower CS and manage CLK
// MISO to MISO  MOSI to MOSI   CLK to CLK   CS to CS, common ground
//   http://forum.arduino.cc/index.php/topic,157203.0.html
// core hardware/arduino/sam/system/libsam/source/spi.c
// TODO:  C class, interrupt, FIFO/16-bit, DMA?

#include <SPI.h>

static int iInVal = 0;
static int iOutVal = 0x5555;

void SPI0_Handler() {
  NVIC_DisableIRQ(SPI0_IRQn);
  
  Serial.println("Interrupt triggered!");
  Serial.print("REG_SPI0_SR: ");
  Serial.println(REG_SPI0_SR);

  uint32_t iStatus = REG_SPI0_SR;

  if(iStatus & SPI_SR_RDRF)  
  {
    uint32_t d = REG_SPI0_RDR;
    iInVal = (byte) d;

    Serial.print(" Read: 0x");
    Serial.println(iInVal, HEX);
  }
  
  if(iStatus & SPI_SR_TDRE)
  {
    REG_SPI0_TDR = (uint32_t) iOutVal;
  }

  NVIC_EnableIRQ(SPI0_IRQn);
}

void slaveBegin(uint8_t _pin) {
	SPI.begin(_pin);
	REG_SPI0_CR = SPI_CR_SWRST;     // reset SPI
	REG_SPI0_CR = SPI_CR_SPIEN;     // enable SPI
	REG_SPI0_MR = SPI_MR_MODFDIS;     // slave and no modefault
	REG_SPI0_CSR = 0x82;    // DLYBCT=0, DLYBS=0, SCBR=0, 8 bit transfer

	REG_SPI0_IDR = ~SPI_IDR_RDRF;
	REG_SPI0_IER = SPI_IDR_RDRF;

	NVIC_EnableIRQ(SPI0_IRQn);
        REG_SPI0_TDR = (uint32_t) iOutVal; 
}

#define PRREG(x) Serial.print(#x" 0x"); Serial.println(x,HEX)

void prregs() {
	PRREG(REG_SPI0_MR);
	PRREG(REG_SPI0_CSR);
	PRREG(REG_SPI0_SR);
}


#define SS 10
void setup() {
	Serial.begin(57600);
	while(!Serial);
        Serial.println("Starting setup");
        
	slaveBegin(SS);
	prregs();  // debug
}

void loop() {
  delay(0);
}
