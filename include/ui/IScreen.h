#pragma once

#include <Arduino_GFX_Library.h>
#include "metrics.h"
#include "events.h"


// Standard 16-bit RGB565 color constants for GC9A01
namespace colors
{
    constexpr uint16_t BLACK     = 0x0000;
    constexpr uint16_t WHITE     = 0xFFFF;
    constexpr uint16_t RED       = 0xF800;
    constexpr uint16_t GREEN     = 0x07E0;
    constexpr uint16_t BLUE      = 0x001F;
    constexpr uint16_t CYAN      = 0x07FF;
    constexpr uint16_t MAGENTA   = 0xF81F;
    constexpr uint16_t YELLOW    = 0xFFE0;
    constexpr uint16_t ORANGE    = 0xFD20;
    constexpr uint16_t DARKGREY  = 0x39E7;
    constexpr uint16_t LIGHTGREY = 0xC618;
}


/**
 * @brief Abstract screen interface for circular UI views.
 */
class IScreen
{
public:
    virtual ~IScreen() = default;

    virtual const char* getName() const = 0;
    virtual void onEnter(Arduino_GFX* gfx) {}
    virtual void render(Arduino_GFX* gfx, const RideMetrics& metrics, bool force_redraw) = 0;
    virtual void handleGesture(TouchGesture gesture) {}
    virtual void onExit(Arduino_GFX* gfx) {}
};
