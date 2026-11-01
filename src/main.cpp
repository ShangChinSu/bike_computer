#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ! Pin define for the board version with postfix Y(B)
#define TFT_SCK    6
#define TFT_MOSI   7
#define TFT_MISO   -1
#define TFT_CS     10
#define TFT_DC     2
#define TFT_RST    -1
#define TFT_BL     3

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);

Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 0, true);

void setup()
{
	Serial.begin(115200);

	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	if (!gfx->begin())
	{
		Serial.println("螢幕初始化失敗！");
	}

	gfx->fillScreen(0xF800);   // 0xF800 為紅色
	gfx->setTextColor(0xFFFF); // 0xFFFF 為白色
	gfx->setTextSize(3);
	gfx->setCursor(40, 110);
	gfx->println("SUCCESS");
}

void loop()
{
}