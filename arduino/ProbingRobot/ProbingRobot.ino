#include "CommanderHS.h"
#include <DynamixelSerial.h>
  
//Servo 1: Left, rear wheel
#define LEFT_REAR_WHEEL_ID 1
//Servo 2: Left, front wheel
#define LEFT_FRONT_WHEEL_ID 2
//Servo 3: Right, front wheel
#define RIGHT_FRONT_WHEEL_ID 3
//Servo 4: Right, rear wheel
#define RIGHT_REAR_WHEEL_ID 4
//Servo 5: Claw wrist
#define CLAW_WRIST_ID 5
//Servo 6: Right Claw
#define RIGHT_CLAW_ID 6
//Servo 7: Left claw
#define LEFT_CLAW_ID 7

#define WRIST_POS_MIN 312
#define WRIST_POS_MAX 612

#define LEFT_CLAW_POS_MIN 412
#define LEFT_CLAW_POS_MAX 712

#define RIGHT_CLAW_POS_MIN 312
#define RIGHT_CLAW_POS_MAX 612

#define ENABLE_DEBUG 1

int wristPos = 512;
int leftClawPos = 512;
int rightClawPos = 512;

CommanderData commanderData;
CommanderHS command = CommanderHS(&Serial3);
  
DynamixelSerial Dynamixel(&Serial2);

void processLeftWheels() {
  if(commanderData.walkV >= 0)
  {
    int leftWheelSpeed = map(commanderData.walkV, 0, 102, 0, 1023);
    Dynamixel.turn(LEFT_REAR_WHEEL_ID, 0, leftWheelSpeed);
    delay(10);
    Dynamixel.turn(LEFT_FRONT_WHEEL_ID, 0, leftWheelSpeed);
    delay(10);
  }
  else
  {
    int leftWheelSpeed = map(-commanderData.walkV, 0, 102, 0, 1023);
    Dynamixel.turn(LEFT_REAR_WHEEL_ID, 1, leftWheelSpeed);
    delay(10);
    Dynamixel.turn(LEFT_FRONT_WHEEL_ID, 1, leftWheelSpeed);
    delay(10);
  }
}

void processRightWheels() {
  if(commanderData.lookV >= 0)
  {
    int rightWheelSpeed = map(commanderData.lookV, 0, 102, 0, 1023);
    Dynamixel.turn(RIGHT_REAR_WHEEL_ID, 1, rightWheelSpeed);
    delay(10);
    Dynamixel.turn(RIGHT_FRONT_WHEEL_ID, 1, rightWheelSpeed);
    delay(10);
  }
  else
  {
    int rightWheelSpeed = map(-commanderData.lookV, 0, 102, 0, 1023);
    Dynamixel.turn(RIGHT_REAR_WHEEL_ID, 0, rightWheelSpeed);
    delay(10);
    Dynamixel.turn(RIGHT_FRONT_WHEEL_ID, 0, rightWheelSpeed);
    delay(10);
  }
}

void processWrist() {

    if(command.buttons&BUT_RT) {
      if(wristPos+3 <= WRIST_POS_MAX)
        wristPos += 3;

#ifdef ENABLE_DEBUG
      Serial.print("Wrist Pos: "); Serial.println(wristPos); 
#endif
      Dynamixel.moveSpeed(CLAW_WRIST_ID, wristPos, 700);
      delay(10);
    }
    else if(command.buttons&BUT_LT) {
      if(wristPos-3 >= WRIST_POS_MIN)
        wristPos -= 3;

#ifdef ENABLE_DEBUG
      Serial.print("Wrist Pos: "); Serial.println(wristPos); 
#endif
      Dynamixel.moveSpeed(CLAW_WRIST_ID, wristPos, 700);
      delay(10);
    }
}

void processClaw() {

    if(command.buttons&BUT_R1) {
      if(leftClawPos+3 <= LEFT_CLAW_POS_MAX)
        leftClawPos += 3;
      if(rightClawPos-3 >= RIGHT_CLAW_POS_MIN)
        rightClawPos -= 3;

#ifdef ENABLE_DEBUG
      Serial.print("Left Claw: "); Serial.print(leftClawPos); 
      Serial.print(",  Right Claw: "); Serial.println(rightClawPos); 
#endif
      Dynamixel.moveSpeed(LEFT_CLAW_ID, leftClawPos, 700);
      delay(10);
      Dynamixel.moveSpeed(RIGHT_CLAW_ID, rightClawPos, 700);
      delay(10);
    }
    else if(command.buttons&BUT_L6) {
      if(leftClawPos-3 >= LEFT_CLAW_POS_MIN)
        leftClawPos -= 3;
      if(rightClawPos+3 <= RIGHT_CLAW_POS_MAX)
        rightClawPos += 3;0.
        

#ifdef ENABLE_DEBUG
      Serial.print("Left Claw: "); Serial.print(leftClawPos); 
      Serial.print(",  Right Claw: "); Serial.println(rightClawPos); 
#endif
      Dynamixel.moveSpeed(LEFT_CLAW_ID, leftClawPos, 700);
      delay(10);
      Dynamixel.moveSpeed(RIGHT_CLAW_ID, rightClawPos, 700);
      delay(10);
    }
}

