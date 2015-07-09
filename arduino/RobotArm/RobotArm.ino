#include <DynamixelSerial.h>

//#define SERVO_ID1 1
//#define SERVO_ID2 2

#define SERVO_COUNT 4
#define AXIS_COUNT 4
// These constants won't change.  They're used to give names
// to the pins used:
const int x1Pin = A0;  
const int y1Pin = A1; 
const int x2Pin = A2; 
const int y2Pin = A3;

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
  //delay (5000); 
  
  pinMode(22, OUTPUT);
  Dynamixel.begin (1000000, 22); 

  //Dynamixel.setID(30, SERVO_ID);
  
  //delay(1000);
  //Dynamixel.reset(1);

  delay(1000);
  setServoDelayReturnTime(5);
  setStatusReturnLevel(1);
  
  //delay (100); 
  //int ret_delay = Dynamixel.readReturnDelayTime(1);
  //Serial.print("Ret_Delay: ");
  //Serial.println(ret_delay);
  
  //setServoPositions(512);
    
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
    servoPos[i] = 512;
    servoAdd[i] = 0;
    Dynamixel.moveSpeed (i+1,servoPos[0], 150);
    
  }
  
  //Serial.println("Set speed 1");
  //delay (100); 
  //servoPos[0] = 512;
  //servoAdd[0] = 0;
  //Dynamixel.moveSpeed (SERVO_ID1,servoPos[0], 0);
  //delay (100); 
  //Serial.println("Set speed 2");
  //Dynamixel.moveSpeed (SERVO_ID2,512, 0);
  //delay(1000);
  
  //ref_pos = Dynamixel.readPosition(1);
  //delay (100); 
  Serial.println("Finished Setup");
} 

void ReadPosWithError(int servo, int ref_pos)
{  
  bool done = false;
  int errors = 0;
  int pos = 0, i = 0;
  double avg_time = 0;
  int loops = 10;
  int pos_vals[100];
  int pos_errors = 0;
  unsigned long end_time = 0, diff = 0, start_time = micros();
  
  for(i=0; i<loops; i++)
  {
    pos = Dynamixel.readPosition(servo);       // Request and Print the Position 
    //delayMicroseconds(200);
    delay(50);
    
    if(pos < 0)
      errors++;   
   
    if(pos != ref_pos)
    {
       pos_vals[pos_errors] = pos;
       pos_errors++;   
    }
  }
  
  end_time = micros();
  diff = end_time - start_time;  
  avg_time = (double) diff/ (double) loops;
  
  Serial.print("diff: ");
  Serial.println(diff);
  Serial.print("avg: ");
  Serial.println(avg_time);
  Serial.print("errors: ");
  Serial.println(errors);

  Serial.print("pos_errors: ");
  Serial.println(pos_errors);

  for(i=0; i<pos_errors; i++)
  {
    Serial.println(pos_vals[i]);
  }
}

/*
void ReadPos(int servo, int target)
{  
  bool done = false;
  int iError = 
  //while(!done)
  for(int i=0; i<100; i++)
  {
    int pos = Dynamixel.readPosition(1);       // Request and Print the Position 
    //Serial.print("Position: ");
    //Serial.print(pos);
    //Serial.println(" of 1023 resolution");
    delay(20);
    
    if(pos != Position)
      
    
    //if(abs(pos-target) <= 4)
    //  done = true;      
  }
}
*/

void processJoysticks() {
  String servoPosReport = "", servoAddReport = ""; 
  
  // read the analog in value:
  for(int i=0; i<AXIS_COUNT; i++)
  {
    axisPos[i] = analogRead(axisID[i]);
    servoAdd[i] = map(axisPos[i], 255, 755, -10, 10);
    
    if( (servoPos[i] + servoAdd[i]) >= 0 && (servoPos[i] + servoAdd[i] < 1024))
      servoPos[i] += servoAdd[i];

    servoPosReport += String(servoPos[i]);
    servoAddReport += String(servoAdd[i]);
    
    if(i < AXIS_COUNT)
    {
       servoPosReport += ", ";
       servoAddReport += ", ";
    }
    

    if(servoAdd[i] != 0)  
    {
      //Serial.println("Servo: " + String(i+1) + ", " + servoPos[i]);
      Dynamixel.moveSpeed (i+1, servoPos[i], 150);
      delay(10); 
    }
  }
    
  Serial.println(servoPosReport + "     " + servoAddReport);
}

void loop () { 
  processJoysticks();
  
  //int x1 = 0; //analogRead(A0);

  //int servoAddX1 = map(x1, 255, 755, -10, 10);
  //if( (servoPos[0] + servoAddX1) >= 0 && (servoPos[0] + servoAddX1 < 1024))
  //  servoPos[0] += servoAddX1;


 // digitalWrite(22, HIGH);   // turn the LED on (HIGH is the voltage level)
  //Serial.println("Move");
  //Serial.print("Move");
  //Dynamixel.moveSpeed(1, servoPos[0], 100);  //.move(1, 450);
  

  //Dynamixel.move (SERVO_ID1, 450);  //.move(1, 450);, 30
  //delay(100);
  //Dynamixel.move(SERVO_ID2, 650);  //.move(1, 450);

  //delay(2000);
  //int Position = Dynamixel.readPosition(1);       // Request and Print the Position 
  //Serial.print("Position: ");
  //Serial.println(Position);

  //ReadPosWithError(SERVO_ID1, 450);
  
  //setServoPositions(450);
  //delay (2000); 
  //digitalWrite(22, LOW);   // turn the LED on (HIGH is the voltage level)
  //Serial.println("Move 650");
  //setServoPositions(650);

  //Dynamixel.move(SERVO_ID1, 650); 
  //delay(100);
  //Dynamixel.move(SERVO_ID2, 450); 
   //Dynamixel.move(1, 650);
   
  //delay(2000);
  //Position = Dynamixel.readPosition(1);       // Request and Print the Position 
  //Serial.print("Position: ");
  //Serial.println(Position);

  delay (20); 
} 

