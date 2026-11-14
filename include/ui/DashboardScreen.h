#pragma once

#include "ui/IScreen.h"


class DashboardScreen : public IScreen
{
public:
    const char* getName() const override { return "Dashboard"; }

    void onEnter(Arduino_GFX* gfx) override;
    void render(Arduino_GFX* gfx, const RideMetrics& metrics, bool force_redraw) override;
    void handleGesture(TouchGesture gesture) override;

private:
    float last_speed = -1.0f;
    float last_distance = -1.0f;
    uint32_t last_time = 0xFFFFFFFF;
    bool last_recording = false;
};
