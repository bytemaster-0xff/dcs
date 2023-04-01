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

#define NUMBER_PINS 14

#define DISPLAY_SCK 33
#define DISPLAY_DAT 32

TM1637Display display(DISPLAY_SCK, DISPLAY_DAT);

uint16_t pins[32];

#define NUMBER_LEDS 30

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

	pins[0] = 15;
	pins[1] = 2;

	pins[2] = 4;
	pins[3] = (4 << 8) | 16;
	pins[4] = 16;

	pins[5] = 17;
	pins[6] = (17 << 8) | 5;
	pins[7] = 5;

	pins[8] = 18;
	pins[9] = (18 << 8) | 19;
	pins[10] = 19;

	pins[11] = 21;
	pins[12] = (21 << 8) | 22;
	pins[13] = 22;

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

	// config.setAutoReport(true);
	config.setButtonCount(NUMBER_PINS);

	// config.setAxesMin(0);
	// config.setAxesMax(100);
	config.setWhichAxes(false, false, false, false, false, false, true, true);

	config.setPid(PID_AUX);
	bleGamepad.begin(&config);

	bleGamepad.deviceName = "F18 - Aux Panel";
	bleGamepad.deviceManufacturer = "Software Logistics";

	strip1.Begin();

#ifdef FULL_GREEN
	for (int idx = 0; idx < NUMBER_LEDS; ++idx)
	{
		strip1.SetPixelColor(idx - 1, RgbColor(0, 255, 0)); // red
	}
	
	strip1.Show();
#endif

	display.setBrightness(0x0f);
	uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
	data[0] = display.encodeDigit(0);
	data[1] = display.encodeDigit(0);
	data[2] = display.encodeDigit(0);
	data[3] = display.encodeDigit(0);
	display.setSegments(data);
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

	/*pinMode(36, OUTPUT);
	digitalWrite(36, HIGH);
	delay(1500);
	digitalWrite(36, LOW);
	delay(1500);*/
	uint16_t adc1 = analogRead(35);
	uint16_t adc2 = analogRead(34);

	bleGamepad.setSlider1(map(adc1, 0, 4095, 32737, 0));	
	bleGamepad.setSlider2(map(adc2, 0, 4095, 32737, 0));

	Serial.println("ADC: " + String(map(adc1, 0, 4095, 32737, 0)) + "," + String(map(adc2, 0, 4095, 32737, 0)));

	for (int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		Serial.print(String(digitalRead(pins[idx])) + ",");
	}

	delay(250);

	Serial.println();
	//    webSocket.loop();
}