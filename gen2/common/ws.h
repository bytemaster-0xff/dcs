#include <Arduino.h>
#include <NuvIoT.h>

void initCommonSettings() {
	sysConfig.Commissioned = true;
	sysConfig.WiFiEnabled = true;
	sysConfig.WiFiSSID = "NuvIoT";
	sysConfig.WiFiPWD = "TheWolfBytes";
	sysConfig.SrvrType = "mqtt";
	sysConfig.SrvrHostName = "10.1.1.116";
	sysConfig.Port = 1883;
	sysConfig.SrvrUID = "";
	sysConfig.SrvrPWD = "";
}