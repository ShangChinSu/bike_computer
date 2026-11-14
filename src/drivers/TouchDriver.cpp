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
        pinMode(pin_irq, INPUT_PULLUP);
    }

    Wire.begin(pin_sda, pin_scl, 400000);

    // Test communication
    Wire.beginTransmission(CST816_ADDR);
    return (Wire.endTransmission() == 0);
}


bool TouchDriver::update()
{
    current_gesture = TouchGesture::NONE;
    uint32_t now = millis();

    // 1. Rate-limit I2C polling to every 15ms
    if (now - last_poll_time < 15)
    {
        return false;
    }
    last_poll_time = now;

    // 2. Read 6 bytes starting from register 0x01
    bool packet_valid = false;
    uint8_t raw_gesture = 0;
    uint8_t points = 0;
    uint8_t x_high = 0, x_low = 0, y_high = 0, y_low = 0;

    Wire.beginTransmission(CST816_ADDR);
    Wire.write(0x01);
    if (Wire.endTransmission(false) == 0)
    {
        if (Wire.requestFrom((int)CST816_ADDR, 6) == 6)
        {
            raw_gesture = Wire.read();
            points      = Wire.read();
            x_high      = Wire.read();
            x_low       = Wire.read();
            y_high      = Wire.read();
            y_low       = Wire.read();
            packet_valid = true;
        }
    }

    // Event type in upper 2 bits of byte 2: 0=Down, 1=Up/Release, 2=Contact/Move
    uint8_t event_type = (x_high >> 6) & 0x03;
    bool active_contact = packet_valid && (points > 0) && (event_type != 1);

    if (active_contact)
    {
        // User is actively touching the glass
        touch_x = ((x_high & 0x0F) << 8) | x_low;
        touch_y = ((y_high & 0x0F) << 8) | y_low;
        last_touch_seen_ms = now;

        if (!is_touching)
        {
            // Initial contact
            is_touching = true;
            start_x = touch_x;
            start_y = touch_y;
            touch_start_time = now;
            pending_gesture = TouchGesture::NONE;
        }

        // Latch any hardware gesture detected during the continuous swipe
        if (pending_gesture == TouchGesture::NONE && raw_gesture != 0)
        {
            switch (raw_gesture)
            {
                case 0x01: pending_gesture = TouchGesture::SWIPE_DOWN; break;
                case 0x02: pending_gesture = TouchGesture::SWIPE_UP; break;
                case 0x03: pending_gesture = TouchGesture::SWIPE_LEFT; break;
                case 0x04: pending_gesture = TouchGesture::SWIPE_RIGHT; break;
                case 0x05: pending_gesture = TouchGesture::TAP; break;
                case 0x0B: pending_gesture = TouchGesture::DOUBLE_TAP; break;
                case 0x0C: pending_gesture = TouchGesture::LONG_PRESS; break;
                default:   break;
            }
        }

        // NEVER emit an event while the finger is actively in contact!
        return false;
    }
    else
    {
        // No active contact.
        // The finger has truly lifted ONLY IF:
        // 1. We were touching, AND
        // 2. Either an explicit hardware UP packet arrived (event_type == 1) OR
        //    at least 80ms of silence has passed since the last touch packet.
        if (is_touching && ((event_type == 1) || (now - last_touch_seen_ms >= 80)))
        {
            is_touching = false; // Officially released!

            // If hardware didn't report a gesture code, calculate vector displacement
            if (pending_gesture == TouchGesture::NONE)
            {
                int16_t dx = touch_x - start_x;
                int16_t dy = touch_y - start_y;
                uint32_t duration = now - touch_start_time;

                if (abs(dx) > 35 && abs(dx) > abs(dy))
                {
                    pending_gesture = (dx < 0) ? TouchGesture::SWIPE_LEFT : TouchGesture::SWIPE_RIGHT;
                }
                else if (abs(dy) > 35 && abs(dy) > abs(dx))
                {
                    pending_gesture = (dy < 0) ? TouchGesture::SWIPE_UP : TouchGesture::SWIPE_DOWN;
                }
                else if (abs(dx) < 25 && abs(dy) < 25 && duration < 600)
                {
                    pending_gesture = TouchGesture::TAP;
                }
            }

            // Cooldown debounce check
            if (pending_gesture != TouchGesture::NONE && (now - last_gesture_time >= 300))
            {
                current_gesture = pending_gesture;
                pending_gesture = TouchGesture::NONE;
                last_gesture_time = now;
                return true; // TRIGGER EVENT ONLY AFTER RELEASING!
            }

            pending_gesture = TouchGesture::NONE;
        }

        return false;
    }
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
