// #define DEBUG_WEBSOCKETS

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <NuvIoT.h>

#define NUMBER_PINS 6
#define NUMBER_LEDS 3

#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/simpit.h"

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(NUMBER_LEDS, 19); // note: modern WS2812 with letter like WS2812b
NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip2(NUMBER_LEDS, 5); // note: modern WS2812 with letter like WS2812b
NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip3(NUMBER_LEDS, 4); // note: modern WS2812 with letter like WS2812b

#define LEFT_MID_PANEL_SKU "MFD BUttons"
#define FIRMWARE_VERSION "1.0.0"

void setup()
{
	configureConsole();

	strip1.Begin();
	strip2.Begin();
	strip3.Begin();
	setAllLeds(&strip1, 0xff, 0x00, 0x00);
	setAllLeds(&strip2, 0xFF, 0x00, 0x00);
	setAllLeds(&strip3, 0x00, 0xFF, 0x00);

	pins[0] =22;
	pins[1] = (22 << 8) | 21;
	pins[2] = 21;

	pins[3] = 16;
	pins[4] = (16 << 8) | 17;
	pins[5] = 17;

	pressTopics[0] = "mfd/right/off";
	pressTopics[1] = "mfd/right/night";
	pressTopics[2] = "mfd/right/day";

	pressTopics[3] = "mfd/left/off";
	pressTopics[4] = "mfd/left/night";	
	pressTopics[5] = "mfd/left/day";

	for(uint8_t idx = 0; idx < 6; ++idx){
		releaseTopics[idx] = "";
	}

	pinMode(16, INPUT);
	pinMode(17, INPUT);

	sysConfig.DeviceId = "mfdctrl";
	initCommonSettings();
	initButtons();

	state.init(LEFT_MID_PANEL_SKU, FIRMWARE_VERSION, "mfdctrls", "mfdctlrs", 010);

	writeConfigPins();
}

void loop()
{
	simPitLoop(&strip1);
	if (wifiMQTT.isConnected()){
		setAllLeds(&strip1, 0x00, 0xFF, 0x00);
		setAllLeds(&strip2, 0x00, 0xFF, 0x00);
		setAllLeds(&strip3, 0x00, 0xff, 0x00);
	}
	else {
		setAllLeds(&strip1, 0xff, 0x00, 0x00);
		setAllLeds(&strip2, 0xFF, 0x00, 0x00);
		setAllLeds(&strip3, 0xFF, 0x00, 0x00);
	}	
}