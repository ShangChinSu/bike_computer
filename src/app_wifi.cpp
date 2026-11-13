#include "app_wifi.h"


namespace wifi
{


void initialization()
{
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, PASSWORD);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
	}
}


} // namespace wifi