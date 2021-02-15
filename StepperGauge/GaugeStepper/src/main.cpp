#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>
#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

#define TESTMODE

#define STEPS 2048

unsigned char servoRightExists = 0;
unsigned char servoLeftExists = 0;
unsigned char stepperExists = 0;

unsigned int servoRightAddress = 0;
unsigned int servoLeftAddress = 0;
unsigned int stepperAddress = 0;

unsigned int servoRightMin = 600;
unsigned int servoRightMax = 2200;
unsigned int servoRightMinValue = 0;
unsigned int servoRightMaxValue = 100;

unsigned int servoLeftMin = 600;
unsigned int servoLeftMax = 2200;
unsigned int servoLeftMinValue = 0;
unsigned int servoLeftMaxValue = 100;

unsigned int stepperOffset = 0;
unsigned int stepperSpeed = 15;

unsigned char stepperMapFunction = 0;
float stepperScaler = 1.0f;
float stepperP1 = 0.0f;
float stepperP2 = 1.0f;
float stepperP3 = 1.0f;
float stepperP4 = 1.0f;
 
#define SERVO_RIGHT_EXISTS 4
#define SERVO_RIGHT_ADDR SERVO_RIGHT_EXISTS + sizeof(byte)
#define SERVO_RIGHT_MIN SERVO_RIGHT_ADDR + sizeof(unsigned int)
#define SERVO_RIGHT_MAX SERVO_RIGHT_MIN + sizeof(unsigned int)
#define SERVO_RIGHT_MIN_VALUE SERVO_RIGHT_MAX + sizeof(unsigned int)
#define SERVO_RIGHT_MAX_VALUE SERVO_RIGHT_MIN_VALUE + sizeof(unsigned int)

#define SERVO_LEFT_EXISTS SERVO_RIGHT_MAX_VALUE + sizeof(unsigned int)
#define SERVO_LEFT_ADDR SERVO_LEFT_EXISTS + sizeof(byte)
#define SERVO_LEFT_MIN SERVO_LEFT_ADDR + sizeof(int)
#define SERVO_LEFT_MAX SERVO_LEFT_MIN + sizeof(unsigned int)
#define SERVO_LEFT_MIN_VALUE SERVO_LEFT_MAX + sizeof(unsigned int)
#define SERVO_LEFT_MAX_VALUE SERVO_LEFT_MIN_VALUE + sizeof(unsigned int)

#define STEPPER_EXISTS SERVO_LEFT_MAX_VALUE  + sizeof(unsigned int)
#define STEPPER_ADDR STEPPER_EXISTS  + sizeof(unsigned int)
#define STEPPER_OFFSET STEPPER_ADDR  + sizeof(unsigned int)
#define STEPPER_SPEED STEPPER_OFFSET + sizeof(unsigned int)
#define STEPPER_MAP_FUNCTION STEPPER_SPEED + sizeof(unsigned int)
#define STEPPER_P1 STEPPER_MAP_FUNCTION + sizeof(byte)
#define STEPPER_P2 STEPPER_P1 + sizeof(float)
#define STEPPER_P3 STEPPER_P2 + sizeof(float)
#define STEPPER_P4 STEPPER_P3 + sizeof(float)

#define SERVO_LEFT_ADDR 10
#define SERVO_RIGHT_MIN 12
#define SERVO_RIGHT_MAX 4

unsigned int oldValue = 0;

DcsBios::Stepper mainStepper(0x509A, STEPS, 6, 2, 4, 3, 5);
DcsBios::Switch2Pos unsafeLndGearLtTest("UNSAFE_LND_GEAR_LT_TEST", 6);

void cmdHandler(unsigned char cmd, unsigned int address, int value)
{ 
  switch (cmd)
  {
  case 0x00:
    mainStepper.home();
    oldValue = 0;
    break;
  }
}

#define LEFT_RPM_FUEL
#ifdef LEFT_RPM_FUEL
#define STEPPER_OFFSET 375
#define STEPPER_CALIBRATION -0.435
#define STEPPER_SPEED 15
#define HAS_SERVOS
#define STEPPER_ADDRESS 0x509c

#define LEFT_SERVO_MIN 600
#define LEFT_SERVO_MAX 2000

#define RIGHT_SERVO_MIN 600
#define RIGHT_SERVO_MAX 1860

#define LEFT_ADDRESS 0x50b4
#define RIGHT_ADDRESS 0x50b6
#endif

//#define RIGHT_MFD_PRESSURE_PSI 
#define HAS_SERVOS      