void checkCommander() {
  
  if(command.ReadMsgs() > 0) {
      //Serial.print("Commander has messages: ");
      //Serial.println(command.ReadMsgs());
    
    processWrist();
    processClaw();

    if(command.buttons != commanderData.buttons) {
      commanderData.buttons = command.buttons;
      commanderData.dataChanged = true;
    }

    if(command.walkV != commanderData.walkV) {
      commanderData.walkV = command.walkV;
      commanderData.dataChanged = true;
    }

    if(command.walkH != commanderData.walkH) {
      commanderData.walkH = command.walkH;
      commanderData.dataChanged = true;
    }

    if(command.walkV != commanderData.walkV) {
      commanderData.walkV = command.walkV;
      commanderData.dataChanged = true;
    }

    if(command.lookH != commanderData.lookH) {
      commanderData.lookH = command.lookH;
      commanderData.dataChanged = true;
    }

    if(command.lookV != commanderData.lookV) {
      commanderData.lookV = command.lookV;
      commanderData.dataChanged = true;
    }

    //If the commander data has changed then fill out the
    //custom sysex byte array and send it.    
    if(commanderData.dataChanged == true) {
#ifdef ENABLE_DEBUG
      Serial.print("Commander ");
      Serial.print(", WalkV: "); Serial.print(commanderData.walkV); 
      Serial.print(", WalkH: "); Serial.print(commanderData.walkH); 
      Serial.print(", LookV: "); Serial.print(commanderData.lookV); 
      Serial.print(", LookH: "); Serial.print(commanderData.lookH); 
      Serial.print(", Buttons: "); Serial.print(commanderData.buttons); 
      Serial.println ("");
#endif

      processLeftWheels();
      processRightWheels();
      
      commanderData.dataChanged = false;
    } 
  }  
}

void configureServos() {

  Dynamixel.setStatusReturnLevel(LEFT_REAR_WHEEL_ID, 1);
  delay(50);
  Dynamixel.setStatusReturnLevel(LEFT_FRONT_WHEEL_ID, 1);
  delay(50);
  Dynamixel.setStatusReturnLevel(RIGHT_REAR_WHEEL_ID, 1);
  delay(50);
  Dynamixel.setStatusReturnLevel(RIGHT_FRONT_WHEEL_ID, 1);
  delay(50);
  Dynamixel.setStatusReturnLevel(CLAW_WRIST_ID, 1);
  delay(50);
  Dynamixel.setStatusReturnLevel(LEFT_CLAW_ID, 1);
  delay(50);
  Dynamixel.setStatusReturnLevel(RIGHT_CLAW_ID, 1);
  delay(50);
  
  //Set the wheels to be in wheel mode
  Dynamixel.setEndless(LEFT_REAR_WHEEL_ID, true);
  delay(50);
  Dynamixel.setEndless(LEFT_FRONT_WHEEL_ID, true);
  delay(50);
  Dynamixel.setEndless(RIGHT_REAR_WHEEL_ID, true);
  delay(50);
  Dynamixel.setEndless(RIGHT_FRONT_WHEEL_ID, true);
  delay(50);
  
  Dynamixel.setCWLimit(CLAW_WRIST_ID, WRIST_POS_MIN);
  delay(50);
  Dynamixel.setCCWLimit(CLAW_WRIST_ID, WRIST_POS_MAX);
  delay(50);
  
  Dynamixel.setCWLimit(LEFT_CLAW_ID, LEFT_CLAW_POS_MIN);
  delay(50);
  Dynamixel.setCCWLimit(LEFT_CLAW_ID, LEFT_CLAW_POS_MAX);
  delay(50);
  
  Dynamixel.setCWLimit(RIGHT_CLAW_ID, RIGHT_CLAW_POS_MIN);
  delay(50);
  Dynamixel.setCCWLimit(RIGHT_CLAW_ID, RIGHT_CLAW_POS_MAX);
  delay(50);
  
  Dynamixel.turn(LEFT_REAR_WHEEL_ID, 1, 0);
  delay(50);
  Dynamixel.turn(LEFT_FRONT_WHEEL_ID, 1, 0);
  delay(50);
  Dynamixel.turn(RIGHT_REAR_WHEEL_ID, 1, 0);
  delay(50);
  Dynamixel.turn(RIGHT_FRONT_WHEEL_ID, 1, 0);
  delay(50);
  
  Dynamixel.moveSpeed(CLAW_WRIST_ID, 512, 0);
  delay(50);
  Dynamixel.moveSpeed(LEFT_CLAW_ID, 512, 0);
  delay(50);
  Dynamixel.moveSpeed(RIGHT_CLAW_ID, 512, 0);
  delay(50);
} 
  
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

#ifdef ENABLE_DEBUG
  Serial.begin(57600);
  while(!Serial);
  Serial.println("Starting setup");
#endif

  command.begin(38400);
  
  Dynamixel.begin (1000000, 22); 
  
  configureServos();  
}

void loop() {
  checkCommander();
  delay(10);                
}



