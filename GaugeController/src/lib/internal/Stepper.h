#include <Arduino.h>

namespace DcsBios
{
  class Stepper
  {
  private:
    int m_endStopPin;
    int motor_pin_1;
    int motor_pin_2;
    int motor_pin_3;
    int motor_pin_4;
    int direction;                // Direction of rotation
    unsigned long step_delay;     // delay between steps, in ms, based on speed
    int number_of_steps;          // total number of steps this motor can take
    unsigned long last_step_time; // time stamp in us of when the last step was taken
    int step_number;              // which step the motor is on
    int zero_offset;
    void stepMotor(int thisStep)
    {
      switch (thisStep)
      {
      case 0: // 1010
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, LOW);
        break;
      case 1: // 0110
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, LOW);
        break;
      case 2: //0101
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, HIGH);
        break;
      case 3: //1001
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, HIGH);
        break;
      }
    }

  public:
    Stepper(unsigned int address, int number_of_steps, int endStopPin, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4) 
    {
      m_endStopPin = endStopPin;
      this->step_number = 0;                   // which step the motor is on
      this->direction = 0;                     // motor direction
      this->last_step_time = 0;                // time stamp in us of the last step taken
      this->number_of_steps = number_of_steps; // total number of steps for this motor

      // Arduino pins for the motor control connection:
      this->motor_pin_1 = motor_pin_1;
      this->motor_pin_2 = motor_pin_2;
      this->motor_pin_3 = motor_pin_3;
      this->motor_pin_4 = motor_pin_4;

      // setup the pins on the microcontroller:
      pinMode(this->motor_pin_1, OUTPUT);
      pinMode(this->motor_pin_2, OUTPUT);
      pinMode(this->motor_pin_3, OUTPUT);
      pinMode(this->motor_pin_4, OUTPUT);

      pinMode(endStopPin, INPUT_PULLUP);
    }

    void setZeroOffset(long offset) {
      zero_offset = offset;
    }

    void pollInput() {

    }

    void setSpeed(long whatSpeed)
    {
      this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
    }

    void home()
    {
      int steps_to_move = 5000;
      
      // If the end stop is low the zero offset is triggered, we don't necessarily know that 
      // this is the actual zero, move it 600 steps
      if (digitalRead(m_endStopPin) == LOW)
      {
          step(600);
      }
      
      unsigned long last_step_micros = micros();
      // decrement the number of steps, moving one step each time:
      while (steps_to_move > 0)
      {        
          unsigned long now = micros();
          // move only if the appropriate delay has passed:
          if (now - last_step_micros >= this->step_delay)
          {
            if (digitalRead(m_endStopPin) == LOW)
            {
              steps_to_move = 0;
            }

            last_step_micros = now;
            steps_to_move--;
            stepMotor(steps_to_move % 4);
          }
      }

      step(zero_offset);
    }

    void step(int steps_to_move)
    {
      int steps_left = abs(steps_to_move); // how many steps to take

      // determine direction based on whether steps_to_mode is + or -:
      if (steps_to_move > 0)
      {
        this->direction = 1;
      }
      if (steps_to_move < 0)
      {
        this->direction = 0;
      }

      // decrement the number of steps, moving one step each time:
      while (steps_left > 0)
      {
        unsigned long now = micros();
        // move only if the appropriate delay has passed:
        if (now - this->last_step_time >= this->step_delay)
        {
          // get the timeStamp of when you stepped:
          this->last_step_time = now;
          // increment or decrement the step number,
          // depending on direction:
          if (this->direction == 1)
          {
            this->step_number++;
            if (this->step_number == this->number_of_steps)
            {
              this->step_number = 0;
            }
          }
          else
          {
            if (this->step_number == 0)
            {
              this->step_number = this->number_of_steps;
            }
            this->step_number--;
          }
          // decrement the steps left:
          steps_left--;
          // step the motor to step number 0, 1, ..., {3 or 10}

          stepMotor(this->step_number % 4);
        }
      }
    }
  };
} // namespace DcsBios