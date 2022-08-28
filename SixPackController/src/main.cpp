#include <Arduino.h>
#include "./DcsBios.h"
#include "../include/AsyncStepper.h"
#include "../include/RotaryEncoder.h"
#include <Servo.h>
#include <Stepper.h>

#define STEPS 2048
long lastSteps = 0;


Stepper altitudeStepper(STEPS, 22, 24, 26, 28);
AsyncStepper  targetHeadingStepper(30, 32, 34, 36);
AsyncStepper headingStepper(23, 25, 27, 29);

RotaryEncoder headingAdjust(46, 48);
RotaryEncoder targetHeadingAdjust(42,44);
RotaryEncoder pressureAdjust(38, 40 );

  DcsBios::IntegerBuffer *hdgDegBuffer;
  DcsBios::IntegerBuffer *altMslFtBuffer;
  DcsBios::IntegerBuffer *slipballBuffer;
  DcsBios::IntegerBuffer *turnIndicatorBuffer;

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
{ 
   headingStepper.update();
   targetHeadingStepper.update();
}

int32_t lastHeading = 0;

  void setCompass(int32_t heading){
    int32_t delta = heading - lastHeading;
    int32_t delta2 = lastHeading - heading;
    lastHeading = heading;
    double scaledvalue = 0;
    if(abs(delta) < abs(delta2))
      scaledvalue = (double)delta * 5.7166;
    else
      scaledvalue = (double)delta2 * 5.7166;

    int32_t steps = (int32_t)scaledvalue;

    int32_t current = headingStepper.getCurrent();
    int32_t newTarget = steps + current;
    
    targetHeadingStepper.setTarget(-                    AAERT(uint32_t)newTarget);
    headingStepper.setTarget((uint32_t)newTarget);

  }

  void setAltimeter(long altitude) {
    double scaledValue = (double)altitude / 0.4875;
    long steps = (long)scaledValue - lastSteps;
    
    altitudeStepper.step(steps);
 
   Serial.println("altimeter:      " + String(altitude) + " " + String(steps));
 
    lastSteps = steps;
  }

  void zeroAltimeter(long delta) {
    double scaledValue = (double)delta / 0.4875;
    altitudeStepper.step(-(long)scaledValue);
    lastSteps = 0;
  }

  void onAltMslFtChange(unsigned int newValue) {
    setAltimeter(newValue);
  }

  void onSlipballChange(unsigned int newValue) {
    Serial.println("slip ball:      " + String(32767 - (int)newValue));
  }

  void onTurnIndicatorChange(unsigned int newValue) {
   Serial.println("turn indicator: " + String( 32767 - (int)newValue));
  }

  void onHdgDegChange(unsigned int newValue) {
    setCompass(newValue);  
   // Serial.println("heading:        " + String(newValue));
  }

void setup() {
  Serial.begin(115200);
  Serial.println("Software Logistics, Aeronatical Division");
  Serial.println("Six Pack Gauge Controller");
  Serial.println("Version 0.1");


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

  sei();

  hdgDegBuffer = new DcsBios::IntegerBuffer(0x0436, 0x01ff, 0, onHdgDegChange);
  altMslFtBuffer  = new DcsBios::IntegerBuffer(0x0434, 0xffff, 0, onAltMslFtChange); 
  slipballBuffer = new DcsBios::IntegerBuffer(0x508e, 0xffff, 0, onSlipballChange);
  turnIndicatorBuffer = new DcsBios::IntegerBuffer(0x504e, 0xffff, 0, onTurnIndicatorChange);

  DcsBios::setup(&Serial1);

  // put your setup code here, to run once:
}

int countDown = 2500;

void loop() {
  checkRotary();
  DcsBios::loop();
}