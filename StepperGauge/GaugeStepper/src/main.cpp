#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>
#define DCSBIOS_DEFAULT_SERIAL
#include "./lib/dcsbios.h"

#define STEPS 2048
#define SERVO_MAX 2400

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

int stepperOffset = 0;
unsigned int stepperSpeed = 15;

unsigned char stepperMapFunction = 0;

double stepperP1 = 0.0;
double stepperP2 = 1.0;
double stepperP3 = 1.0;
double stepperP4 = 1.0;
double stepperP5 = 1.0;
double stepperCenter = 500.0;
double stepper2xP1 = 0.0;
double stepper2xP2 = 1.0;
double stepper2xP3 = 1.0;
double stepper2xP4 = 1.0;
double stepper2xP5 = 1.0;
double stepperMinValue = 0;
double stepperMaxValue = 0;

int oldStepperValue = 0;

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

#define STEPPER_EXISTS SERVO_LEFT_MAX_VALUE + sizeof(unsigned int)
#define STEPPER_ADDR STEPPER_EXISTS + sizeof(unsigned int)
#define STEPPER_OFFSET STEPPER_ADDR + sizeof(unsigned int)
#define STEPPER_SPEED STEPPER_OFFSET + sizeof(unsigned int)
#define STEPPER_MAP_FUNCTION STEPPER_SPEED + sizeof(unsigned int)
#define STEPPER_P1 STEPPER_MAP_FUNCTION + sizeof(byte)
#define STEPPER_P2 STEPPER_P1 + sizeof(double)
#define STEPPER_P3 STEPPER_P2 + sizeof(double)
#define STEPPER_P4 STEPPER_P3 + sizeof(double)
#define STEPPER_P5 STEPPER_P4 + sizeof(double)

#define STEPPER_CENTER STEPPER_P5 + sizeof(double)

#define STEPPER_2X_P1 STEPPER_CENTER + sizeof(double)
#define STEPPER_2X_P2 STEPPER_2X_P1 + sizeof(double)
#define STEPPER_2X_P3 STEPPER_2X_P2 + sizeof(double)
#define STEPPER_2X_P4 STEPPER_2X_P3 + sizeof(double)
#define STEPPER_2X_P5 STEPPER_2X_P4 + sizeof(double)
#define STEPPER_MIN_VALUE STEPPER_2X_P5 + sizeof(double)
#define STEPPER_MAX_VALUE STEPPER_MIN_VALUE + sizeof(double)
#define END_EEPROM STEPPER_MAX_VALUE + sizeof(double)

#define TEST_MODE_PIN 9

#define INITIALIZED_VALUE 0x1F1F1F1F
Servo leftGauge;
Servo rightGauge;

DcsBios::Stepper *mainStepper;

DcsBios::IntegerBuffer *stepperValueBuffer;
DcsBios::IntegerBuffer *rightServoValueBuffer;
DcsBios::IntegerBuffer *leftServoValueBuffer;

void cmdHandler(unsigned char cmd, unsigned int address, int value)
{
  switch (cmd)
  {
  case 0x00:
    mainStepper->home();
    oldStepperValue = 0;
    break;
  }
}

bool isInTestMode()
{
  return digitalRead(TEST_MODE_PIN) == LOW;

}

void printAddress(unsigned int addr)
{
  char str[10];
  sprintf(str, "0x%x", addr);
  Serial.println(str);
}

void PrintValue(String string)
{
  Serial.println(string);
  Serial.flush();
}

void WriteDouble(double value)
{
  if (isnan(value))
    Serial.println(F("NAN"));
  else
    Serial.println(String(value, 12));
}

