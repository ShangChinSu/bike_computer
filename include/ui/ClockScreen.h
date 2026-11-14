#pragma once

#include "ui/IScreen.h"
#include <time.h>


class ClockScreen : public IScreen
{
public:
    const char* getName() const override { return "Clock"; }

    void onEnter(Arduino_GFX* gfx) override;
    void render(Arduino_GFX* gfx, const RideMetrics& metrics, bool force_redraw) override;
    void handleGesture(TouchGesture gesture) override;

private:
    int last_hour = -1;
    int last_min = -1;
    int last_sec = -1;
    int last_day = -1;
};
