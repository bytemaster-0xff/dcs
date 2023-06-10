#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <NuvIoT.h>

#define NUMBER_PINS 5
#define NUMBER_LEDS 0

HardwareSerial hw_serial ( 1 );

void sendCurrentPinState(){
	hw_serial.println("SENDALL");
}

#include "../../common/ws.h"

#define ACES_PANEL_SKU "ACES Panel"
#define FIRMWARE_VERSION "0.7.0"

void setup()
{
	configureConsole();

	hw_serial.begin(115200, SERIAL_8N1, 26, 25);

	initCommonSettings();
	sysConfig.DeviceId = "acespanel";

	state.init(ACES_PANEL_SKU, FIRMWARE_VERSION, "acescommo", "acescommo", 010);
	welcome(ACES_PANEL_SKU, FIRMWARE_VERSION);
}

void loop()
{
	if(hw_serial.available() > 0){
		String cmd = hw_serial.readStringUntil('\r');
		console.print("ACTION=" + cmd + '\n');
	}
	commonLoop();
	console.loop();
}