void onStepperValueChanged(double newValue, bool raw)
{
  if(newValue < stepperMinValue || newValue > stepperMaxValue){
    Serial.print("Stepper Out Of Range ");
    Serial.println(newValue);
    Serial.println();
    return;
  }

  double steps = 0;

  if (!raw)
  {
    if (newValue == 0)
    {
      steps = newValue;
    }
    else
    {
      switch (stepperMapFunction)
      {
      case 0: /* NOP */;
        break;
      case 1:
        steps = (newValue * stepperP2) + stepperP1;
        break;
      case 2:
        steps = (pow(newValue, 2) * stepperP3) + (newValue * stepperP2) + stepperP1;
        break;
      case 3:
        steps = (pow(newValue, 3) * stepperP4) + (pow(newValue, 2) * stepperP3) + (newValue * stepperP2) + stepperP1;
        break;
      case 4:
        steps = (pow(newValue, 4) * stepperP5) + (pow(newValue, 3) * stepperP4) + (pow(newValue, 2) * stepperP3) + (newValue * stepperP2) + stepperP1;
        break;
      case 5:
        if (newValue < stepperCenter)
        {
          steps = (newValue * stepperP2) + stepperP1;
        }
        else
        {
          steps = (newValue * stepper2xP2) - stepper2xP1;
        }
        break;
      case 6:
        if (steps < stepperCenter)
        {
          steps = (pow(newValue, 2) * stepperP3) + (newValue * stepperP2) + stepperP1;
        }
        else
        {
          steps = (pow(newValue, 2) * stepper2xP3) + (newValue * stepper2xP2) + stepper2xP1;
        }
        break;
      case 7:
        if (steps < stepperCenter)
        {
          steps = (pow(newValue, 3) * stepperP4) + (pow(newValue, 2) * stepperP3) + (newValue * stepperP2) + stepperP1;
        }
        else
        {
          steps = (pow(newValue, 3) * stepper2xP4) + (pow(newValue, 2) * stepper2xP3) + (newValue * stepper2xP2) + stepper2xP1;
        }
      case 8:
        if (steps < stepperCenter)
        {
          steps = (pow(newValue, 4) * stepperP5) + (pow(newValue, 3) * stepperP4) + (pow(newValue, 2) * stepperP3) + (newValue * stepperP2) + stepperP1;
        }
        else
        {
          steps = (pow(newValue, 4) * stepper2xP5) + (pow(newValue, 3) * stepper2xP4) + (pow(newValue, 2) * stepper2xP3) + (newValue * stepper2xP2) + stepper2xP1;
        }
        break;
        break;
      }
    }
  }
  else
  {
    steps = newValue;
  }

  int delta = ((int)steps - oldStepperValue);
  
  if (isInTestMode() || true)
  {
    Serial.print(F("Value="));
    Serial.print(newValue);
    Serial.print(F("Setting steps="));
    Serial.print(oldStepperValue);
    Serial.print(F("; New Value="));
    Serial.print(steps);
    Serial.print(F("; Delta="));
    Serial.print(delta);
    Serial.println(F(";"));
  }

  mainStepper->step(delta);

  oldStepperValue = steps;
}

void onStepperValueChanged(unsigned int newValue)
{
  onStepperValueChanged(newValue, false);
}

void leftServoChanged(unsigned int newValue)
{
if(newValue < servoLeftMinValue || newValue > servoLeftMaxValue){
    Serial.print("Left Servo Out Of Range ");
    Serial.println(newValue);
    Serial.println();
    return;
  }

  if (isInTestMode()|| true)
  {
    Serial.print(F("Setting left servo="));
    Serial.println(newValue);
   // 	DcsBios::PollingInput::setMessageSentOrQueued();
  }

  int microSecondRange = servoLeftMax - servoLeftMin;
  int range = servoLeftMaxValue - servoLeftMinValue;
  if (range > 0)
  {
    float ratio = (float)newValue / (float)range;
    unsigned int newMS = microSecondRange * ratio;
    leftGauge.writeMicroseconds(servoLeftMin + newMS);
  }
}

void rightServoChanged(unsigned int newValue)
{
if(newValue < servoRightMinValue || newValue > servoRightMaxValue){
    Serial.print("Right Servo Out Of Range ");
    Serial.println(newValue);
    Serial.println();
    return;
  }


  if (isInTestMode())
  {
    Serial.print(F("Setting right servo="));
    Serial.println(newValue);
//    	DcsBios::PollingInput::setMessageSentOrQueued();
  }

  int microSecondRange = servoRightMax - servoRightMin;
  unsigned int range = servoRightMaxValue - servoRightMinValue;
  if (range > 0)
  {
    float ratio = (float)newValue / (float)range;
    int newMS = microSecondRange * ratio;
    if(isInTestMode())
      Serial.println(String(microSecondRange) + "," + String(range) + "," + String(ratio) + "," + String(newMS));
    
    rightGauge.writeMicroseconds(SERVO_MAX - (servoRightMin + newMS));
  }
}

