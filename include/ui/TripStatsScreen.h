#pragma once

#include "ui/IScreen.h"


class TripStatsScreen : public IScreen
{
public:
    const char* getName() const override { return "TripStats"; }

    void onEnter(Arduino_GFX* gfx) override;
    void render(Arduino_GFX* gfx, const RideMetrics& metrics, bool force_redraw) override;
    void handleGesture(TouchGesture gesture) override;

private:
    float last_avg = -1.0f;
    float last_max = -1.0f;
    uint16_t last_cadence = 0xFFFF;
    uint8_t last_hr = 0xFF;
};
