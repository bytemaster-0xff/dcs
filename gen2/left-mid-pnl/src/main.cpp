#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <NuvIoT.h>

#define USE_SERIAL Serial

#define NUMBER_PINS 18
#define NUMBER_LEDS 80

#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/simpit.h"

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(NUMBER_LEDS, 23); // note: modern WS2812 with letter like WS2812b

#define LEFT_MID_PANEL_SKU "Left Mid Panel"
#define FIRMWARE_VERSION "1.2.0"

void setup()
{
	configureConsole();	

	strip1.Begin();
	setAllLeds(&strip1, 0x1f,0,0);

	pins[0] = 34;
	pins[1] = (35 << 8) | 34;
	pins[2] = 35;
	
	pins[3] = 32;
	pins[4] = (32 << 8) | 33;
	pins[5] = 33;

	pins[6] = 25;
	pins[7] = 26;
	pins[8] = 27;

	pins[9] = 14;
	pins[10] = (14 << 8) | 2;
	pins[11] = 2;	
	
	pins[12] = 13;
	pins[13] = 16;

	pins[14] = 17;
	pins[15] = 5;
	pins[16] = 18;
	pins[17] = 19;

	pressTopics[0] = "park/rotate";
	pressTopics[1] = "park/pull";
	pressTopics[2] = "park/push";

	pressTopics[3] = "gear/rotate";
	pressTopics[4] = "gear/pull";	
	pressTopics[5] = "gear/push";

	pressTopics[6] = "launchbar/retract";
	pressTopics[7] = "lights/landing/on";
	pressTopics[8] = "hookbypass/field";

	pressTopics[9] = "flaps/full";
	pressTopics[10] = "flaps/half";
	pressTopics[11] = "flaps/auto";

	pressTopics[12] = "antiskid/on";
	pressTopics[13] = "jettison/exec";

	pressTopics[14] = "jettison/racklcr";
	pressTopics[15] = "jettison/rfusmsl";
	pressTopics[16] = "jettison/lfusmsl";
	pressTopics[17] = "jettison/stores";
	
	for(uint8_t idx = 0; idx <= 5; ++idx){
		releaseTopics[idx] = "";
	}

	releaseTopics[6] = "launchbar/extend";
	releaseTopics[7] = "lights/landing/off";
	releaseTopics[8] = "hookbypass/carrier";

	releaseTopics[9] = "";
	releaseTopics[10] = "";
	releaseTopics[11] = "";

	releaseTopics[12] = "antiskid/off";
	releaseTopics[13] = "";

	releaseTopics[14] = "";
	releaseTopics[15] = "";
	releaseTopics[16] = "";
	releaseTopics[17] = "";


	sysConfig.DeviceId = "leftmidpanel";
	initCommonSettings();
	initButtons();

	state.init(LEFT_MID_PANEL_SKU, FIRMWARE_VERSION, "leftmidpanel", "leftmidpanel", 010);
}

void loop()
{
	simPitLoop(&strip1);
}