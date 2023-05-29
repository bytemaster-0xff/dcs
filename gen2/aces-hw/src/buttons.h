#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>


#ifndef NUMBER_PINS
    #define NUMBER_PINS 0
#endif

#define PIN_DETAIL

uint16_t inputs[32];
uint8_t lastStates[32];

String pressTopics[32];
String releaseTopics[32];

extern void mqttPublish(String value);

void sendPress(uint8_t button)
{
	if (pressTopics[button] != NULL && pressTopics[button].length() > 0)
	{
		mqttPublish(pressTopics[button]);
	}
}

void sendRelease(uint8_t button)
{
	if (releaseTopics[button] != NULL && releaseTopics[button].length() > 0)
	{
		mqttPublish(releaseTopics[button]);
	}
}


void checkButton(uint8_t button, int pin, bool initial = false)
{
	byte ioPin = abs(pin);

	if (pin > 0xFF)
	{
		uint8_t pin1 = pin & 0xFF;
		uint8_t pin2 = pin >> 8;

		uint8_t pin1Value = digitalRead(pin1);
		uint8_t pin2Value = digitalRead(pin2);

		if (pin1Value == LOW && pin2Value == LOW && (lastStates[button] == LOW || initial))
		{
#ifdef PIN_DETAIL
			Serial.println("PRESS " + String(button) + ", " + String(pin1) + " - " + String(pin2));
#endif
			lastStates[button] = HIGH;
			sendPress(button);
		}
		else if ((pin1Value == HIGH || pin2Value == HIGH) && (lastStates[button] == HIGH || initial))
		{
#ifdef PIN_DETAIL
			Serial.println("RELEASE " + String(button) + ", " + String(pin1) + " - " + String(pin2));
#endif
			lastStates[button] = LOW;
			sendRelease(button);
		}
	}
	else
	{
		uint8_t pinValue = digitalRead(ioPin);
		if (pinValue == HIGH && (lastStates[button] == LOW || initial))
		{
#ifdef PIN_DETAIL
			Serial.println("PRESS " + String(button) + " " + String(ioPin));
#endif
			lastStates[button] = HIGH;
			sendPress(button);
		}
		else if (pinValue == LOW && (lastStates[button] == HIGH || initial))
		{
#ifdef PIN_DETAIL
			Serial.println("RELEASE " + String(button) + " " + String(ioPin));
#endif
			if(lastStates[button] == HIGH)
			{
				sendRelease(button);
				lastStates[button] = LOW;
			}
		}

		lastStates[button] = pinValue;
	}
}

void checkButtons() {
	for (int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		checkButton(idx, inputs[idx], false);
	}	
}

void sendCurrentPinState() {
	for (int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		checkButton(idx, inputs[idx], true);
	}	
}

void initButtons(){
	for (int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		pinMode(inputs[idx], INPUT);
	}

	sendCurrentPinState();
}

#endif