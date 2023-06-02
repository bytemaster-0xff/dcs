#include <Arduino.h>
#include <NuvIoT.h>

void initCommonSettings() {
	sysConfig.Commissioned = true;
	sysConfig.WiFiEnabled = false;
	sysConfig.WiFiSSID = "";
	sysConfig.WiFiPWD = "";
	sysConfig.SrvrType = "";
	sysConfig.SrvrHostName = "";
	sysConfig.Port = 1883;
	sysConfig.SrvrUID = "";
	sysConfig.SrvrPWD = "";
}