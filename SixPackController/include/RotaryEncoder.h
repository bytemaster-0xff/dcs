#ifndef __ROTARY_ENCODER_H
#define __ROTARY_ENCODER_H

#include <Arduino.h>

class RotaryEncoder {
    private:
        uint8_t m_pinA;
        uint8_t m_pinB;
        int32_t m_delta;
        char m_lastState;
        uint8_t m_stepsPerDetent = 4;

    public:
        RotaryEncoder(uint8_t pinA, uint8_t pinB){
            m_pinA = pinA;
            m_pinB = pinB;
            pinMode(m_pinA, INPUT_PULLUP);
            pinMode(m_pinB, INPUT_PULLUP);
        };

	    char readState() {
				return (digitalRead(m_pinA) << 1) | digitalRead(m_pinB);
		}

        void pollInput() {
            char state = readState();
            if(state != m_lastState){    
                switch(m_lastState) {
                    case 0:
                        if (state == 2) m_delta--;
                        if (state == 1) m_delta++;
                        break;
                    case 1:
                        if (state == 0) m_delta--;
                        if (state == 3) m_delta++;
                        break;
                    case 2:
                        if (state == 3) m_delta--;
                        if (state == 0) m_delta++;
                        break;
                    case 3:
                        if (state == 1) m_delta--;
                        if (state == 2) m_delta++;
                        break;
                }
                m_lastState = state;
                }


        }

        int8_t loop() {
            int32_t lastDelta = m_delta;
            pollInput();
            int32_t delta = m_delta - lastDelta;
           
            if (m_delta >= m_stepsPerDetent) {
                    m_delta -= m_stepsPerDetent;
                    return 1;
            }
            if (m_delta <= -m_stepsPerDetent) {
                    m_delta += m_stepsPerDetent;
                    return -1;
            }

            return 0;
        }
};

#endif