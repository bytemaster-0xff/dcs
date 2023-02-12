#include "../../include/AsyncStepper.h"

void AsyncStepper::stepMotor(int thisStep)
{
  switch (thisStep)
  {
  case 0: // 1010
    digitalWrite(motor_pin1, HIGH);
    digitalWrite(motor_pin2, LOW);
    digitalWrite(motor_pin3, HIGH);
    digitalWrite(motor_pin4, LOW);
    break;
  case 1: // 0110
    digitalWrite(motor_pin1, LOW);
    digitalWrite(motor_pin2, HIGH);
    digitalWrite(motor_pin3, HIGH);
    digitalWrite(motor_pin4, LOW);
    break;
  case 2: // 0101
    digitalWrite(motor_pin1, LOW);
    digitalWrite(motor_pin2, HIGH);
    digitalWrite(motor_pin3, LOW);
    digitalWrite(motor_pin4, HIGH);
    break;
  case 3: // 1001
    digitalWrite(motor_pin1, HIGH);
    digitalWrite(motor_pin2, LOW);
    digitalWrite(motor_pin3, LOW);
    digitalWrite(motor_pin4, HIGH);
    break;
  }
}

void AsyncStepper::step(int32_t steps){
  if(steps < 0){
    uint8_t thisStep = 3;
    while(steps++ < 0){
      stepMotor(thisStep);
      if(thisStep == 0){
        thisStep = 3;
      }
      else
        thisStep--;

      delayMicroseconds(5000);
    }
  }
  else if(steps > 0) {
    uint8_t thisStep = 0;
    while(steps-- > 0){
      stepMotor(thisStep);
      if(thisStep == 3){
        thisStep = 0;
      }
      else
        thisStep++;
      
      delayMicroseconds(5000);
    }
  }


}
void AsyncStepper::update()
{
  if(countDown > 0){
    countDown--;
    return;
  }

  if (target == current)
  {
    
  }
  else if (target < current)
  {
    stepMotor(current_step);
    
    current_step--;
    if (current_step == 255)
    {
      current_step = 3;
    }

    current--;
  }
  else if (target > current)
  {
    current++;

    
    stepMotor(current_step);   

    current_step++;
    if (current_step > 3)
    {
      current_step = 0;
    }
  }

  countDown = 10;
}
