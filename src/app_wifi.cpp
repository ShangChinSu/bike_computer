#include "app_wifi.h"


namespace wifi
{


void initialization()
{
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, PASSWORD);
	Serial.print("[WiFi] Connecting to ");
	Serial.print(SSID);

	uint32_t start = millis();
	// Wait up to 6 seconds for connection, then continue offline
	while (WiFi.status() != WL_CONNECTED && (millis() - start < 6000))
	{
		delay(500);
		Serial.print(".");
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		Serial.println("\n[WiFi] Connected! IP: " + WiFi.localIP().toString());
	}
	else
	{
		Serial.println("\n[WiFi] Connect timeout. Running offline.");
	}
}


} // namespace wifi