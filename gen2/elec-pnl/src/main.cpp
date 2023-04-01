// #define DEBUG_WEBSOCKETS

#include <Arduino.h>
#include <BleGamepad.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <ESP32Servo.h>

#define USE_SERIAL Serial
#define NUMBER_PINS 6
#define NUMBER_LEDS 48

#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/pids.h"

#include <WebSocketsClient.h>

#include <RotaryEncoder.h>

BleGamepad bleGamepad("F18 - Elec Panel", "Software Logistics", 100);

char path[] = "/api/websocket";
char host[] = "10.1.1.115";

Servo servo1;
Servo servo2;

#define FULL_GREEN
// #define TEST_PATTERN

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(NUMBER_LEDS, 23); // note: modern WS2812 with letter like WS2812b

BleGamepadConfiguration config;

void setup()
{
	USE_SERIAL.begin(115200);
	USE_SERIAL.setDebugOutput(true);
	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	pins[0] = 4;
	pins[1] = 16;
	pins[2] = (16 << 8) | 17;
	pins[3] = 17;
	pins[4] = 5;
	pins[5] = 18;

	servo1.attach(19);
	servo2.attach(21);

	strip1.Begin();
	for (int idx = 0; idx < NUMBER_LEDS; ++idx)
	{
		strip1.SetPixelColor(idx - 1, RgbColor(0, 255, 255)); // red
	}
	strip1.Show();

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

	config.setPid(PID_ELEC);
	config.setButtonCount(NUMBER_PINS);
	config.setWhichAxes(false, false, false, false, false, false, false, false);

	Serial.println("Starting BLE work!");
	bleGamepad.begin(&config);
	bleGamepad.deviceName = "F18 - Elec Panel";
	bleGamepad.deviceManufacturer = "Software Logistics";

	strip1.Begin();

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

int angle = 30;

void loop()
{
	if (bleGamepad.isConnected())
	{
		if (wasConnected == false)
		{
			initButtons(&bleGamepad);
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
		Serial.print(String(digitalRead(pins[idx])));
	}

	Serial.println();
	delay(250);

	servo1.write(angle);
	servo2.write(angle);

	/*	angle++;
		if(angle == 270)
			angle = 30;*/

	//    webSocket.loop();
}