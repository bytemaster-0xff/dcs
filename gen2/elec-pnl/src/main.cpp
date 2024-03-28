#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <NuvIoT.h>

#define NUMBER_PINS 6
#define NUMBER_LEDS 48

#define FULL_GREEN
// #define TEST_PATTERN

#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/simpit.h"


Servo servo1;
Servo servo2;

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(NUMBER_LEDS, 23); // note: modern WS2812 with letter like WS2812b

#define ELEC_PANEL_SKU "F18 Electric Panel"
#define FIRMWARE_VERSION "1.0.0"

void setup()
{
	configureConsole();

	strip1.Begin();
	setAllLeds(&strip1, 0x1f, 0x9, 0x0u);

	pins[0] = 14;
	pins[1] = 16;
	pins[2] = (16 << 8) | 17;
	pins[3] = 17;
	pins[4] = 5;
	pins[5] = 18;

	pressTopics[0] = F("elec/gen/left/on");
	releaseTopics[0] = F("elec/gen/left/off");

	pressTopics[1] = F("elec/batt/oride");
	pressTopics[2] = F("elec/batt/off");
	pressTopics[3] = F("elec/batt/on");

	releaseTopics[1] = F("");
	releaseTopics[2] = F("");
	releaseTopics[3] = F("");

	pressTopics[4] = F("elec/gen/right/on");
	releaseTopics[4] = F("elec/gen/right/off");

	pressTopics[5] = F("elec/avcool/emerg");
	releaseTopics[5] = F("elec/avcool/norm");

	sysConfig.DeviceId = "f18elecpanel";
	initButtons();

	state.init(ELEC_PANEL_SKU, FIRMWARE_VERSION, "f18elecpnl", "f18elecpnl", 010);

	servo1.attach(19);
	servo2.attach(21);

	initCommonSettings();
}

int angle = 30;

void loop()
{	
	simPitLoop(&strip1);
	servo1.write(angle);
	servo2.write(angle);
}