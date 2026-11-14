#include "ui/DashboardScreen.h"

using namespace colors;


void DashboardScreen::onEnter(Arduino_GFX* gfx)
{
    last_speed = -1.0f;
    last_distance = -1.0f;
    last_time = 0xFFFFFFFF;
    last_recording = false;
}


void DashboardScreen::render(Arduino_GFX* gfx, const RideMetrics& metrics, bool force_redraw)
{
    if (force_redraw)
    {
        gfx->fillScreen(BLACK);

        // Circular outer accent ring
        gfx->drawCircle(120, 120, 118, 0x07FF); // Cyan
        gfx->drawCircle(120, 120, 117, 0x07FF);

        // Unit label
        gfx->setTextColor(0xC618, BLACK); // Light grey
        gfx->setTextSize(2);
        gfx->setCursor(95, 130);
        gfx->print("km/h");

        // Lower divider line
        gfx->drawFastHLine(45, 155, 150, 0x39E7); // Dark grey

        // Labels for bottom stats
        gfx->setTextColor(0xC618, BLACK);
        gfx->setTextSize(1);
        gfx->setCursor(55, 168);
        gfx->print("DIST:");
        gfx->setCursor(55, 188);
        gfx->print("TIME:");

        // Page indicator dots at bottom (3 dots: left active)
        gfx->fillCircle(104, 215, 3, 0x07FF);
        gfx->drawCircle(120, 215, 3, 0x39E7);
        gfx->drawCircle(136, 215, 3, 0x39E7);
    }

    // Top status indicator (Recording state + Battery)
    if (force_redraw || last_recording != metrics.is_recording)
    {
        last_recording = metrics.is_recording;
        uint16_t status_color = metrics.is_recording ? 0x07E0 : 0xFD20; // Green / Orange

        gfx->fillCircle(65, 42, 4, status_color);
        gfx->setTextColor(status_color, BLACK);
        gfx->setTextSize(1);
        gfx->setCursor(75, 39);
        gfx->print(metrics.is_recording ? "RECORDING" : "PAUSED   ");

        // Battery percentage
        gfx->setTextColor(0xC618, BLACK);
        gfx->setCursor(160, 39);
        gfx->printf("%d%% ", metrics.battery_percent);
    }

    // Main Speed Display (Center, Large)
    if (force_redraw || abs(last_speed - metrics.speed_kph) > 0.05f)
    {
        last_speed = metrics.speed_kph;
        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(5);

        // Center speed text (approx 30px per char width at size 5)
        char speed_buf[16];
        snprintf(speed_buf, sizeof(speed_buf), "%.1f", metrics.speed_kph);

        int len = strlen(speed_buf);
        int x_pos = 120 - (len * 15);
        gfx->setCursor(x_pos, 75);
        gfx->print(speed_buf);
    }

    // Distance reading
    if (force_redraw || abs(last_distance - metrics.trip_distance_km) > 0.05f)
    {
        last_distance = metrics.trip_distance_km;
        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(1);
        gfx->setCursor(105, 168);
        gfx->printf("%.2f km   ", metrics.trip_distance_km);
    }

    // Moving time reading
    if (force_redraw || last_time != metrics.moving_time_sec)
    {
        last_time = metrics.moving_time_sec;
        uint32_t hours   = metrics.moving_time_sec / 3600;
        uint32_t minutes = (metrics.moving_time_sec % 3600) / 60;
        uint32_t seconds = metrics.moving_time_sec % 60;

        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(1);
        gfx->setCursor(105, 188);
        gfx->printf("%02lu:%02lu:%02lu", (unsigned long)hours, (unsigned long)minutes, (unsigned long)seconds);
    }
}


void DashboardScreen::handleGesture(TouchGesture gesture)
{
    // Tap or long press actions can be handled here
    if (gesture == TouchGesture::TAP)
    {
        Serial.println("[DashboardScreen] TAP detected");
    }
}
