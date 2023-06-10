#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <NuvIoT.h>
#include <RotaryEncoder.h>

#define NUMBER_PINS 9
#define NUMBER_LEDS 51

#define FULL_GREEN
// #define TEST_PATTERN

#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/simpit.h"

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(NUMBER_LEDS, 23); // note: modern WS2812 with letter like WS2812b

RotaryEncoder encoder(34, 35, RotaryEncoder::LatchMode::TWO03);

#define ELEC_PANEL_SKU "F18 FCS APU OXY ENG Panel"
#define FIRMWARE_VERSION "1.0.3"

static int pos = 0;

void fcsCallback(String cmd)
{
	cmd.trim();
	if (cmd.length() > 0)
	{
		if (cmd == "fcs/apu/running")
		{
			digitalWrite(17, HIGH);
		}
		else if (cmd == "fcs/apu/shutdown")
			{
				digitalWrite(17, LOW);
			}
		else 
			cmdCallback(cmd);
	}
}

void setup()
{
	configureConsole();

	strip1.Begin();
	setAllLeds(&strip1, 0x1f, 0x9, 0x0u);

	pins[0] = 13;
	pins[1] = 14;

	pins[2] = 27;
	pins[3] = (27 << 8) | 26;
	pins[4] = 26;

	pins[5] = 25;
	pins[6] = 33;
	pins[7] = 32;
	pins[8] = 16;

	pressTopics[0] = F("oxy/flow/on");
	releaseTopics[0] = F("oxy/flow/off");

	pressTopics[1] = F("apu/on");
	releaseTopics[1] = F("apu/off");

	pressTopics[2] = F("motor/crank/left");
	releaseTopics[2] = F("");
	pressTopics[3] = F("motor/crank/off");
	releaseTopics[3] = F("");
	pressTopics[4] = F("motor/crank/right");
	releaseTopics[4] = F("");

	pressTopics[5] = F("oxy/obogs/off");
	releaseTopics[5] = F("oxy/obogs/on");

	pressTopics[6] = F("fcs/bittest/on");
	releaseTopics[6] = F("fcs/bittest/off");

	pressTopics[7] = F("");
	releaseTopics[7] = F("rudder/trim/reset/press");

	pressTopics[8] = F("fcs/reset/press");
	releaseTopics[8] = F("");

	for (int idx = 9; idx < 18; ++idx)
		releaseTopics[idx] = F("");

	sysConfig.DeviceId = "fcsengoxyapu";
	initCommonSettings();
	console.registerCallback(fcsCallback);
	initButtons();

	pinMode(17, OUTPUT);
	digitalWrite(17, LOW);

	state.init(ELEC_PANEL_SKU, FIRMWARE_VERSION, "fcsengoxyapu", "fcsengoxyapu", 010);
}

void loop()
{
	simPitLoop(&strip1);

	encoder.tick();

	int newPos = encoder.getPosition();
	if (pos != newPos)
	{
		pos = newPos;
		console.println("rudder/trim/" + String(pos));
	} // if
}