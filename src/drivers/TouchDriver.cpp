#include "drivers/TouchDriver.h"


TouchDriver::TouchDriver()
    : pin_sda(4), pin_scl(5), pin_rst(1), pin_irq(0)
{
}


bool TouchDriver::begin(int sda, int scl, int rst, int irq)
{
    pin_sda = sda;
    pin_scl = scl;
    pin_rst = rst;
    pin_irq = irq;

    // Hardware reset sequence for CST816
    if (pin_rst >= 0)
    {
        pinMode(pin_rst, OUTPUT);
        digitalWrite(pin_rst, LOW);
        delay(30);
        digitalWrite(pin_rst, HIGH);
        delay(50);
    }

    if (pin_irq >= 0)
    {
        pinMode(pin_irq, INPUT);
    }

    Wire.begin(pin_sda, pin_scl, 400000);

    // Test communication
    Wire.beginTransmission(CST816_ADDR);
    return (Wire.endTransmission() == 0);
}


bool TouchDriver::update()
{
    current_gesture = TouchGesture::NONE;
    touched = false;

    // Check if touch controller is responding
    Wire.beginTransmission(CST816_ADDR);
    Wire.write(0x01); // Start reading from gesture register
    if (Wire.endTransmission(false) != 0)
    {
        return false;
    }

    // Read 6 bytes: Gesture (0x01), Points (0x02), X_H, X_L, Y_H, Y_L
    if (Wire.requestFrom((int)CST816_ADDR, 6) != 6)
    {
        return false;
    }

    uint8_t raw_gesture = Wire.read();
    uint8_t points      = Wire.read();
    uint8_t x_high      = Wire.read();
    uint8_t x_low       = Wire.read();
    uint8_t y_high      = Wire.read();
    uint8_t y_low       = Wire.read();

    touched = (points > 0);
    if (touched)
    {
        touch_x = ((x_high & 0x0F) << 8) | x_low;
        touch_y = ((y_high & 0x0F) << 8) | y_low;
    }

    // Decode CST816 native gestures
    switch (raw_gesture)
    {
        case 0x01: current_gesture = TouchGesture::SWIPE_DOWN; break;
        case 0x02: current_gesture = TouchGesture::SWIPE_UP; break;
        case 0x03: current_gesture = TouchGesture::SWIPE_LEFT; break;
        case 0x04: current_gesture = TouchGesture::SWIPE_RIGHT; break;
        case 0x05: current_gesture = TouchGesture::TAP; break;
        case 0x0B: current_gesture = TouchGesture::DOUBLE_TAP; break;
        case 0x0C: current_gesture = TouchGesture::LONG_PRESS; break;
        default:   current_gesture = TouchGesture::NONE; break;
    }

    return (current_gesture != TouchGesture::NONE || touched);
}


const char* TouchDriver::gestureToString(TouchGesture gesture)
{
    switch (gesture)
    {
        case TouchGesture::SWIPE_LEFT:   return "SWIPE_LEFT";
        case TouchGesture::SWIPE_RIGHT:  return "SWIPE_RIGHT";
        case TouchGesture::SWIPE_UP:     return "SWIPE_UP";
        case TouchGesture::SWIPE_DOWN:   return "SWIPE_DOWN";
        case TouchGesture::TAP:          return "TAP";
        case TouchGesture::DOUBLE_TAP:   return "DOUBLE_TAP";
        case TouchGesture::LONG_PRESS:   return "LONG_PRESS";
        default:                         return "NONE";
    }
}
