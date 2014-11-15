// SPI slave for DUE, hack
//  master will lower CS and manage CLK
// MISO to MISO  MOSI to MOSI   CLK to CLK   CS to CS, common ground
//   http://forum.arduino.cc/index.php/topic,157203.0.html
// core hardware/arduino/sam/system/libsam/source/spi.c
// TODO:  C class, interrupt, FIFO/16-bit, DMA?

#include <SPI.h>

// assumes MSB
static BitOrder bitOrder = MSBFIRST;

static byte aryBuffer[7] = {0,0,0,0,0,0,0};
static byte iBufferIdx = 0;

void SPI0_Handler() {
  NVIC_DisableIRQ(SPI0_IRQn);
  
  uint32_t d = REG_SPI0_RDR;
  aryBuffer[iBufferIdx] = (byte) d;
  
  iBufferIdx++;
  //Serial.print("BufferIdx: ");
  //Serial.println(iBufferIdx);

  if(iBufferIdx > 6)
  {
    iBufferIdx=0;
    
    Serial.print("Recieved: ");
    for(int i=0; i<7; i++)
    {
      Serial.print(aryBuffer[i]);
      Serial.print(", ");
    }
    Serial.println(" ");
  }
  
  NVIC_EnableIRQ(SPI0_IRQn);
}

void slaveBegin(uint8_t _pin) {
	SPI.begin(_pin);
	REG_SPI0_CR = SPI_CR_SWRST;     // reset SPI
	REG_SPI0_CR = SPI_CR_SPIEN;     // enable SPI
	REG_SPI0_MR = SPI_MR_MODFDIS;     // slave and no modefault
	REG_SPI0_CSR = SPI_MODE0;    // DLYBCT=0, DLYBS=0, SCBR=0, 8 bit transfer

	REG_SPI0_IDR = ~SPI_IDR_RDRF;
	REG_SPI0_IER = SPI_IER_RDRF;

	NVIC_EnableIRQ(SPI0_IRQn);
}

/*
byte transfer(uint8_t _pin, uint8_t _data) {
    // Reverse bit order
    if (bitOrder == LSBFIRST)
        _data = __REV(__RBIT(_data));
    uint32_t d = _data;

    Serial.println("Waiting till clear to send");
    
    while ((REG_SPI0_SR & SPI_SR_TDRE) == 0) ;

    Serial.println("Setting transmit data");
    REG_SPI0_TDR = d;

    Serial.println("Waiting till recieve full");
    Serial.print("1. REG_SPI0_SR: ");
    Serial.println(REG_SPI0_SR);
    
    while ((REG_SPI0_SR & SPI_SR_RDRF) == 0) 
    //{
    //  Serial.print("REG_SPI0_SR: ");
    //  Serial.println(REG_SPI0_SR);
    //};
    
    d = REG_SPI0_RDR;
    Serial.print("2. REG_SPI0_SR: ");
    Serial.println(REG_SPI0_SR);

    Serial.print("Recieved: ");
    Serial.println(d);
    
    // Reverse bit order
    if (bitOrder == LSBFIRST)
        d = __REV(__RBIT(d));
    return d & 0xFF;
}
*/

#define PRREG(x) Serial.print(#x" 0x"); Serial.println(x,HEX)

void prregs() {
	Serial.begin(57600);
	while(!Serial);
	PRREG(REG_SPI0_MR);
	PRREG(REG_SPI0_CSR);
	PRREG(REG_SPI0_SR);
}


#define SS 10
void setup() {
	slaveBegin(SS);
	prregs();  // debug
}

void loop() {
  delay(0);
	//byte in;
	//static byte out=0x83;

        //Serial.println("starting transfer");
	//in = transfer(SS, out);
	//out = in;
        //Serial.print("Out: ");
        //Serial.println(out);
}
