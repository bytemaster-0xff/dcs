// #define DEBUG_WEBSOCKETS

#include <Arduino.h>
#include <BleGamepad.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <WebSocketsClient.h>

#include <RotaryEncoder.h>

BleGamepad bleGamepad("SL - SimPit - MFD Controls", "Software Logistics", 100);

#define USE_SERIAL Serial

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

char path[] = "/api/websocket";
char host[] = "10.1.1.115";

RotaryEncoder encoder1(21, 19, RotaryEncoder::LatchMode::TWO03);
RotaryEncoder encoder2(23, 5, RotaryEncoder::LatchMode::TWO03);
RotaryEncoder encoder3(17, 4, RotaryEncoder::LatchMode::TWO03);

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

void setup()
{
	USE_SERIAL.begin(115200);
	USE_SERIAL.setDebugOutput(true);
	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for (uint8_t t = 4; t > 0; t--)
	{
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	pinMode(22, INPUT_PULLDOWN);
	pinMode(18, INPUT_PULLDOWN);
	pinMode(16, INPUT_PULLDOWN);

	// config.setAutoReport(true);
	config.setButtonCount(3);
	//config.setAxesMin(0);
	//config.setAxesMax(100);
	config.setWhichAxes(true, true, true, false, false, false, false, false);

	Serial.println("Starting BLE work!");
	 bleGamepad.begin(&config);
	//bleGamepad.begin();

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

	bleGamepad.deviceName = "NuvIoT - MDF Controller";
	bleGamepad.deviceManufacturer = "Software Logistics";

	//  webSocket.begin("10.1.1.115", 5010, "/api/websocket");

	// event handler
	//	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	// webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	//	webSocket.setReconnectInterval(5000);
}

uint8_t buffer[1024];

int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
uint8_t wasPressed1;
uint8_t wasPressed2;
uint8_t wasPressed3;
uint8_t wasConnected;

void loop()
{
	if (bleGamepad.isConnected())
	{
		if (wasConnected == false)
		{
			if (digitalRead(22) == LOW)
				bleGamepad.press(BUTTON_1);
			else
				bleGamepad.release(BUTTON_1);

			if (digitalRead(18) == LOW)
				bleGamepad.press(BUTTON_2);
			else
				bleGamepad.release(BUTTON_2);

			if (digitalRead(16) == LOW)
				bleGamepad.press(BUTTON_3);
			else
				bleGamepad.release(BUTTON_3);

			bleGamepad.setAxes(pos1, pos2, pos3, 0, 0, 0, 0, 0);
		}

		if (digitalRead(22) == LOW && !wasPressed1)
		{
			bleGamepad.press(BUTTON_1);
			wasPressed1 = true;
		}
		if (digitalRead(18) == LOW && !wasPressed2)
		{
			bleGamepad.press(BUTTON_2);
			wasPressed2 = true;
		}
		if (digitalRead(16) == LOW && !wasPressed3)
		{
			bleGamepad.press(BUTTON_3);
			wasPressed3 = true;
		}

		if (digitalRead(22) == HIGH && wasPressed1)
		{
			bleGamepad.release(BUTTON_1);
			wasPressed1 = false;
		}
		if (digitalRead(18) == HIGH && wasPressed2)
		{
			bleGamepad.release(BUTTON_2);
			wasPressed2 = false;
		}
		if (digitalRead(16) == HIGH && wasPressed3)
		{
			bleGamepad.release(BUTTON_3);
			wasPressed3 = false;
		}
		wasConnected = true;
	}	

	encoder1.tick();
	encoder2.tick();
	encoder3.tick();

	int newPos = encoder1.getPosition();
	if (pos1 != newPos)
	{
		pos1 = newPos;
		int adjustedValue = map(pos1, 0, 100,-32737,32737);
		Serial.print(" pos:");
		Serial.print(adjustedValue);
		Serial.print(" new:");
		Serial.print(newPos);
		Serial.print(" dir:");
		Serial.println((int)(encoder1.getDirection()));
		bleGamepad.setX(adjustedValue);
	} 

	newPos = encoder2.getPosition();
	if (pos2 != newPos)
	{
		pos2 = newPos;
		int adjustedValue = map(pos2, -100, 100,0,32737);
		Serial.print(" pos2:");
		Serial.print(adjustedValue);
		Serial.print(" new:");
		Serial.print(newPos);
		Serial.print(" dir:");
		Serial.println((int)(encoder2.getDirection()));
		bleGamepad.setY(adjustedValue);
	} 

	newPos = encoder3.getPosition();
	if (pos3 != newPos)
	{
		pos3 = newPos;
		int adjustedValue = map(pos3, -100, 100,0,32737);
		Serial.print(" pos3:");
		Serial.print(adjustedValue);
		Serial.print(" new:");
		Serial.print(newPos);
		Serial.print(" dir:");
		Serial.println((int)(encoder3.getDirection()));
		bleGamepad.setZ(adjustedValue);
	} 

	if (digitalRead(22) == LOW)
		Serial.println(" SW 1 DWN ");
	if (digitalRead(18) == LOW)
		Serial.println(" SW 2 DWN ");
	if (digitalRead(16) == LOW)
		Serial.println(" SW 3 DWN ");

	//    webSocket.loop();
}