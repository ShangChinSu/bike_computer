#include "ui/ClockScreen.h"

using namespace colors;


void ClockScreen::onEnter(Arduino_GFX* gfx)
{
    last_hour = -1;
    last_min  = -1;
    last_sec  = -1;
    last_day  = -1;
}


void ClockScreen::render(Arduino_GFX* gfx, const RideMetrics& metrics, bool force_redraw)
{
    // Retrieve system time
    time_t raw_time = time(nullptr);
    struct tm timeinfo;
    localtime_r(&raw_time, &timeinfo);

    // If NTP has not synced yet, fallback to elapsed uptime as clock
    bool time_valid = (timeinfo.tm_year >= (2020 - 1900));
    int hour = time_valid ? timeinfo.tm_hour : ((millis() / 3600000) % 24);
    int min  = time_valid ? timeinfo.tm_min  : ((millis() / 60000) % 60);
    int sec  = time_valid ? timeinfo.tm_sec  : ((millis() / 1000) % 60);
    int day  = time_valid ? timeinfo.tm_mday : 1;

    if (force_redraw)
    {
        gfx->fillScreen(BLACK);

        // Circular outer accent ring (Sport Green)
        gfx->drawCircle(120, 120, 118, GREEN);
        gfx->drawCircle(120, 120, 117, GREEN);

        // Dividers
        gfx->drawFastHLine(45, 65, 150, DARKGREY);
        gfx->drawFastHLine(45, 160, 150, DARKGREY);

        // Page indicator dots at bottom (3 dots: rightmost active)
        gfx->drawCircle(104, 215, 3, DARKGREY);
        gfx->drawCircle(120, 215, 3, DARKGREY);
        gfx->fillCircle(136, 215, 3, GREEN);
    }

    // Date header (Top, Y = 40)
    if (force_redraw || last_day != day)
    {
        last_day = day;
        char date_buf[24];
        if (time_valid)
        {
            const char* days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
            const char* months[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
            snprintf(date_buf, sizeof(date_buf), "%s, %s %02d",
                     days[timeinfo.tm_wday], months[timeinfo.tm_mon], timeinfo.tm_mday);
        }
        else
        {
            snprintf(date_buf, sizeof(date_buf), "CLOCK (LOCAL)");
        }

        gfx->setTextColor(LIGHTGREY, BLACK);
        gfx->setTextSize(2);
        int date_len = strlen(date_buf);
        int x_pos = 120 - (date_len * 6); // approx 12px per char width
        gfx->setCursor(x_pos, 42);
        gfx->print(date_buf);
    }

    // Main Time: HH:MM (Center, Y = 80, Large Font size 5)
    if (force_redraw || last_hour != hour || last_min != min)
    {
        last_hour = hour;
        last_min  = min;

        char time_buf[16];
        snprintf(time_buf, sizeof(time_buf), "%02d:%02d", hour, min);

        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(5);
        gfx->setCursor(45, 85);
        gfx->print(time_buf);
    }

    // Seconds: :SS (Center below HH:MM, Y = 135)
    if (force_redraw || last_sec != sec)
    {
        last_sec = sec;

        char sec_buf[16];
        snprintf(sec_buf, sizeof(sec_buf), ":%02d", sec);

        gfx->setTextColor(GREEN, BLACK);
        gfx->setTextSize(2);
        gfx->setCursor(105, 138);
        gfx->print(sec_buf);
    }

    // Bottom Information (Y = 175)
    if (force_redraw)
    {
        gfx->setTextColor(LIGHTGREY, BLACK);
        gfx->setTextSize(1);
        gfx->setCursor(55, 175);
        gfx->printf("BAT: %d%%", metrics.battery_percent);

        gfx->setCursor(140, 175);
        gfx->print(time_valid ? "NTP SYNC" : "UPTIME");
    }
}


void ClockScreen::handleGesture(TouchGesture gesture)
{
    if (gesture == TouchGesture::TAP)
    {
        Serial.println("[ClockScreen] TAP detected");
    }
}