void loadSettings()
{
  unsigned long initValue;
  EEPROM.get(0, initValue);
  if (initValue != INITIALIZED_VALUE)
  {

    if (isInTestMode())
    {
      Serial.println("Was not initialized - setting parameters");
    }

    EEPROM.put(SERVO_LEFT_MIN, servoLeftMin);
    EEPROM.put(SERVO_LEFT_MAX, servoLeftMax);
    EEPROM.put(SERVO_RIGHT_MIN, servoRightMin);
    EEPROM.put(SERVO_RIGHT_MAX, servoRightMax);

    EEPROM.put(SERVO_LEFT_MIN_VALUE, servoLeftMinValue);
    EEPROM.put(SERVO_LEFT_MAX_VALUE, servoLeftMaxValue);
    EEPROM.put(SERVO_RIGHT_MIN_VALUE, servoRightMinValue);
    EEPROM.put(SERVO_RIGHT_MAX_VALUE, servoRightMaxValue);

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
    EEPROM.put(STEPPER_P5, stepperP5);
    EEPROM.put(STEPPER_CENTER, stepperCenter);
    EEPROM.put(STEPPER_2X_P1, stepper2xP1);
    EEPROM.put(STEPPER_2X_P2, stepper2xP2);
    EEPROM.put(STEPPER_2X_P3, stepper2xP3);
    EEPROM.put(STEPPER_2X_P4, stepper2xP4);
    EEPROM.put(STEPPER_2X_P5, stepper2xP5);
    EEPROM.put(STEPPER_MIN_VALUE, stepperMinValue);
    EEPROM.put(STEPPER_MAX_VALUE, stepperMaxValue);

    EEPROM.put(0, (unsigned long)INITIALIZED_VALUE);
  }
  else
  {
    if (isInTestMode())
    {
      Serial.println("Was initialized - Loading Parameters");
    }

    EEPROM.get(SERVO_LEFT_MIN, servoLeftMin);
    EEPROM.get(SERVO_LEFT_MAX, servoLeftMax);
    EEPROM.get(SERVO_RIGHT_MIN, servoRightMin);
    EEPROM.get(SERVO_RIGHT_MAX, servoRightMax);

    EEPROM.get(SERVO_LEFT_MIN_VALUE, servoLeftMinValue);
    EEPROM.get(SERVO_LEFT_MAX_VALUE, servoLeftMaxValue);
    EEPROM.get(SERVO_RIGHT_MIN_VALUE, servoRightMinValue);
    EEPROM.get(SERVO_RIGHT_MAX_VALUE, servoRightMaxValue);

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
    EEPROM.get(STEPPER_P5, stepperP5);
    EEPROM.get(STEPPER_CENTER, stepperCenter);
    EEPROM.get(STEPPER_2X_P1, stepper2xP1);
    EEPROM.get(STEPPER_2X_P2, stepper2xP2);
    EEPROM.get(STEPPER_2X_P3, stepper2xP3);
    EEPROM.get(STEPPER_2X_P4, stepper2xP4);
    EEPROM.get(STEPPER_2X_P5, stepper2xP5);
    EEPROM.get(STEPPER_MIN_VALUE, stepperMinValue);
    EEPROM.get(STEPPER_MAX_VALUE, stepperMaxValue);

    if (isnan(stepperP1))
      stepperP1 = 0.0;
    if (isnan(stepperP2))
      stepperP2 = 1.0;
    if (isnan(stepperP3))
      stepperP3 = 1.0;
    if (isnan(stepperP4))
      stepperP4 = 1.0;
    if (isnan(stepperP5))
      stepperP5 = 1.0;

    if (isnan(stepper2xP1))
      stepper2xP1 = 0.0;
    if (isnan(stepper2xP2))
      stepper2xP2 = 1.0;
    if (isnan(stepper2xP3))
      stepper2xP3 = 1.0;
    if (isnan(stepper2xP4))
      stepper2xP4 = 1.0;
    if (isnan(stepper2xP5))
      stepper2xP5 = 1.0;

    if (isnan(stepperCenter))
      stepperCenter = 0;
    if (isnan(stepperMinValue))
      stepperMinValue = 0;
    if (isnan(stepperMaxValue))
      stepperMaxValue = 100;
  }

  if (isInTestMode())
  {
    Serial.println();
    Serial.print(F("EEPROM USED        :"));
    Serial.println(END_EEPROM);
    Serial.print(F("Left Servo Exists  : "));
    Serial.println((servoLeftExists) ? F("true") : F("false"));
    Serial.print(F("Left Servo Address : "));
    printAddress(servoLeftAddress);
    Serial.print(F("Left Servo Min     : "));
    Serial.println(servoLeftMin);
    Serial.print(F("Left Servo Max     : "));
    Serial.println(servoLeftMax);
    Serial.print(F("Left Servo Min Val : "));
    Serial.println(servoLeftMinValue);
    Serial.print(F("Left Servo Max Val : "));
    Serial.print(servoLeftMaxValue);
    Serial.flush();
    Serial.println();
    Serial.print(F("Right Servo Exists : "));
    Serial.println(servoRightExists ? F("true") : F("false"));
    Serial.print("Right Servo Address: ");
    printAddress(servoRightAddress);
    Serial.print(F("Right Servo Min    : "));
    Serial.println(servoRightMin);
    Serial.print(F("Right Servo Max    : "));
    Serial.println(servoRightMax);
    Serial.print(F("Right Servo Min Val: "));
    Serial.println(servoRightMinValue);
    Serial.print(F("Right Servo Max Val: "));
    Serial.println(servoRightMaxValue);
    Serial.println();
    Serial.flush();
    Serial.print(F("Stepper Exists     : "));
    Serial.println(stepperExists ? F("true") : F("false"));
    Serial.print(F("Stepper Address    : "));
    printAddress(stepperAddress);
    Serial.print("Stepper Speed      : ");
    Serial.println(stepperSpeed);
    Serial.print(F("Stepper Zero Offset: "));
    Serial.println(stepperOffset);
    Serial.print(F("Stepper Mapping Fun: "));
    Serial.flush();
    switch (stepperMapFunction)
    {
    case 0:
      Serial.println(F("none"));
      break;
    case 1:
      Serial.println(F("linear"));
      break;
    case 2:
      Serial.println(F("2nd order"));
      break;
    case 3:
      Serial.println(F("3rd order"));
      break;
    case 4:
      Serial.println(F("4rd order"));
      break;
    case 5:
      Serial.println(F("2x linear"));
      break;
    case 6:
      Serial.println(F("2x 2nd Order"));
      break;
    case 7:
      Serial.println(F("2x 3nd Order"));
      break;
    case 8:
      Serial.println(F("2x 4nd Order"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
    }
    Serial.print(F("Stepper P1         : "));
    WriteDouble(stepperP1);
    Serial.print(F("Stepper P2         : "));
    WriteDouble(stepperP2);
    Serial.print(F("Stepper P3         : "));
    WriteDouble(stepperP3);
    Serial.print(F("Stepper P4         : "));
    WriteDouble(stepperP4);
    Serial.print(F("Stepper P5         : "));
    WriteDouble(stepperP5);
    Serial.print(F("Stepper Center     : "));
    WriteDouble(stepperCenter);
    Serial.flush();
    Serial.print(F("Stepper 2x P1      : "));
    WriteDouble(stepper2xP1);
    Serial.print(F("Stepper 2x P2      : "));
    WriteDouble(stepper2xP2);
    Serial.print(F("Stepper 2x P3      : "));
    WriteDouble(stepper2xP3);
    Serial.print(F("Stepper 2x P4      : "));
    WriteDouble(stepper2xP4);
    Serial.print(F("Stepper 2x P5      : "));
    WriteDouble(stepper2xP5);
    Serial.print(F("Stepper Min Val    : "));
    WriteDouble(stepperMinValue);
    Serial.print(F("Stepper Max Val    : "));
    WriteDouble(stepperMaxValue);

    Serial.flush();
  }

  if (stepperExists)
  {
    mainStepper = new DcsBios::Stepper(stepperAddress, STEPS, 6, 2, 4, 3, 5);
    stepperValueBuffer = new DcsBios::IntegerBuffer(stepperAddress, 0xffff, 0, onStepperValueChanged);
    mainStepper->setZeroOffset(stepperOffset);
    mainStepper->setSpeed(stepperSpeed);
  }

  if (servoRightExists)
  {
    rightServoValueBuffer = new DcsBios::IntegerBuffer(servoRightAddress, 0xffff, 0, rightServoChanged);
    rightGauge.attach(10);
    rightGauge.writeMicroseconds(SERVO_MAX - servoRightMin);
  }

  if (servoLeftExists)
  {
    leftServoValueBuffer = new DcsBios::IntegerBuffer(servoLeftAddress, 0xffff, 0, leftServoChanged);
    leftGauge.attach(11);
    leftGauge.writeMicroseconds(servoLeftMin);
  }
}

void setup()
{
  pinMode(TEST_MODE_PIN, INPUT_PULLUP);

  DcsBios::RegisterCommandCallback(cmdHandler); 

  if (isInTestMode())
  {
    Serial.begin(115200);
    Serial.println();
    Serial.println(F("Welcome - Test Mode"));
    Serial.println(F("VERSION 0.8"));
    Serial.println(F("============================================"));
    Serial.println(digitalRead(TEST_MODE_PIN) == HIGH ? "HIGH " : "LOW");
  }

  loadSettings();

  if (!isInTestMode())
  {
    DcsBios::setup();
  }

  mainStepper->home();
  Serial.print("startup complete");
  DcsBios::PollingInput::setMessageSentOrQueued();
}



boolean toggled;

void (*resetFunc)(void) = 0;

void loop()
{
  if (!isInTestMode())
  {
    DcsBios::loop();
  }

  if (isInTestMode())
  {
    String input = Serial.readStringUntil('\n');
    if (input.length() > 0)
    {
      if (input == F("zero"))
      {
        Serial.println(F("zero"));
        oldStepperValue = 0;
        mainStepper->home();
        leftGauge.writeMicroseconds(servoLeftMin);
        rightGauge.writeMicroseconds(SERVO_MAX - servoRightMin);
      }
      else
      {
        String cmd = input.substring(0, input.indexOf(" "));
        String param = input.substring(input.indexOf(" ") + 1);
        Serial.print(F("Handling: "));
        Serial.println(cmd + " - " + param);
        int paramValue = atoi(param.c_str());
        int hexAddr = strtol(param.c_str(), NULL, 16);
        double dParamValue = strtod(param.c_str(), NULL);

        Serial.print(F("Parsed Double Value    : "));
        WriteDouble(dParamValue);

        if (cmd == F("stepper"))
          onStepperValueChanged(dParamValue, false);
        else if (cmd == F("stepper-raw"))
          onStepperValueChanged(paramValue, true);
        else if (cmd == F("left"))
          leftServoChanged(paramValue);
        else if (cmd == F("right"))
          rightServoChanged(paramValue);
        else if (cmd == F("left-raw"))
          leftGauge.writeMicroseconds(paramValue);
        else if (cmd == F("right-raw"))
          rightGauge.writeMicroseconds(SERVO_MAX - paramValue);

        else if (cmd == F("set-min-left"))
        {
          EEPROM.put(SERVO_LEFT_MIN, paramValue);
          servoLeftMin = paramValue;
        }
        else if (cmd == F("set-max-left"))
        {
          EEPROM.put(SERVO_LEFT_MAX, paramValue);
          servoLeftMax = paramValue;
        }
        else if (cmd == F("set-min-right"))
        {
          EEPROM.put(SERVO_RIGHT_MIN, paramValue);
          servoRightMin = paramValue;
        }
        else if (cmd == F("set-max-right"))
        {
          EEPROM.put(SERVO_RIGHT_MAX, paramValue);
          servoRightMax = paramValue;
        }

        else if (cmd == F("set-minvalue-left"))
        {
          EEPROM.put(SERVO_LEFT_MIN_VALUE, paramValue);
          servoLeftMinValue = paramValue;
        }
        else if (cmd == F("set-maxvalue-left"))
        {
          EEPROM.put(SERVO_LEFT_MAX_VALUE, paramValue);
          servoLeftMaxValue = paramValue;
        }
        else if (cmd == F("set-minvalue-right"))
        {
          EEPROM.put(SERVO_RIGHT_MIN_VALUE, paramValue);
          servoRightMinValue = paramValue;
        }
        else if (cmd == F("set-maxvalue-right"))
        {
          EEPROM.put(SERVO_RIGHT_MAX_VALUE, paramValue);
          servoRightMaxValue = paramValue;
        }

        else if (cmd == F("set-addr-left"))
        {
          EEPROM.put(SERVO_LEFT_ADDR, hexAddr);
          servoLeftAddress = hexAddr;
        }
        else if (cmd == F("set-addr-right"))
        {
          EEPROM.put(SERVO_RIGHT_ADDR, hexAddr);
          servoRightAddress = hexAddr;
        }
        else if (cmd == F("set-addr-stepper"))
        {
          EEPROM.put(STEPPER_ADDR, hexAddr);
          stepperAddress = hexAddr;
        }

        else if (cmd == F("set-exists-stepper"))
        {
          EEPROM.put(STEPPER_EXISTS, (byte)paramValue);
          stepperExists = paramValue;
        }
        else if (cmd == F("set-exists-left"))
        {
          EEPROM.put(SERVO_LEFT_EXISTS, (byte)paramValue);
          servoLeftExists = paramValue;
        }
        else if (cmd == F("set-exists-right"))
        {
          EEPROM.put(SERVO_RIGHT_EXISTS, (byte)paramValue);
          servoRightExists = paramValue;
        }

        else if (cmd == F("set-mapfunc-stepper"))
        {
          EEPROM.put(STEPPER_MAP_FUNCTION, (byte)paramValue);
          stepperMapFunction = (byte)paramValue;
        }
        else if (cmd == F("set-offset-stepper"))
        {
          EEPROM.put(STEPPER_OFFSET, paramValue);
          stepperOffset = paramValue;
          mainStepper->setZeroOffset(paramValue);
        }
        else if (cmd == F("set-speed-stepper"))
        {
          EEPROM.put(STEPPER_SPEED, paramValue);
          stepperSpeed = paramValue;
        }
        else if (cmd == F("set-p1-stepper"))
        {
          EEPROM.put(STEPPER_P1, dParamValue);
          stepperP1 = dParamValue;
        }
        else if (cmd == F("set-p2-stepper"))
        {
          EEPROM.put(STEPPER_P2, dParamValue);
          stepperP2 = dParamValue;
        }
        else if (cmd == F("set-p3-stepper"))
        {
          EEPROM.put(STEPPER_P3, dParamValue);
          stepperP3 = dParamValue;
        }
        else if (cmd == F("set-p4-stepper"))
        {
          EEPROM.put(STEPPER_P4, dParamValue);
          stepperP4 = dParamValue;
        }
        else if (cmd == F("set-p5-stepper"))
        {
          EEPROM.put(STEPPER_P5, dParamValue);
          stepperP5 = dParamValue;
        }
        else if (cmd == F("set-2xp1-stepper"))
        {
          EEPROM.put(STEPPER_P1, dParamValue);
          stepper2xP1 = dParamValue;
        }
        else if (cmd == F("set-2xp2-stepper"))
        {
          EEPROM.put(STEPPER_P2, dParamValue);
          stepper2xP2 = dParamValue;
        }
        else if (cmd == F("set-2xp3-stepper"))
        {
          EEPROM.put(STEPPER_P3, dParamValue);
          stepper2xP3 = dParamValue;
        }
        else if (cmd == F("set-2xp4-stepper"))
        {
          EEPROM.put(STEPPER_P4, dParamValue);
          stepper2xP4 = dParamValue;
        }
        else if (cmd == F("set-2xp5-stepper"))
        {
          EEPROM.put(STEPPER_P5, dParamValue);
          stepper2xP5 = dParamValue;
        }
        else if (cmd == F("set-cen-stepper"))
        {
          EEPROM.put(STEPPER_CENTER, dParamValue);
          stepperCenter = dParamValue;
        }
        else if (cmd == F("set-minvalue-stepper"))
        {
          EEPROM.put(STEPPER_MIN_VALUE, dParamValue);
          stepperMinValue = dParamValue;
        }
        else if (cmd == F("set-maxvalue-stepper"))
        {
          EEPROM.put(STEPPER_MAX_VALUE, dParamValue);
          stepperMaxValue = dParamValue;
        }
        else if (cmd == F("reset"))
        {
          delay(50);
          resetFunc();
        }
        else
        {
          Serial.print(F("unknown => "));
          Serial.println(cmd);
        }
      }
    }
  }
}