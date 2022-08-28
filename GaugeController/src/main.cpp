#include <Arduino.h>
#include <Servo.h>
#include <Stepper.h>

#define DCSBIOS_DEFAULT_SERIAL
#include "./lib/DcsBios.h"
#include "../include/AsyncStepper.h"
#include "../include/RotaryEncoder.h"

#include <Arduino.h>

#define STEPS 2048
long lastSteps = 0;

Stepper altitudeStepper(STEPS, 22, 24, 26, 28);
AsyncStepper  targetHeadingStepper(30, 32, 34, 36);
AsyncStepper headingStepper(23, 25, 27, 29);

RotaryEncoder headingAdjust(46, 48);
RotaryEncoder targetHeadingAdjust(42,44);
RotaryEncoder pressureAdjust(38, 40 );


#define TEST_MODE_PIN 9


void cmdHandler(unsigned char cmd, unsigned int address, int value)
{
  switch (cmd)
  {
  case 0x00:
   // mainStepper->home();
    //oldStepperValue = 0;
    break;
  }
}

bool isInTestMode()
{
  return digitalRead(TEST_MODE_PIN) == LOW;

}

Servo turn;
Servo slip;

//DcsBios::ServoOutput flapsControlHandleOutput(0x502a, 9, 544, 2400);
  //DcsBios::ServoOutput flapsControlHandleOutput(0x502a, 10, 544, 2400);

Servo servo_s[15];

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


void setAltimeter(long altitude) {
    double scaledValue = (double)altitude / 0.4875;
    long steps = (long)scaledValue - lastSteps;
    
    altitudeStepper.step(steps);
    lastSteps = steps;
}

void zeroAltimeter(long delta) {
    double scaledValue = (double)delta / 0.4875;
    altitudeStepper.step(-(long)scaledValue);
    lastSteps = 0;
}


void setCompass(int32_t heading){
  //double scaledValues = (double)heading * 5.7166;
  //targetHeadingStepper.setTarget(-(uint32_t)scaledValues);
  //headingStepper.setTarget((uint32_t)scaledValues);
}

  void onHdgDegChange(unsigned int newValue) {
   // setCompass(newValue);  
    Serial2.println("new heading " + String(newValue));
  }


  void onAltMslFtChange(unsigned int newValue) {
    setAltimeter(newValue);
  }

  void onSlipballChange(unsigned int newValue) {
    /* your code here */
  }

  void onTurnIndicatorChange(unsigned int newValue) {
    /* your code here */

  }

void checkRotary() {
  int8_t headingAdjustDelta = headingAdjust.loop();
  int8_t targetAjustDelta = targetHeadingAdjust.loop(); 
  int8_t pressureAdjustDelta = pressureAdjust.loop();

  if(targetAjustDelta > 0){
     targetHeadingStepper.step(-10); 
  }
  else if(targetAjustDelta < 0) {
     targetHeadingStepper.step(+10);   
  }

  if(headingAdjustDelta > 0){
     headingStepper.step( +15); 
  }
  else if(headingAdjustDelta < 0) {
     headingStepper.step(- 15);   
   }

     if(pressureAdjustDelta > 0){
     //altitudeStepper.step(30); 
    Serial.println(headingStepper.getCurrent());
  }
  else if(pressureAdjustDelta < 0) {
   //  altitudeStepper.step(- 30);   
    Serial.println(headingStepper.getCurrent());
   }
}

ISR(TIMER0_COMPA_vect)
{ // timer0 interrupt 2kHz toggles pin 8
  // headingStepper.update();
  // targetHeadingStepper.update();
}



void setSensor(int idx, int value)
{
  switch (idx)
  {
  case 0: /* Turn */
    turn.writeMicroseconds(value);
    break;
  case 1: /* Slip */
    slip.writeMicroseconds(value);    
    break;
  case 4:  
   // pitch.writeMicroseconds(value);
    break;
  case 5:
   // bank.writeMicroseconds(value);
    break;
  case 6: /* Altitude */
    setAltimeter(value);
    break;
  case 7:
    zeroAltimeter(value);
    break;
  case 8:
    //altitudeStepper.step(-lastSteps);
    lastSteps = 0;
    break;
  case 10: /* Heading */
    setCompass(value);
    break;
  case 11: /* Heading Set */
    break;
  }

  Serial.println("completed.");
}

  DcsBios::IntegerBuffer *hdgDegBuffer;
  DcsBios::IntegerBuffer *altMslFtBuffer;
  DcsBios::IntegerBuffer *slipballBuffer;
  DcsBios::IntegerBuffer *turnIndicatorBuffer;


void setup()
{
  resetParser();
/*
    cli(); // stop interrupts

  // set timer0 interrupt at 2kHz
  TCCR0A = 0; // set entire TCCR0A register to 0
  TCCR0B = 0; // same for TCCR0B
  TCNT0 = 0;  // initialize counter value to 0

  // set compare match register for 2khz increments
  OCR0A = 124; // = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);

  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

  sei();*/

  pinMode(TEST_MODE_PIN, INPUT_PULLUP);


 // altitudeStepper.setSpeed(5);

//Serial.begin(250000);

  if(isInTestMode() || true){
    Serial.begin(115200);
    Serial.println("Software Logistics - Aeronatics Division");
    Serial.println("> Test Mode");
  }
  else{
    Serial2.begin(115200);
    Serial2.println("Software Logistics - Aeronatics Division");
    Serial2.println("> Run Mode");   
  }

  hdgDegBuffer = new DcsBios::IntegerBuffer(0x0436, 0x01ff, 0, onHdgDegChange);
  altMslFtBuffer  = new DcsBios::IntegerBuffer(0x0434, 0xffff, 0, onAltMslFtChange); 
  slipballBuffer = new DcsBios::IntegerBuffer(0x508e, 0xffff, 0, onSlipballChange);
  turnIndicatorBuffer = new DcsBios::IntegerBuffer(0x504e, 0xffff, 0, onTurnIndicatorChange);

   if (!isInTestMode())
   {
    //DcsBios::setup();
   }

  //turn.writeMicroseconds(1460);
  //turn.attach(3);
  //slip.writeMicroseconds(1115);
  //slip.attach(4);

 // DcsBios::RegisterCommandCallback(cmdHandler);
}



int counter = 0;
bool forward = true;

int countDown = 5000;

void loop(){
  return;

  //checkRotary();
  if (!isInTestMode())
  {
    //DcsBios::loop();
  }
  counter++;
  if(countDown-- == 0)
  {
    Serial.println("hello world 45 " + String(counter++));
    countDown = 5000;
  }

  return;
  if(isInTestMode())
  {
    int available = Serial.available();
  
    if (available > 0)
    {
      Serial.readBytes(buffer, available);
      for (int idx = 0; idx < available; ++idx)
      {
        byte ch = buffer[idx];
        Serial.println(String(ch));
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
  }
}