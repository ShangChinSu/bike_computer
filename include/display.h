#pragma once


namespace display
{


// * Pin definitions for ESP32-2424S012C-Y(B)
// ! The numbers are the GPIO numbers, not physical pin headers
#define TFT_SCK    6
#define TFT_MOSI   7
#define TFT_MISO   -1
#define TFT_CS     10
#define TFT_DC     2
#define TFT_RST    -1
#define TFT_BL     3

// * Touch Controller (CST816D) Pins
#define TOUCH_SDA  4
#define TOUCH_SCL  5
#define TOUCH_RST  1
#define TOUCH_INT  0


} // namespace display