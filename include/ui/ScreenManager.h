#pragma once

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "ui/IScreen.h"
#include "events.h"
#include "metrics.h"


class ScreenManager
{
public:
    static constexpr size_t MAX_SCREENS = 6;

    explicit ScreenManager(Arduino_GFX* display);

    bool addScreen(IScreen* screen);
    void nextScreen();
    void prevScreen();
    void setScreen(size_t index);

    void processGesture(TouchGesture gesture);
    void render(const RideMetrics& metrics);

    size_t getActiveIndex() const { return current_index; }
    size_t getScreenCount() const { return screen_count; }

private:
    Arduino_GFX* gfx;
    IScreen* screens[MAX_SCREENS] = {nullptr};
    size_t screen_count = 0;
    size_t current_index = 0;
    bool force_redraw = true;
};
