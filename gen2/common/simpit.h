#ifndef SIMPIT_H
#define SIMPIT_H

#include <Arduino.h>
#include <NuvIoT.h>
#include "leds.h"
#include "buttons.h"

bool flashState = false;
uint32_t nextFlash;

void simPitLoop(NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> *strip)
{
	commonLoop();

	if (strip != NULL)
	{
		nextFlash = millis() + 500;
		if (nextFlash > millis())
		{
			nextFlash = millis() + 500;
			flashState = !flashState;
		}

		if (wifiMgr.isConnected())
		{
			if (wifiMQTT.isConnected())
			{
				setAllLeds(strip, 0x00, 0x7F, 0x00);
			}
			else
			{
				if (flashState)
					setAllLeds(strip, 0x00, 0x00, 0x7F);
				else
					setAllLeds(strip, 0x00, 0x00, 0x00);
			}
		}
		else
		{
			if (flashState)
				setAllLeds(strip, 0x7F, 0x00, 0x00);
			else
				setAllLeds(strip, 0x00, 0x00, 0x00);
		}
	}

	for (int idx = 0; idx < NUMBER_PINS; ++idx)
	{
		checkButton(idx, pins[idx]);
	}
}

#endif