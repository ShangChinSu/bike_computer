#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "events.h"


class TouchDriver
{
public:
    TouchDriver();

    bool begin(int sda, int scl, int rst, int irq);
    bool update();

    TouchGesture getGesture() const { return current_gesture; }
    bool isTouched() const { return touched; }
    int16_t getX() const { return touch_x; }
    int16_t getY() const { return touch_y; }

    static const char* gestureToString(TouchGesture gesture);

private:
    int pin_sda;
    int pin_scl;
    int pin_rst;
    int pin_irq;

    TouchGesture current_gesture = TouchGesture::NONE;
    bool touched = false;
    int16_t touch_x = 0;
    int16_t touch_y = 0;

    static constexpr uint8_t CST816_ADDR = 0x15;
};
