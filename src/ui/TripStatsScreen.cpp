#include "ui/TripStatsScreen.h"

using namespace colors;


void TripStatsScreen::onEnter(Arduino_GFX* gfx)
{
    last_avg = -1.0f;
    last_max = -1.0f;
    last_cadence = 0xFFFF;
    last_hr = 0xFF;
}


void TripStatsScreen::render(Arduino_GFX* gfx, const RideMetrics& metrics, bool force_redraw)
{
    if (force_redraw)
    {
        gfx->fillScreen(BLACK);

        // Circular outer accent ring
        gfx->drawCircle(120, 120, 118, 0xFFE0); // Yellow
        gfx->drawCircle(120, 120, 117, 0xFFE0);

        // Title
        gfx->setTextColor(0xFFE0, BLACK);
        gfx->setTextSize(2);
        gfx->setCursor(55, 38);
        gfx->print("TRIP STATS");

        gfx->drawFastHLine(45, 62, 150, 0x39E7);

        // Labels
        gfx->setTextColor(0xC618, BLACK);
        gfx->setTextSize(1);

        gfx->setCursor(45, 75);
        gfx->print("AVG SPEED:");

        gfx->setCursor(45, 108);
        gfx->print("MAX SPEED:");

        gfx->setCursor(45, 141);
        gfx->print("CADENCE:");

        gfx->setCursor(45, 174);
        gfx->print("HEART RATE:");

        // Page indicator dots at bottom (left inactive, right active)
        gfx->drawCircle(112, 215, 3, 0x39E7);
        gfx->fillCircle(128, 215, 3, 0xFFE0);
    }

    // Avg speed
    if (force_redraw || abs(last_avg - metrics.avg_speed_kph) > 0.05f)
    {
        last_avg = metrics.avg_speed_kph;
        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(2);
        gfx->setCursor(45, 87);
        gfx->printf("%.1f km/h  ", metrics.avg_speed_kph);
    }

    // Max speed
    if (force_redraw || abs(last_max - metrics.max_speed_kph) > 0.05f)
    {
        last_max = metrics.max_speed_kph;
        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(2);
        gfx->setCursor(45, 120);
        gfx->printf("%.1f km/h  ", metrics.max_speed_kph);
    }

    // Cadence
    if (force_redraw || last_cadence != metrics.cadence_rpm)
    {
        last_cadence = metrics.cadence_rpm;
        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(2);
        gfx->setCursor(45, 153);
        gfx->printf("%u rpm  ", metrics.cadence_rpm);
    }

    // Heart rate
    if (force_redraw || last_hr != metrics.heart_rate_bpm)
    {
        last_hr = metrics.heart_rate_bpm;
        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(2);
        gfx->setCursor(45, 186);
        gfx->printf("%u bpm  ", metrics.heart_rate_bpm);
    }
}


void TripStatsScreen::handleGesture(TouchGesture gesture)
{
    if (gesture == TouchGesture::TAP)
    {
        Serial.println("[TripStatsScreen] TAP detected");
    }
}
