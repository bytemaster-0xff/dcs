#include <Arduino.h>
#include <BleGamepad.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <WebSocketsClient.h>

#include <RotaryEncoder.h>

BleGamepad bleGamepad("F18 - Elec Panel", "Software Logistics", 100);

#define USE_SERIAL Serial

#define NUMBER_PINS 18
#define NUMBER_LEDS 80

#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/pids.h"

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


//#define TEST_PATTERN
#define FULL_GREEN

void setup()
{
	USE_SERIAL.begin(115200);
	USE_SERIAL.setDebugOutput(true);
	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	strip1.Begin();
	for (int idx = 0; idx < NUMBER_LEDS; ++idx)
	{
		strip1.SetPixelColor(idx - 1, RgbColor(0, 255, 255)); // red
	}
	strip1.Show();


	pins[0] = 34;
	pins[1] = (35 << 8) | 34;
	pins[2] = 35;
	
	pins[3] = 32;
	pins[4] = (32 << 8) | 33;
	pins[5] = 33;

	pins[6] = -25;
	pins[7] = 25;
	pins[8] = 26;
	pins[9] = 27;

	pins[10] = 15;
	pins[11] = (15 << 8) | 2;
	pins[12] = 2;	
	
	pins[13] = 4;
	pins[14] = 16;

	pins[15] = 17;
	pins[16] = 5;
	pins[17] = 18;
	pins[18] = 19;

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

	
	config.setPid(PID_LEFT_MID_PANEL);
	config.setButtonCount(NUMBER_PINS);
	config.setWhichAxes(false, false, false, false, false, false, false, false);

	bleGamepad.begin(&config);

	bleGamepad.deviceName = "F18 - Left Mid Panel";
	bleGamepad.deviceManufacturer = "Software Logistics";

#ifdef FULL_GREEN
	for (int idx = 0; idx < NUMBER_LEDS; ++idx)
	{
		strip1.SetPixelColor(idx - 1, RgbColor(0, 255, 0)); // red
	}

	strip1.Show();
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
	else
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

	Serial.println();

	delay(250);

	
	//    webSocket.loop();
}