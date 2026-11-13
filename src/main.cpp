#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#include <WiFi.h>

#include "system_config.h"
#include "display.h"
#include "app_wifi.h"


Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);

Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 0, true);


void setup()
{
	Serial.begin(115200);

	wifi::initialization();
	// * Config the time
	configTime(system_config::gmtOffset_sec, system_config::daylightOffset_sec, system_config::ntpServer1, system_config::ntpServer2);

	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	if (!gfx->begin())
	{
		Serial.println("螢幕初始化失敗！");
	}

	gfx->fillScreen(0xF800);   // 0xF800 為紅色
	gfx->setTextColor(0xFFFF); // 0xFFFF 為白色
	gfx->setTextSize(3);
	gfx->setCursor(0, 120);
	gfx->println("Sam & Carina");
}


void loop()
{
}