#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <NuvIoT.h>

#define NUMBER_PINS 5
#define NUMBER_LEDS 0

#include "../../common/ws.h"

#define ACES_PANEL_SKU "ACES Panel"
#define FIRMWARE_VERSION "0.7.0"

HardwareSerial hw_serial ( 1 );

void mqttReceived(String topic, byte *buffer, size_t len){
	size_t written = hw_serial.print(topic + '\r');
}

void setup()
{
	configureConsole();

	hw_serial.begin(115200, SERIAL_8N1, 26, 25);

	console.println("app startup.");

	initCommonSettings();
	sysConfig.DeviceId = "acespanel";

	wifiMQTT.addSubscriptions("#");
	wifiMQTT.setMessageReceivedCallback(mqttReceived);

	state.init(ACES_PANEL_SKU, FIRMWARE_VERSION, "acescommo", "acescommo", 010);
	welcome(ACES_PANEL_SKU, FIRMWARE_VERSION);
}

void loop()
{
	if(hw_serial.available() > 0){
		String cmd = hw_serial.readStringUntil('\r');
		console.println("FROM SERIAL" + cmd);
		mqttPublish(cmd);
	}
	commonLoop();
}