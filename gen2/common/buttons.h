#include <Arduino.h>

uint16_t pins[32];

#ifndef NUMBER_PINS
    #define NUMBER_PINS 0
#endif

int lastStates[32];

void initButtons(BleGamepad *gamePad){
	for(int idx = 0; idx < NUMBER_PINS; ++idx) {
		lastStates[idx] = digitalRead(pins[idx]);
		if (lastStates[idx] == HIGH)
			gamePad->press(idx + 1);
		else
			gamePad->release(idx + 1);
	}
}

