#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <NuvIoT.h>
#include <TM1637Display.h>

#define NUMBER_PINS 5
#define NUMBER_LEDS 0

#include "../../common/buttons.h"
#include "../../common/leds.h"
#include "../../common/ws.h"
#include "../../common/simpit.h"

#define AUX_PANEL_SKU "ARM Panel"
#define FIRMWARE_VERSION "1.0.0"

void mqttReceived(String topic, byte *buffer, size_t len)
{
	if(topic == "weapons/mode/aa") {
		digitalWrite(32, LOW);
		digitalWrite(33, HIGH);

	}
	else if(topic == "weapons/mode/ag") {
		digitalWrite(32, HIGH);
		digitalWrite(33, LOW);		
	}
	else if(topic == "weapons/mode/off") {
		digitalWrite(32, LOW);
		digitalWrite(33, LOW);		
	}
	else if(topic == "fire/extinguisher/on") {
		digitalWrite(25, HIGH);
	}
	else if(topic == "fire/extinguisher/off") {
		digitalWrite(25, LOW);
	}

	console.println("mqtt=handltopic; // Topic: " + topic);
}

void setup()
{
	configureConsole();

	pins[0] = 16;
	pins[1] = 17;

	pins[2] = 5;
	pins[3] = 18;
	pins[4] = 19;

	pressTopics[0] = "weapons/arm";
	releaseTopics[0] = "weapons/safe";

	pressTopics[1] = "weapons/mode/ag/press";
	releaseTopics[1] = "weapons/mode/ag/release";

	pressTopics[2] = "weapons/mode/aa/press";
	releaseTopics[2] = "weapons/mode/aa/release";

	pressTopics[3] = "fire/extinguisher/press";
	releaseTopics[3] = "fire/extinguisher/release";

	pressTopics[4] = "stores/emergency/jettison/press";
	releaseTopics[4] = "stores/emergency/jettison/release";

	sysConfig.DeviceId = "f18armpanel";
	initCommonSettings();
	initButtons();

	pinMode(32, OUTPUT);
	pinMode(33, OUTPUT);
	pinMode(25, OUTPUT);

	wifiMQTT.addSubscriptions("#");
	wifiMQTT.setMessageReceivedCallback(mqttReceived);

	state.init(AUX_PANEL_SKU, FIRMWARE_VERSION, "f18armpanel", "f18armpanel", 010);
}

void loop()
{
	simPitLoop(NULL);
}