#ifdef RIGHT_MFD_PRESSURE_PSI 
#define STEPPER_OFFSET 0
#define STEPPER_CALIBRATION 1
#define STEPPER_SPEED 15
#define STEPPER_ADDRESS 0x509c

#define LEFT_SERVO_MIN 600
#define LEFT_SERVO_MAX 2000

#define RIGHT_SERVO_MIN 600
#define RIGHT_SERVO_MAX 1860
#endif


//#define MPH
#ifdef MPH
#define STEPPER_OFFSET 0
#define STEPPER_SPEED 15
#define STEPPER_ADDRESS 0x5098
#define P1 -0.0042
#define P2 5.9099
#define P3 -230.85
#endif

//#define VARIOMETER
#ifdef VARIOMETER
#define STEPPER_OFFSET 0
#define STEPPER_SPEED 15
#define STEPPER_ADDRESS 0x503c
#define P1 -0.0042
#define P2 5.9099
#define P3 -230.85
#define CENTER 0x7fff
#endif

#ifdef HAS_SERVOS
Servo leftGauge;
Servo rightGauge;
#endif

#define INITIALIZED_VALUE  0x1F1F1F1F

void printAddress(unsigned int addr) {
  char str[10];
  sprintf(str, "0x%x", addr);
  Serial.println(str);
}

void loadSettings() {
  unsigned long initValue;
  EEPROM.get(0, initValue);
  if(initValue != INITIALIZED_VALUE) {
#ifdef TESTMODE  
  Serial.println("Was not initialized - setting parameters");
#endif  

      EEPROM.put(SERVO_LEFT_MIN, servoLeftMin);
      EEPROM.put(SERVO_LEFT_MAX, servoLeftMax );
      EEPROM.put(SERVO_RIGHT_MIN, servoRightMin);
      EEPROM.put(SERVO_RIGHT_MAX, servoRightMax );

      EEPROM.put(SERVO_LEFT_MIN_VALUE, servoLeftMinValue);
      EEPROM.put(SERVO_LEFT_MAX_VALUE, servoLeftMaxValue );
      EEPROM.put(SERVO_RIGHT_MIN_VALUE, servoRightMinValue);
      EEPROM.put(SERVO_RIGHT_MAX_VALUE, servoRightMaxValue ); 

      EEPROM.put(SERVO_LEFT_ADDR, servoLeftAddress);
      EEPROM.put(SERVO_RIGHT_ADDR, servoRightAddress);
      EEPROM.put(STEPPER_ADDR, stepperAddress);

      EEPROM.put(STEPPER_EXISTS, (unsigned char)stepperExists); 
      EEPROM.put(SERVO_LEFT_EXISTS, (unsigned char)servoLeftExists);
      EEPROM.put(SERVO_RIGHT_EXISTS, (unsigned char)servoRightExists);

      EEPROM.put(STEPPER_OFFSET, stepperOffset);
      EEPROM.put(STEPPER_SPEED, stepperSpeed); 

      EEPROM.put(STEPPER_MAP_FUNCTION, (unsigned char)stepperMapFunction); 
      EEPROM.put(STEPPER_P1, stepperP1); 
      EEPROM.put(STEPPER_P2, stepperP2); 
      EEPROM.put(STEPPER_P3, stepperP3); 
      EEPROM.put(STEPPER_P4, stepperP4); 

      EEPROM.put(0, (unsigned long)INITIALIZED_VALUE);
  }
  else {
#ifdef TESTMODE  
  Serial.println("Was initialized - Loading Parameters");
#endif  

      EEPROM.get(SERVO_LEFT_MIN, servoLeftMin);
      EEPROM.get(SERVO_LEFT_MAX, servoLeftMax );
      EEPROM.get(SERVO_RIGHT_MIN, servoRightMin);
      EEPROM.get(SERVO_RIGHT_MAX, servoRightMax );

      EEPROM.get(SERVO_LEFT_MIN_VALUE, servoLeftMinValue);
      EEPROM.get(SERVO_LEFT_MAX_VALUE, servoLeftMaxValue );
      EEPROM.get(SERVO_RIGHT_MIN_VALUE, servoRightMinValue);
      EEPROM.get(SERVO_RIGHT_MAX_VALUE, servoRightMaxValue ); 

      EEPROM.get(SERVO_LEFT_ADDR, servoLeftAddress);
      EEPROM.get(SERVO_RIGHT_ADDR, servoRightAddress);
      EEPROM.get(STEPPER_ADDR, stepperAddress);

      EEPROM.get(STEPPER_EXISTS, stepperExists); 
      EEPROM.get(SERVO_LEFT_EXISTS, servoLeftExists);
      EEPROM.get(SERVO_RIGHT_EXISTS, servoRightExists);

      EEPROM.get(STEPPER_OFFSET, stepperOffset);
      EEPROM.get(STEPPER_SPEED, stepperSpeed); 

      EEPROM.get(STEPPER_MAP_FUNCTION, stepperMapFunction); 
      EEPROM.get(STEPPER_P1, stepperP1); 
      EEPROM.get(STEPPER_P2, stepperP2); 
      EEPROM.get(STEPPER_P3, stepperP3); 
      EEPROM.get(STEPPER_P4, stepperP4); 

  }

#ifdef TESTMODE  
  Serial.println();
  Serial.print("Left Servo Exists  : ");
  Serial.println((servoLeftExists) ? "true" : "false");
  Serial.print("Left Servo Address : ");
  printAddress(servoLeftAddress);
  Serial.println("Left Servo Min     : " + String(servoLeftMin));
  Serial.println("Left Servo Max     : " + String(servoLeftMax));
  Serial.println("Left Servo Min Val : " + String(servoLeftMinValue));
  Serial.println("Left Servo Max Val : " + String(servoLeftMaxValue));
  Serial.println();
  Serial.print("Right Servo Exists : ");
  Serial.println(servoRightExists ? "true" : "false");
  Serial.print("Right Servo Address: ");
  printAddress(servoRightAddress);
  Serial.println("Right Servo Min    : " + String(servoRightMin));
  Serial.println("Right Servo Max    : " + String(servoRightMax));
  Serial.println("Right Servo Min Val: " + String(servoRightMinValue));
  Serial.println("Right Servo Max Val: " + String(servoRightMaxValue));
  Serial.println();
  Serial.print("Stepper Exists     : ");
  Serial.println(stepperExists ? "true" : "false");  
  Serial.print("Stepper Address    : ");
  printAddress(stepperAddress);
  Serial.println("Stepper Zero Offset: " + String(stepperOffset));
  Serial.print("Stepper Mapping Fun: ");
  switch(stepperMapFunction) {
    case 0: Serial.println("none"); break;
    case 1: Serial.println("linear"); break;
    case 2: Serial.println("2nd order"); break;
    case 3: Serial.println("3rd order"); break;
    default: Serial.println("Uknown"); break;
  }
  Serial.println("Stepper P1         : " + String(stepperP1));
  Serial.println("Stepper P2         : " + String(stepperP2));
  Serial.println("Stepper P3         : " + String(stepperP3));
  Serial.println("Stepper P4         : " + String(stepperP4));
#endif  

}

