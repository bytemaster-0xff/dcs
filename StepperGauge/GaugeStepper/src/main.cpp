#include <Arduino.h>
#include <Servo.h>
#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

#define TESTMODE

#define STEPS 2048

unsigned int oldValue = 0;


DcsBios::Stepper mainStepper(0x509A, STEPS, 6, 2, 4, 3, 5);
DcsBios::Switch2Pos unsafeLndGearLtTest("UNSAFE_LND_GEAR_LT_TEST", 6);

void cmdHandler(unsigned char cmd)
{ 
  switch (cmd)
  {
  case 0x00:
    mainStepper.home();
    oldValue = 0;
    break;
  }
}

#ifdef LEFT_RPM_FUEL
#define STEPPER_OFFSET -375
#define STEPPER_CALIBRATION 0.435
#define STEPPER_SPEED 15
#define HAS_SERVOS
#define STEPPER_ADDRESS 0x509c
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

#define VARIOMETER
#ifdef VARIOMETER
#define STEPPER_OFFSET 0
#define STEPPER_SPEED 15
#define STEPPER_ADDRESS 0x5098
#define P1 -0.0042
#define P2 5.9099
#define P3 -230.85
#endif

#ifdef HAS_SEROVS
Servo leftGauge;
Servo rightGauge;
#endif


void setup()
{
  DcsBios::RegisterCommandCallback(cmdHandler);
  pinMode(13, OUTPUT);

#ifdef HAS_SEROVS  
  leftGauge.attach(11);
  leftGauge.write(0);
  
  rightGauge.attach(10);
  rightGauge.write(110);
#endif

  mainStepper.setSpeed(15);

  mainStepper.setZeroOffset(STEPPER_OFFSET);
  mainStepper.setSpeed(STEPPER_SPEED);

#ifndef TESTMODE
  DcsBios::setup();
#endif

#ifdef TESTMODE
  Serial.begin(115200);
  Serial.println("Welcome - Calibration Mode");
#endif
}

#ifdef MPH
int scale(unsigned int newValue) {
  float fNewValue = newValue;
  return (int)(((fNewValue * fNewValue)  * P1) + (P2 * fNewValue) + P3);
}
#endif

int scale(int newValue) {

}

void onStepperValueChanged(unsigned int newValue)
{

#ifdef STEPPER_CALIBRATION
  int delta = (newValue - oldValue);
  delta *= STEPPER_CALIBRATION;
  mainStepper.step(delta);
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


#if HAS_SERVOS
void onLeftFuelTankValueChange(unsigned int newValue)
{
  /* your code here */
}
DcsBios::IntegerBuffer leftFuelTankValueBuffer(0x50b4, 0xffff, 0, onLeftFuelTankValueChange);
#endif

boolean toggled;

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
      if (cmd == "setstep")
      {
        onStepperValueChanged(paramValue);
      }
#if HAS_SERVOS      
      else if (cmd == "left")
      {
        leftGauge.write(paramValue);
      }
      else if (cmd == "right")
      {
        rightGauge.write(paramValue);
      }
#endif      
    }
  }
#endif
}