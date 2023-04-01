// #define DEBUG_WEBSOCKETS

#include <Arduino.h>
#include <BleGamepad.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <SparkFun_Alphanumeric_Display.h> //Click here to get the library: http://librarymanager/All#Alphanumeric_Display by SparkFun

#include <Wire.h>

#include <WebSocketsClient.h>

#include <RotaryEncoder.h>

BleGamepad bleGamepad("F18 - Elec Panel", "Software Logistics", 100);

#define USE_SERIAL Serial

int lastStates[16];

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

char path[] = "/api/websocket";
char host[] = "10.1.1.115";

#define NUMBER_PINS  5

uint8_t pins[16];


HT16K33 display5;
HT16K33 display4;
HT16K33 display3;
HT16K33 display2;
HT16K33 display1;

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> strip1(43, 26); // note: modern WS2812 with letter like WS2812b

BleGamepadConfiguration config;

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
	const uint8_t *src = (const uint8_t *)mem;
	USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for (uint32_t i = 0; i < len; i++)
	{
		if (i % cols == 0)
		{
			USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		USE_SERIAL.printf("%02X ", *src);
		src++;
	}
	USE_SERIAL.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

	switch (type)
	{
	case WStype_DISCONNECTED:
		USE_SERIAL.printf("[WSc] Disconnected!\n");
		break;
	case WStype_CONNECTED:
		USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

		// send message to server when Connected
		webSocket.sendTXT("{\"datatype\":\"live_data\",\"data\":{}}");
		break;
	case WStype_TEXT:
		USE_SERIAL.printf("[WSc] get text: %s\n", payload);

		// send message to server
		// webSocket.sendTXT("message here");
		break;
	case WStype_BIN:
		USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
		hexdump(payload, length);

		// send data to server
		// webSocket.sendBIN(payload, length);
		break;
	case WStype_ERROR:
		USE_SERIAL.println("ERROR");
	case WStype_FRAGMENT_TEXT_START:
	case WStype_FRAGMENT_BIN_START:
	case WStype_FRAGMENT:
	case WStype_FRAGMENT_FIN:
		break;
	}
}

void initButtons()
{
	for(int idx = 0; idx < NUMBER_PINS; ++idx) {
		lastStates[idx] = digitalRead(pins[idx]);
		if (lastStates[idx] == HIGH)
			bleGamepad.press(idx + 1);
		else
			bleGamepad.release(idx + 1);
	}
}

void setup()
{
	USE_SERIAL.begin(115200);
	USE_SERIAL.setDebugOutput(true);
	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	pins[0] = 2;
	pins[1] = 4;
	pins[2] = 18;
	pins[3] = 5;
	pins[4] = 25;

	for (uint8_t t = 4; t > 0; t--)
	{
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	for(int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		pinMode(pins[idx], INPUT_PULLDOWN);
	}
	
	// config.setAutoReport(true);
	config.setButtonCount(NUMBER_PINS);

	// config.setAxesMin(0);
	// config.setAxesMax(100);
	config.setWhichAxes(false, false, false, false, false, false, false, false);

	Serial.println("Starting BLE work!");
	bleGamepad.begin(&config);
	// bleGamepad.begin();

	WiFiMulti.addAP("NuvIoT", "TheWolfBytes");

	// WiFi.disconnect();
	while (WiFiMulti.run() != WL_CONNECTED)
	{
		delay(100);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	// Serial.println(WiFiMulti.());

	bleGamepad.deviceName = "F18 - Elec Panel";
	bleGamepad.deviceManufacturer = "Software Logistics";

	strip1.Begin();

	//  webSocket.begin("10.1.1.115", 5010, "/api/websocket");

	// event handler\Heltec-Aaron-Lee\WiFi_Kit_series\issues\62
	//	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	// webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	//	webSocket.setReconnectInterval(5000);

	  
	if(Wire1.begin(19,23,0) == false) {
		Serial.println("Could not init Wire1! Freezing.");
	}
	else {
		Serial.println("Wire 1 Initialized.");
	}

  	if(Wire.begin(21,22,0) == false) {
		Serial.println("Could not init Wire! Freezing.");
	}	
	else {
		Serial.println("Wire Default Initialized.");
	}

  	if (display5.begin(0x70) == false)
  	{
    	Serial.println("Device 5 did not acknowledge! Freezing.");
    	while (1);
  	}

	if (display4.begin(0x71) == false)
  	{
    	Serial.println("Device 4 did not acknowledge! Freezing.");
    	while (1);
  	}

	if (display3.begin(0x72) == false)
  	{
    	Serial.println("Device 3 did not acknowledge! Freezing.");
    	while (1);
  	}

	if (display2.begin(0x73) == false)
  	{
    	Serial.println("Device 2 did not acknowledge! Freezing.");
    	while (1);
  	}

	if (display1.begin(0x70, DEFAULT_NOTHING_ATTACHED, DEFAULT_NOTHING_ATTACHED, DEFAULT_NOTHING_ATTACHED, Wire1) == false)
  	{
    	Serial.println("Device 1 did not acknowledge! Freezing.");
    	while (1);
  	}


  	Serial.println("Display acknowledged.");

	display1.print("One");
	display2.print("Two");
	display3.print("Thre");
	display4.print("Four");
  	display5.print("Five");
}

uint8_t buffer[255];

void checkButton(uint8_t button, int pin)
{
	if (digitalRead(pin) == HIGH)
	{
		//if (lastStates[button-1] == LOW)
		{
			bleGamepad.press(button);
			lastStates[button-1] == HIGH;			
			Serial.println("PRESS " + String(button));
		}		
	}
	else
	{
		//if (lastStates[button-1] == HIGH)
		{
			bleGamepad.release(button);
			lastStates[button-1] = LOW;
			Serial.println("RELEASE " + String(button));
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

		for(int idx = 0; idx < NUMBER_PINS; ++idx)
			checkButton(idx + 1, pins[idx]);

		wasConnected = true;
	}

	for (int idx = 0; idx < 43; ++idx)
	{
		if (idx > 0)
//			strip1.SetPixelColor(idx - 1, RgbColor(0, 0, 0)); // red

		strip1.SetPixelColor(idx, RgbColor(0, 0, 255)); // red
		strip1.Show();
		delay(25);
	}

//	strip1.SetPixelColor(42, RgbColor(0, 0, 0));
	strip1.Show();
	/*pinMode(36, OUTPUT);
	digitalWrite(36, HIGH);
	delay(1500);
	digitalWrite(36, LOW);
	delay(1500);*/

	for(int idx = 0; idx < NUMBER_PINS; ++idx) {
		Serial.println(String(digitalRead(pins[idx])));
	}

	Serial.println();
	//    webSocket.loop();
}