void setup()
{
  DcsBios::RegisterCommandCallback(cmdHandler);

  if(servoLeftExists) {
    leftGauge.attach(11);
    leftGauge.writeMicroseconds(servoLeftMin);
  }
  
  if(servoRightExists) {
    rightGauge.attach(10);
    rightGauge.writeMicroseconds(servoRightMin);
  }

  mainStepper.setSpeed(15);

  mainStepper.setZeroOffset(STEPPER_OFFSET);
  mainStepper.setSpeed(STEPPER_SPEED);

#ifndef TESTMODE
  DcsBios::setup();
#endif

#ifdef TESTMODE
  Serial.begin(115200);
  Serial.println();
  Serial.println("Welcome - Test Mode");
  Serial.println("============================================");
#endif

  loadSettings();

  mainStepper.home();
}

#ifdef MPH
int scale(unsigned int newValue) {
  float fNewValue = newValue;
  return (int)(((fNewValue * fNewValue)  * P1) + (P2 * fNewValue) + P3);
}
#endif

#ifdef VARIOMETER
int scale(unsigned int newValue) {
   int newValue = CENTER;
  return newValue;
}
#endif

void onStepperValueChanged(unsigned int newValue)
{
#ifdef STEPPER_CALIBRATION
  int delta = (newValue - oldValue);
  delta *= STEPPER_CALIBRATION;
  mainStepper.step(delta);
  oldValue = newValue;
#else  
#endif  

#ifdef P3
  int scaledNewValue = scale(newValue);
  int delta = oldValue - scaledNewValue;
  mainStepper.step(delta);
  oldValue = scaledNewValue;
#endif 

#ifdef TESTMODE
  Serial.println("Setting steps=" + String(oldValue) + ", New Value=" + String(newValue) + " Delta=" + String (delta));
#endif

  /* your code here */
}
DcsBios::IntegerBuffer stepperValueBuffer(STEPPER_ADDRESS, 0xffff, 0, onStepperValueChanged);

