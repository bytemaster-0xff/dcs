#include <Arduino.h>


#ifndef AsyncStepper_h
#define AsyncStepper_h

// library interface description
class AsyncStepper {


    private: 
        uint16_t countDown = 0;

        uint8_t current_step = 0;

        uint8_t motor_pin1;
        uint8_t motor_pin2;
        uint8_t motor_pin3;
        uint8_t motor_pin4;

        int32_t target = 0;
        int32_t current = 0;

        void stepMotor(int thisStep);

  public:
    // constructors:
    AsyncStepper(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4) {
                                    motor_pin1 = motor_pin_1;
                                    motor_pin2 = motor_pin_2;
                                    motor_pin3 = motor_pin_3;
                                    motor_pin4 = motor_pin_4;

                                    pinMode(motor_pin1, OUTPUT);
                                    pinMode(motor_pin2, OUTPUT);
                                    pinMode(motor_pin3, OUTPUT);
                                    pinMode(motor_pin4, OUTPUT);
                                    digitalWrite(motor_pin1, LOW);
                                    digitalWrite(motor_pin2, LOW);
                                    digitalWrite(motor_pin3, LOW);
                                    digitalWrite(motor_pin4, LOW);

                                 }


    void setTarget(int32_t target) {        
        this->target = target;
    }

    void step(int32_t steps);

    int32_t getCurrent() {
        
        return this->current;
    }

    void update();
};

#endif