#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NuvIoT.h>

#ifndef NUMBER_LEDS
    #define NUMBER_LEDS 0
#endif    

void setAllLeds(NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> *strip, uint8_t red, uint8_t green, uint8_t blue){
    if(strip == NULL)
		return;
	
    for (int idx = 0; idx < NUMBER_LEDS; ++idx)
	{
		strip->SetPixelColor(idx - 1, RgbColor(red, green, blue)); // red
	}
	strip->Show(); 
}

void testPattern(NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1X8Ws2812xMethod> *strip, uint8_t red, uint8_t green, uint8_t blue) {
   	if(strip == NULL)
		return;

	for (int idx = 0; idx < NUMBER_LEDS; ++idx)
	{
		if (idx > 0)
			strip->SetPixelColor(idx - 1, RgbColor(0, 0, 0)); // red

		strip->SetPixelColor(idx, RgbColor(0, 0, 255)); // red
		strip->Show();
		delay(25);
	}

	strip->SetPixelColor(NUMBER_LEDS - 1, RgbColor(0, 0, 0));
	strip->Show();
}

#endif