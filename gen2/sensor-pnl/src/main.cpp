#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <NuvIoT.h>

#define NUMBER_PINS 18
#define NUMBER_LEDS 48

#define FULL_GREEN
// #define TEST_PATTERN

#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/simpit.h"


NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(NUMBER_LEDS, 23); // note: modern WS2812 with letter like WS2812b

#define ELEC_PANEL_SKU "F18 Sensor Panel"
#define FIRMWARE_VERSION "1.0.0"

void setup()
{
	configureConsole();

	strip1.Begin();
	setAllLeds(&strip1, 0x1f, 0x9, 0x0u);

	pins[0] = 13;
	pins[1] = 14;
	pins[2] = 27;
	pins[3] = 26;
	pins[4] = 25;
	pins[5] = 33;
	pins[6] = 32;
	pins[7] = 35;

	pins[8] = 34;

	pins[9] = 21;
	pins[10] = (21 << 8) | 2;
	pins[11] = 2;

	pins[12] = 16;
	pins[13] = (16 << 8) | 17;
	pins[14] = 17;

	pins[15] = 18;
	pins[16] = (18 << 8) | 19;
	pins[17] = 19;

	pressTopics[0] = F("sensor/ins/gnd");
	pressTopics[1] = F("sensor/ins/nav");
	pressTopics[2] = F("sensor/ins/fa");
	pressTopics[3] = F("sensor/ins/gyro");
	pressTopics[4] = F("sensor/ins/gb");
	pressTopics[5] = F("sensor/ins/test");
	pressTopics[6] = F("sensor/ins/off");
	pressTopics[7] = F("sensor/ins/cv");

	for(int idx = 0; idx < 8; ++idx)
		releaseTopics[idx] = F("");

	pressTopics[8] = F("sensor/lstnflr/on");
	releaseTopics[8] = F("sensor/lstnflr/off");

	pressTopics[9] = F("sensor/ltdr/arm");
	pressTopics[10] = F("sensor/ltdr/safe");
	pressTopics[11] = F("sensor/ltdr/aft");

	pressTopics[12] = F("sensor/flir/on");
	pressTopics[13] = F("sensor/flir/stby");
	pressTopics[14] = F("sensor/flir/off");

	pressTopics[15] = F("sensor/radar/on");
	pressTopics[16] = F("sensor/radar/stby");
	pressTopics[17] = F("sensor/radar/off");

	for(int idx = 9; idx < 18; ++idx)
		releaseTopics[idx] = F("");

	sysConfig.DeviceId = "f18sensorpanel";
	initCommonSettings();
	initButtons();

	state.init(ELEC_PANEL_SKU, FIRMWARE_VERSION, "f18sensorpanel", "f18sensorpanel", 010);
}


void loop()
{	
	simPitLoop(&strip1);	
}