#include <Arduino.h>

//#define PIN_DETAIL
#define NUMBER_PINS 12

#include "buttons.h"

#define FRONT_TILT_UP 39
#define FRONT_TILT_DOWN 41

#define SLIDE_FORWARD 43
#define SLIDE_BACK 45

#define TILT_BACK 47
#define TILT_FORWARD 49

#define BACK_TILT_UP 51
#define BACK_TILT_DOWN 53

#define EJECT 30
#define BTN1_UP 32
#define BTN1_DN 34

#define BTN2_UP 36
#define BTN2_DN 38

#define BTN3_UP 40
#define BTN3_DN 42

#define BTN4_UP 44
#define BTN4_DN 46

#define CANOPY_UP 48
#define CANOPY_DN 50

void mqttPublish(String value)
{
  Serial.println(value);
  Serial3.print(value + '\r');
}

uint8_t outputs[8];
uint8_t index;

void setup()
{
  Serial.begin(115200);
  Serial3.begin(115200);
  outputs[0] = TILT_FORWARD;
  outputs[1] = TILT_BACK;
  
  outputs[2] = FRONT_TILT_DOWN;
  outputs[3] = FRONT_TILT_UP;

  outputs[4] = BACK_TILT_UP;
  outputs[5] = BACK_TILT_DOWN;

  outputs[6] = SLIDE_FORWARD;
  outputs[7] = SLIDE_BACK;
  
  inputs[0] = BTN1_UP;
  inputs[1] = BTN1_DN;

  inputs[2] = BTN2_UP;
  inputs[3] = BTN2_DN;

  inputs[4] = BTN3_UP;
  inputs[5] = BTN3_DN;

  inputs[6] = BTN4_UP;
  inputs[7] = BTN4_DN;

  inputs[8] = CANOPY_UP;
  inputs[9] = (CANOPY_UP << 8) | CANOPY_DN;
  inputs[10] = CANOPY_DN;

  inputs[11] = EJECT;

  for (int idx = 0; idx < 7; ++idx)
  {
    pressTopics[idx] = "";
    releaseTopics[idx] = "";
  }

  pressTopics[8] = "canopy/close";
  pressTopics[9] = "canopy/hold";
  pressTopics[10] = "canopy/open";

  releaseTopics[8] = "";
  releaseTopics[9] = "";
  releaseTopics[10] = "";

  pressTopics[11] = "aces2/eject";
  releaseTopics[11] = "aces2/arm";

  for(int idx = 0; idx < 8; ++idx){
    pinMode(outputs[idx], OUTPUT);
  }

  index = 6;

  initButtons();

  pinMode(EJECT, INPUT_PULLUP);
}

String content = "";

void mqttHandle(String msg){
  msg.trim();
  if(msg == "aces2/seat/tilt/forward"){
    digitalWrite(outputs[0], HIGH);
    delay(1000);
    digitalWrite(outputs[0], LOW);

    Serial.println("move seat forward");
  }
  else if(msg == "aces2/seat/tilt/back"){
    digitalWrite(outputs[1], HIGH);
    delay(1000);
    digitalWrite(outputs[1], LOW);    
  }
  else if(msg == "aces2/seat/front/lower"){
    digitalWrite(outputs[2], HIGH);
    delay(1000);
    digitalWrite(outputs[2], LOW);    
  }
  else if(msg == "aces2/seat/front/raise"){
    digitalWrite(outputs[3], HIGH);
    delay(1000);
    digitalWrite(outputs[3], LOW);    
  }
  else if(msg == "aces2/seat/back/raise"){
    digitalWrite(outputs[4], HIGH);
    delay(1000);
    digitalWrite(outputs[4], LOW);    
  }
  else if(msg == "aces2/seat/back/lower"){
    digitalWrite(outputs[5], HIGH);
    delay(1000);
    digitalWrite(outputs[5], LOW);    
  }  
  else if(msg == "aces2/seat/slide/back"){  
    digitalWrite(outputs[6], HIGH);
    delay(1000);
    digitalWrite(outputs[6], LOW);    
  }  
  else if(msg == "aces2/seat/slide/forward"){
    digitalWrite(outputs[7], HIGH);
    delay(1000);
    digitalWrite(outputs[7], LOW);    
  }  
  else if(msg == "SENDALL") {
    sendCurrentPinState();
  }


  Serial.println(msg);
}

void loop()
{
  delay(250);

  for(uint8_t idx = 0; idx < 8; ++idx){
    digitalWrite(outputs[idx],  digitalRead(inputs[idx]));  
  }

  while(Serial3.available() > 0){
    int ch = Serial3.read();
    if(ch == 0x0D){
      mqttHandle(content);
      content = "";
    }    
    else if(ch != 0x00)
      content += (char)ch;
  }

  checkButtons();
}
