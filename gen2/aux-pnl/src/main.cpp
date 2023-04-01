#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <NuvIoT.h>
#include <TM1637Display.h>

#define NUMBER_PINS 14
#define NUMBER_LEDS 30


#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/simpit.h"

#define DISPLAY_SCK 33
#define DISPLAY_DAT 32

TM1637Display sevenSegdisplay(DISPLAY_SCK, DISPLAY_DAT);


#define AUX_PANEL_SKU "SimPit Aux Panel"
#define FIRMWARE_VERSION "1.0.0"


NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(NUMBER_LEDS, 23); // note: modern WS2812 with letter like WS2812b

// #define TEST_PATTERN
#define FULL_GREEN

void setup()
{
	configureConsole();

	strip1.Begin();
	setAllLeds(&strip1, 0xFF, 0xFF, 0xFF);

	pins[0] = 27;
	pins[1] = 2;

	pins[2] = 26;
	pins[3] = (26 << 8) | 16;
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

	pressTopics[0] = "aux/aux8/press";
	pressTopics[1] = "aux/aux7/press";

	pressTopics[2] = "aux/aux1/a";
	pressTopics[3] = "aux/aux1/b";
	pressTopics[4] = "aux/aux1/c";
	
	pressTopics[5] = "aux/aux4/a";
	pressTopics[6] = "aux/aux4/b";
	pressTopics[7] = "aux/aux4/c";

	pressTopics[8] = "aux/aux3/a";
	pressTopics[9] = "aux/aux3/b";
	pressTopics[10] = "aux/aux3/c";

	pressTopics[11] = "aux/aux2/a";
	pressTopics[12] = "aux/aux2/b";
	pressTopics[13] = "aux/aux2/c";

	releaseTopics[0] = "aux/aux8/release";
	releaseTopics[1] = "aux/aux7/release";

	for(uint8_t idx = 2; idx <=13; ++idx) {
		releaseTopics[idx] = "";
	}

	sysConfig.DeviceId = "simpitauxpanel";
	initCommonSettings();
	initButtons();

	state.init(AUX_PANEL_SKU, FIRMWARE_VERSION, "auxpanel", "auxpanel", 010);

	sevenSegdisplay.setBrightness(0x0f);
	uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
	data[0] = sevenSegdisplay.encodeDigit(0);
	data[1] = sevenSegdisplay.encodeDigit(0);
	data[2] = sevenSegdisplay.encodeDigit(0);
	data[3] = sevenSegdisplay.encodeDigit(0);
	sevenSegdisplay.setSegments(data);
}


void loop()
{
	simPitLoop(&strip1);

	uint16_t adc1 = analogRead(35);
	uint16_t adc2 = analogRead(34);

	Serial.println("ADC: " + String(map(adc1, 0, 4095, 32737, 0)) + "," + String(map(adc2, 0, 4095, 32737, 0)));
}