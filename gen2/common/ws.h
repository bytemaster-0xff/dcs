#include <WebSocketsClient.h>
#include <Arduino.h>

WebSocketsClient webSocket;

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


//  webSocket.begin("10.1.1.115", 5010, "/api/websocket");

	// event handler
	//	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	// webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	//	webSocket.setReconnectInterval(5000);