#include <DynamixelSerial.h>

#define SERVO_COUNT 5
#define AXIS_COUNT 4

// These constants won't change.  They're used to give names
// to the pins used:
const int x1Pin = A0;  
const int y1Pin = A1; 
const int x2Pin = A2; 
const int y2Pin = A3;

//Index of the AxisPos array that controls gripper open/close
const int gripperCloseIdx = 1;
const int gripperOpenIdx = 3;

const int gripperIdx = 4;

int servoPos[SERVO_COUNT];
int servoAdd[SERVO_COUNT];
int axisID[AXIS_COUNT];
int axisPos[AXIS_COUNT];

DynamixelSerial Dynamixel(&Serial2);

void setServoDelayReturnTime(int time) {
  for(int i=0; i<SERVO_COUNT; i++)
    Dynamixel.setReturnDelayTime(i, time);
}

void setStatusReturnLevel(unsigned char level) {
  for(int i=0; i<SERVO_COUNT; i++)
    Dynamixel.setStatusReturnLevel(i, level);
}

void setServoPositions(int pos) {
  for(int i=0; i<SERVO_COUNT; i++)
  {
    //servoPos[i] = pos;
    //servoAdd[i] = 0;
    Serial.println("Servo: " + String(i+1) + ", " + pos);
    //Dynamixel.move (i+1, pos);
    Dynamixel.moveSpeed (i+1, pos, 30);
    delay(10);
  }  
}

void setup () {
 
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

  delay(1000);
  //setServoDelayReturnTime(5);
  setStatusReturnLevel(1);
      
  axisID[0] = A1;
  axisPos[0] = 0;
  axisID[1] = A0;
  axisPos[1] = 0;
  axisID[2] = A3;
  axisPos[2] = 0;
  axisID[3] = A2;
  axisPos[3] = 0;
  
  for(int i=0; i<SERVO_COUNT; i++)
  {
    Serial.print("Setup servo: ");
    Serial.println(i+1);
    
    if(i != gripperIdx)
    {
      servoPos[i] = 512;
      servoAdd[i] = 0;
    }
    else
    {
      servoPos[i] = 0;
      servoAdd[i] = 0;
    }
    
    Dynamixel.moveSpeed (i+1,servoPos[0], 150);
  }
  
  Serial.println("Finished Setup");
} 

void processGripper(String &servoPosReport, String &servoAddReport) {
  
  if(axisPos[gripperCloseIdx] == 1023)
    servoAdd[gripperIdx] = -5;
  else if(axisPos[gripperOpenIdx] == 1023)
    servoAdd[gripperIdx] = 5;
  else
    servoAdd[gripperIdx] = 0;
  
  if( (servoPos[gripperIdx] + servoAdd[gripperIdx]) >= 0 
    && (servoPos[gripperIdx] + servoAdd[gripperIdx] <= 512))
    servoPos[gripperIdx] += servoAdd[gripperIdx];
    
  servoPosReport += String(servoPos[gripperIdx]);
  servoAddReport += String(servoAdd[gripperIdx]);
    
  if(servoAdd[gripperIdx] != 0)  
    Dynamixel.moveSpeed (gripperIdx+1, servoPos[gripperIdx], 150);      
}

void processJoysticks() {
  String servoPosReport = "", servoAddReport = "", axisReport = ""; 
  
  // read the analog in value:
  for(int i=0; i<AXIS_COUNT; i++)
  {
    axisPos[i] = analogRead(axisID[i]);
    
    //If it is greater than 1020 then the button is being pressed.
    if(axisPos[i] < 1020)
      servoAdd[i] = map(axisPos[i], 255, 755, -10, 10);
    else
      servoAdd[i] = 0;

    if( (servoPos[i] + servoAdd[i]) >= 0 && (servoPos[i] + servoAdd[i] < 1024))
      servoPos[i] += servoAdd[i];

    servoPosReport += String(servoPos[i]);
    servoAddReport += String(servoAdd[i]);
    axisReport += String(axisPos[i]);
    
    if(i < AXIS_COUNT)
    {
       servoPosReport += ", ";
       servoAddReport += ", ";
       axisReport += ", ";
    }

    if(servoAdd[i] != 0)  
    {
      Dynamixel.moveSpeed (i+1, servoPos[i], 150);
      delay(10); 
    }
  }
  
  processGripper(servoPosReport, servoAddReport);
    
  Serial.println(servoPosReport + "     " + servoAddReport + "     " + axisReport);
}

void loop () { 
  processJoysticks();

  delay (20); 
} 

