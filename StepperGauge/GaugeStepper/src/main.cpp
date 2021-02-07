#include <Arduino.h>
#include <Servo.h>
#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

#define STEPS 2048

Servo leftGuage;
Servo rightGauge;
DcsBios::Stepper mainStepper(0x509A, STEPS, 6, 2, 4, 3, 5);
DcsBios::Switch2Pos unsafeLndGearLtTest("UNSAFE_LND_GEAR_LT_TEST", 6);

void setup()
{
  pinMode(13, OUTPUT);
  //  Serial.begin(119200);
  //  Serial.println("GAUGE CONTROLLER");
  leftGuage.attach(10);
  leftGuage.attach(11);
  mainStepper.setSpeed(15);

  DcsBios::setup();
}

unsigned int oldValue = 0;

void onEngineRpmValueChange(unsigned int newValue) {
  int delta = oldValue - newValue;
  mainStepper.step(delta * 2);
  oldValue = newValue;

    /* your code here */
}
DcsBios::IntegerBuffer engineRpmValueBuffer(0x509c, 0xffff, 0, onEngineRpmValueChange);

void onVariometerVviChange(unsigned int newValue) {
    /* your code here */
}
DcsBios::IntegerBuffer variometerVviBuffer(0x503c, 0xffff, 0, onVariometerVviChange);

void onLeftFuelTankValueChange(unsigned int newValue) {
    /* your code here */
}
DcsBios::IntegerBuffer leftFuelTankValueBuffer(0x50b4, 0xffff, 0, onLeftFuelTankValueChange);

boolean toggled;

void loop()
{
  //mainStepper.setSpeed(5);
 // mainStepper.home();
  //mainStepper.step(-600);
  DcsBios::loop();
/*
  for (int idx = 0; idx < 3; ++idx)
  {
    digitalWrite(13, toggled ? LOW : HIGH);
    toggled = !toggled;
    delay(250);
  }

  mainStepper.setSpeed(15);
  

  for (int idx = 0; idx < 3; ++idx)
  {
    digitalWrite(13, toggled ? LOW : HIGH);
    toggled = !toggled;
    delay(250);
  }

  DcsBios::loop();

  Serial.println("HLLO");*/
}