#ifdef HAS_SERVOS
void leftServoChanged(unsigned int newValue)
{
  leftGauge.writeMicroseconds(newValue);
}
DcsBios::IntegerBuffer leftServoeValueBuffer(0x50b4, 0xffff, 0, leftServoChanged);

void rightServoChanged(unsigned int newValue)
{
  rightGauge.writeMicroseconds(newValue);
}
DcsBios::IntegerBuffer rightServoValueBuffer(0x50b4, 0xffff, 0, rightServoChanged);
#endif

boolean toggled;

void (* resetFunc) (void) = 0;

void loop()
{
#ifndef TESTMODE
  DcsBios::loop();
#endif

#ifdef TESTMODE
  String input = Serial.readStringUntil('\n');
  if (input.length() > 0)
  {
    if (input == "zero")
    {
      Serial.println("zero");
      oldValue = 0;
      mainStepper.home();
    }
    else
    {
      String cmd = input.substring(0, input.indexOf(" "));
      String param = input.substring(input.indexOf(" ") + 1);
      Serial.println("Handling: " + cmd + " - " + param);
      int paramValue = atoi(param.c_str());
      int hexAddr = strtol(param.c_str(), NULL, 16);
      float fParamValue = (float)atof(param.c_str());
      
      if (cmd == "setstep") onStepperValueChanged(paramValue);
      else if (cmd == "left") leftServoChanged(paramValue);
      else if (cmd == "right") rightServoChanged(paramValue);

      else if(cmd == "set-min-left") { EEPROM.put(SERVO_LEFT_MAX, paramValue); servoLeftMin = paramValue; }
      else if(cmd == "set-max-left") { EEPROM.put(SERVO_LEFT_MAX, paramValue); servoLeftMax = paramValue; }
      else if(cmd == "set-min-right") { EEPROM.put(SERVO_RIGHT_MAX, paramValue); servoRightMin = paramValue; }
      else if(cmd == "set-max-right") { EEPROM.put(SERVO_RIGHT_MAX, paramValue); servoRightMax = paramValue; }

      else if(cmd == "set-minvalue-left") { EEPROM.put(SERVO_LEFT_MAX_VALUE, paramValue); servoLeftMinValue = paramValue; }
      else if(cmd == "set-maxvalue-left") { EEPROM.put(SERVO_LEFT_MAX_VALUE, paramValue); servoLeftMaxValue = paramValue; }
      else if(cmd == "set-minvalue-right") { EEPROM.put(SERVO_RIGHT_MAX_VALUE, paramValue); servoRightMinValue = paramValue; }
      else if(cmd == "set-maxvalue-right") { EEPROM.put(SERVO_RIGHT_MAX_VALUE, paramValue); servoRightMaxValue = paramValue; }

      else if(cmd == "set-addr-left") { EEPROM.put(SERVO_LEFT_ADDR, hexAddr); servoLeftAddress = hexAddr; }
      else if(cmd == "set-addr-right") { EEPROM.put(SERVO_RIGHT_ADDR, hexAddr); servoRightAddress = hexAddr; }
      else if(cmd == "set-addr-stepper") { EEPROM.put(STEPPER_ADDR, hexAddr); stepperAddress = hexAddr; }

      else if(cmd == "set-exists-stepper") { EEPROM.put(STEPPER_EXISTS, (byte)paramValue); stepperExists = paramValue; }
      else if(cmd == "set-exists-left") { EEPROM.put(SERVO_LEFT_EXISTS, (byte)paramValue); servoLeftExists = paramValue; }
      else if(cmd == "set-exists-right") { EEPROM.put(SERVO_RIGHT_EXISTS, (byte)paramValue); servoRightExists = paramValue; }

      else if(cmd == "set-offset-stepper") { EEPROM.put(STEPPER_OFFSET, (byte)paramValue); stepperOffset = paramValue; }
      else if(cmd == "set-speed-stepper") { EEPROM.put(STEPPER_SPEED, paramValue); stepperSpeed = paramValue; }
      else if(cmd == "set-p1-stepper") { EEPROM.put(STEPPER_P1, fParamValue); stepperP1 = fParamValue; }
      else if(cmd == "set-p2-stepper") { EEPROM.put(STEPPER_P2, fParamValue); stepperP2 = fParamValue; }
      else if(cmd == "set-p3-stepper") { EEPROM.put(STEPPER_P3, fParamValue); stepperP3 = fParamValue; }
      else if(cmd == "set-p4-stepper") { EEPROM.put(STEPPER_P4, fParamValue); stepperP4 = fParamValue; }
      else if(cmd == "reset") { resetFunc(); }
    }
  }
#endif
}