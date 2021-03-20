#include <Arduino.h>
#include <Servo.h>
#include <Stepper.h>

#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

#include <Arduino.h>

#define STEPS 2048

Stepper altitudeStepper(STEPS, 22, 24, 26, 28);
Stepper headingStepper(STEPS, 30, 32, 34, 36);
Stepper targetHeadingStepper(STEPS, 23, 25, 27, 29);

Servo mph;
Servo bank;
Servo pitch;
Servo turn;
Servo slip;
Servo variometer;
/* PIN 2 - RPM */

/*DcsBios::ServoOutput ahorizonBank(0x503e,10, 544, 2400);
DcsBios::ServoOutput ahorizonPitch(0x5040,11, 544, 2400);
DcsBios::ServoOutput engineRpm(0x5072,8, 544, 2400);
DcsBios::ServoOutput engineRpm(0x5072,8, 544, 2400);*/

DcsBios::ServoOutput flapsControlHandleOutput(0x502a, 9, 544, 2400);
//DcsBios::ServoOutput flapsControlHandleOutput(0x502a, 10, 544, 2400);

Servo servo_s[15];

void initTestServos()
{
  for (int idx = 2; idx <= 13; ++idx)
  {
    servo_s[idx + 1].attach(idx);
  }

  servo_s[0].attach(44);
  servo_s[1].attach(45);
  servo_s[2].attach(46);
}

byte buffer[128];
byte indexBufferIndex = 0;
char indexBuffer[5];
byte valueBufferIndex = 0;
char valueBuffer[6];

bool readingIndex = true;

void resetParser()
{
  readingIndex = true;
  indexBufferIndex = 0;
  valueBufferIndex = 0;
  for (int idx = 0; idx < 5; ++idx)
  {
    indexBuffer[idx] = 0x00;
  }

  for (int idx = 0; idx < 6; ++idx)
  {
    valueBuffer[idx] = 0x00;
  }
}

void setup()
{
  resetParser();

  altitudeStepper.setSpeed(5);
  headingStepper.setSpeed(5);
  targetHeadingStepper.setSpeed(5);

  Serial.begin(115200);
  Serial.println("Software Logistics - Aeronatics Division");
  Serial.println(">");

  //DcsBios::setup();
  ///initTestServos();

  mph.writeMicroseconds(400);
  mph.attach(3);
  bank.writeMicroseconds(1500);
  bank.attach(6);
  pitch.writeMicroseconds(1170);
  pitch.attach(5);
  turn.writeMicroseconds(1400);
  turn.attach(4);
  slip.writeMicroseconds(1400);
  slip.attach(3);
  variometer.writeMicroseconds(1400);
  variometer.attach(2);

  // put your setup code here, to run once:
}

void setSensor(int idx, int value)
{
  Serial.println("Sending value: " + String(idx) + "=" + String(value));

  switch (idx)
  {
  case 0: /* Turn */
    turn.writeMicroseconds(value);
    break;
  case 1: /* Slip */
    slip.writeMicroseconds(value);    
    break;
  case 2: /* variometer */
    variometer.writeMicroseconds(value);  
    break;
  case 3: 
    mph.writeMicroseconds(value);    
    break;
  case 4:  
    pitch.writeMicroseconds(value);
    break;
  case 5:
    bank.writeMicroseconds(value);
    break;
  case 6: /* Altitude */
    altitudeStepper.step(value);
    break;
  case 7: /* Heading */
    headingStepper.step(value);
    break;
  case 8: /* Heading Set */
    targetHeadingStepper.step(value);
    break;
  }

  Serial.println("completed.");
}

int counter = 0;
bool forward = true;

void loop()
{
  int available = Serial.available();

  if (available > 0)
  {
    Serial.readBytes(buffer, available);
    for (int idx = 0; idx < available; ++idx)
    {
      byte ch = buffer[idx];
      Serial.print(char(ch));
      if (ch == ' ')
      {
        readingIndex = false;
      }
      else if (ch == '\n')
      {
        int sensorIndex = atoi(indexBuffer);
        int sensorValue = atoi(valueBuffer);
        Serial.println();
        setSensor(sensorIndex, sensorValue);
        resetParser();
      }
      else
      {
        if (readingIndex)
        {
          indexBuffer[indexBufferIndex++] = ch;
        }
        else
        {
          valueBuffer[valueBufferIndex++] = ch;
        }
      }
    }
  }

  // put your main code here, to run repeatedly:
  //DcsBios::loop();

#ifdef STEPPER_TEST
  char ch = Serial.read();
  if (ch == 'z')
  {
    //counter++;
    forward = true;
  }
  else if (ch == 'x')
  {
    //ounter--;
    forward = false;
  }

  //stepper.step(1);
  if (forward)
  {
    counter++;
  }
  else
  {
    counter--;
  }

  int step = counter % 4;
  if (step < 0)
  {
    step = step + 4;
  }
  altitudeStepper.stepMotor(step);
  headingStepper.stepMotor(step);
  Serial.println("Steps: " + String(counter) + "  step: " + String(step));
#endif
}