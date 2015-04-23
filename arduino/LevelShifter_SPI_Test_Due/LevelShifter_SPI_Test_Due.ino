/* SPI Master Demo
 *
 * Master monitors master button and sends button press status to slave. Master reads 
 * slave LED status every 5 seconds and updates master LED status.
 *
 * Communication Protocol:
 * Slave acknowledges SS (Slave Select) by sending -1 slave response code and receiving 
 * master command code. On subsequent SS slave echoes master command code or -1 slave 
 * response code indicating an unrecognized command has been received. Additional SS's 
 * may be needed to transfer additional data to/from Slave depending on the master command
 * code.
 *
 * Master command codes:
 * cmdBtn.
 * Inform slave of a master button press.
 *
 * SPI pin numbers:
 * SCK   13  // Serial Clock.
 * MISO  12  // Master In Slave Out.
 * MOSI  11  // Master Out Slave In.
 * SS    10  // Slave Select
 *
 * If you found this fun or interesting please make a small donation to my PayPal account 
 * at https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GTBD7T7BXPGQY. 
 * I have many more Arduino projects in mind and appreciate your support.
 *
 * No commercial use without prior consent.
 *
 * Based on code by: Nick Cammon 2/2011.
 */
// Include SPI (Serial Peripheral Interface) library.
#include <SPI.h>

byte send_data = 0;

// The setup() function runs after reset.
void setup() {
  // Initialize serial for DEBUG.
  Serial.begin(9600);
  while(!Serial);
  // Initialize SPI.
  // SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin();
  // Slow down the master a bit
  //SPI.setClockDivider(SPI_CLOCK_DIV8);
  // Master initialized.
  Serial.println("Master Initialized");
}
// The loop() function runs continuously after setup().
void loop() {
  // Master button pressed?
  // Yes
  Serial.println("Master Button Pressed.");
  // Select and wait for slave.
  digitalWrite(10, LOW);
  //Serial.println("***Slave Selected.");
  delay(20);
  // Send cmdBtn
  SPI.transfer(send_data);
  Serial.println("data Sent.");
  // Wait for slave.
  delay(20);
  // Get slave response.
  byte rx = SPI.transfer(255);
  // Acknowledged?
  if (rx == send_data) {
    // Yes.
    Serial.println("Slave acknowledged cmdBtn.");
  }
  // Command not recognized?
  else if (rx == 255) {
    // Yes.
    Serial.println("Slave returned -1 slave return code - master command code not recognized.");
  }
  // Unrecognized response.
  else {
    Serial.println("Slave returned:" + String(rx) + ".");
  }
  // Disable slave.
  digitalWrite(10, HIGH);     

  delay(2000);
}

