#include <Arduino.h>
#include <BleGamepad.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <TM1637Display.h>
#include "../../common/pids.h"

BleGamepad bleGamepad("F18 - Elec Panel", "Software Logistics", 100);

#define USE_SERIAL Serial

int lastStates[32];

#define NUMBER_PINS 7

#define DISPLAY_SCK 33
#define DISPLAY_DAT 32

TM1637Display display(DISPLAY_SCK, DISPLAY_DAT);

uint16_t pins[32];

#define NUMBER_LEDS 0

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(NUMBER_LEDS, 23); // note: modern WS2812 with letter like WS2812b

BleGamepadConfiguration config;

void initButtons()
{
	for (int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		lastStates[idx] = digitalRead(pins[idx]);
		if (lastStates[idx] == HIGH)
			bleGamepad.press(idx + 1);
		else
			bleGamepad.release(idx + 1);
	}
}

// #define TEST_PATTERN
#define FULL_GREEN

void setup()
{
	USE_SERIAL.begin(115200);
	USE_SERIAL.setDebugOutput(true);
	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	pins[0] = 4;
	pins[1] = 16;
	pins[2] = 17;

	pins[3] = 5;
	pins[4] = 18;
	pins[5] = 19;
	pins[6] = 21;


	for (uint8_t t = 4; t > 0; t--)
	{
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	for (int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		pinMode(pins[idx], INPUT_PULLDOWN);
	}

	config.setButtonCount(NUMBER_PINS);
	config.setPid(PID_VIEW_CONTROLLER);
	config.setWhichAxes(false, false, false, false, false, false, false, false);
	config.setHatSwitchCount(0);

	bleGamepad.begin(&config);

	bleGamepad.deviceName = "SimPit View Controller";
	bleGamepad.deviceManufacturer = "Software Logistics";

	if(NUMBER_LEDS > 0)
		strip1.Begin();

#ifdef FULL_GREEN
	for (int idx = 0; idx < NUMBER_LEDS; ++idx)
	{
		strip1.SetPixelColor(idx - 1, RgbColor(0, 255, 0)); // red
	}
#endif
}

uint8_t buffer[255];

void checkButton(uint8_t button, int pin)
{
	byte ioPin = abs(pin);

	if (pin > 0xFF)
	{
		uint8_t pin1 = pin & 0xFF;
		uint8_t pin2 = pin >> 8;
		if (digitalRead(pin1) == LOW && digitalRead(pin2) == LOW)
		{
			bleGamepad.press(button);
			Serial.println("PRESS " + String(button) + ", " + String(pin1) + " - " + String(pin2));
		}
		else
		{
			bleGamepad.release(button);
			Serial.println("RELEASE " + String(button) + ", " + String(pin1) + " - " + String(pin2));
		}
	}
	else if (digitalRead(ioPin) == HIGH)
	{
		// if (lastStates[button-1] == LOW)
		{
			if (pin < 0)
			{
				bleGamepad.release(button);
				Serial.println("RELEASE " + String(button));
			}
			else
			{
				bleGamepad.press(button);
				Serial.println("PRESS " + String(button));
			}

			lastStates[button - 1] == HIGH;
		}
	}
	else
	{
		// if (lastStates[button-1] == HIGH)
		{
			if (pin < 0)
			{
				bleGamepad.press(button);
				Serial.println("PRESS " + String(button));
			}
			else
			{
				bleGamepad.release(button);
				Serial.println("RELEASE " + String(button));
			}

			lastStates[button - 1] = LOW;
		}
	}
}

bool wasConnected = false;

void loop()
{
	if (bleGamepad.isConnected())
	{
		if (wasConnected == false)
		{
			initButtons();
		}

		for (int idx = 0; idx < NUMBER_PINS; ++idx)
		{
			checkButton(idx + 1, pins[idx]);
		}

		wasConnected = true;
	}
	else
	{
		wasConnected = false;
	}

#ifdef TEST_PATTERN
	for (int idx = 0; idx < NUMBER_LEDS; ++idx)
	{
		if (idx > 0)
			strip1.SetPixelColor(idx - 1, RgbColor(0, 0, 0)); // red

		strip1.SetPixelColor(idx, RgbColor(0, 0, 255)); // red
		strip1.Show();
		delay(25);
	}

	strip1.SetPixelColor(42, RgbColor(0, 0, 0));
	strip1.Show();
#endif

	for (int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		Serial.print(String(digitalRead(pins[idx])) + ",");
	}

	delay(250);

	Serial.println();
	//    webSocket.loop();
}