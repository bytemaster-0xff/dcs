#include <Arduino.h>
#include <NuvIoT.h>

void cmdCallback(String cmd)
{
	cmd.trim();
	if (cmd.length() > 0)
	{
		console.println("ECHO => " + cmd);

		if (cmd == "PING")
			console.println("PONG");
		else if (cmd == "WHOIS")
			console.println("IAM=" + sysConfig.DeviceId);
		else if (cmd == "SENDALL")
			sendCurrentPinState();
	}
}

void initCommonSettings()
{
	sysConfig.Commissioned = true;
	sysConfig.WiFiEnabled = false;
	sysConfig.WiFiSSID = "";
	sysConfig.WiFiPWD = "";
	sysConfig.SrvrType = "";
	sysConfig.SrvrHostName = "";
	sysConfig.Port = 1883;
	sysConfig.SrvrUID = "";
	sysConfig.SrvrPWD = "";

	console.registerCallback(cmdCallback);
}