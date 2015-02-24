// SPI slave for DUE, hack
//  master will lower CS and manage CLK
// MISO to MISO  MOSI to MOSI   CLK to CLK   CS to CS, common ground
//   http://forum.arduino.cc/index.php/topic,157203.0.html
// core hardware/arduino/sam/system/libsam/source/spi.c
// TODO:  C class, interrupt, FIFO/16-bit, DMA?

#include <SPI.h>

// assumes MSB
static BitOrder bitOrder = MSBFIRST;

static int aryInBuffer[10] = {0,0,0,0,0,0,0,0,0,0};
static int aryOutBuffer[10] = {1,2,3,4,5,6,7,9,9,10};
static byte iInBufferIdx = 0;
static byte iOldInBufferIdx = 0;
static byte iOutBufferIdx = 0;

void SPI0_Handler() {
  NVIC_DisableIRQ(SPI0_IRQn);
  
  //Serial.println("Interrupt triggered!");
  //Serial.print("REG_SPI0_SR: ");
  //Serial.println(REG_SPI0_SR);

  uint32_t iStatus = REG_SPI0_SR;

  if(iStatus & SPI_SR_RDRF)  
  {
    aryInBuffer[iInBufferIdx] = (int) REG_SPI0_RDR;

    //Serial.print("Rx BufferIdx: ");
    //Serial.print(iInBufferIdx);
    //Serial.print(" Buffer: ");
    //Serial.println(d, HEX);

    if(iInBufferIdx < 7)
      iInBufferIdx++;
  }
  
  if(iStatus & SPI_SR_TDRE)
  {
    if(iOutBufferIdx<6)
      iOutBufferIdx++;      
    
    REG_SPI0_TDR = (uint32_t) aryOutBuffer[iOutBufferIdx];
    
    //Serial.print("TX BufferIdx: ");
    //Serial.print(iOutBufferIdx);
  }
  
  NVIC_EnableIRQ(SPI0_IRQn);
}

void slaveBegin(uint8_t _pin) {
	SPI.begin(_pin);
	REG_SPI0_CR = SPI_CR_SWRST;     // reset SPI
	REG_SPI0_CR = SPI_CR_SPIEN;     // enable SPI
	REG_SPI0_MR = SPI_MR_MODFDIS;     // slave and no modefault
	REG_SPI0_CSR = 0x80;    // DLYBCT=0, DLYBS=0, SCBR=0, 8 bit transfer
	//REG_SPI0_CSR = (SPI_MODE0 | 0x80);    // DLYBCT=0, DLYBS=0, SCBR=0, 16 bit transfer

	//REG_SPI0_IDR = ~SPI_IDR_TDRE;
	//REG_SPI0_IER = SPI_IDR_TDRE;
	REG_SPI0_IDR = ~SPI_IDR_RDRF;
	REG_SPI0_IER = SPI_IDR_RDRF;

	NVIC_EnableIRQ(SPI0_IRQn);
        REG_SPI0_TDR = (uint32_t) 0x5555; //aryOutBuffer[0];
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
    Serial.println(REG_SPI0_SR, HEX);
    
    while ((REG_SPI0_SR & SPI_SR_RDRF) == 0) 
    {
      //Serial.print("REG_SPI0_SR: ");
      //Serial.println(REG_SPI0_SR, HEX);
    };
    
    d = REG_SPI0_RDR;
    Serial.print("2. REG_SPI0_SR: ");
    Serial.println(REG_SPI0_SR, HEX);

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

        //Serial.print("REG_SPI0_CSR: ");
        //Serial.println(SPI_MODE0);
}

void loop() {
  delay(0);
  
  if(iOldInBufferIdx != iInBufferIdx)
  {
    Serial.print("BufferIdx: ");
    Serial.println(iInBufferIdx, HEX);
    iOldInBufferIdx = iInBufferIdx;
  }
  
  if(iInBufferIdx == 4)
  {
    Serial.println("Receivied data packet!");
    for(int iIdx=0; iIdx<10; iIdx++)
    {
      Serial.print(aryInBuffer[iIdx], HEX);
      Serial.print(", ");
    }
    Serial.println("");
    
    iInBufferIdx = 0;
    iOutBufferIdx = 0;
  }
  
/*	byte in;
	static byte out=0x83;

        Serial.println("starting transfer");
	in = transfer(SS, out);
	out = in;
        Serial.print("Out: ");
        Serial.println(out);*/
}
