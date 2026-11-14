#include "ui/ScreenManager.h"


ScreenManager::ScreenManager(Arduino_GFX* display)
    : gfx(display), screen_count(0), current_index(0), force_redraw(true)
{
}


bool ScreenManager::addScreen(IScreen* screen)
{
    if (screen_count >= MAX_SCREENS || screen == nullptr)
    {
        return false;
    }

    screens[screen_count++] = screen;
    return true;
}


void ScreenManager::nextScreen()
{
    if (screen_count <= 1) return;

    if (screens[current_index])
    {
        screens[current_index]->onExit(gfx);
    }

    current_index = (current_index + 1) % screen_count;
    force_redraw = true;

    if (screens[current_index])
    {
        screens[current_index]->onEnter(gfx);
    }
}


void ScreenManager::prevScreen()
{
    if (screen_count <= 1) return;

    if (screens[current_index])
    {
        screens[current_index]->onExit(gfx);
    }

    current_index = (current_index == 0) ? (screen_count - 1) : (current_index - 1);
    force_redraw = true;

    if (screens[current_index])
    {
        screens[current_index]->onEnter(gfx);
    }
}


void ScreenManager::setScreen(size_t index)
{
    if (index >= screen_count || index == current_index) return;

    if (screens[current_index])
    {
        screens[current_index]->onExit(gfx);
    }

    current_index = index;
    force_redraw = true;

    if (screens[current_index])
    {
        screens[current_index]->onEnter(gfx);
    }
}


void ScreenManager::processGesture(TouchGesture gesture)
{
    if (gesture == TouchGesture::NONE) return;

    // Handle universal navigation gestures
    if (gesture == TouchGesture::SWIPE_LEFT)
    {
        nextScreen();
    }
    else if (gesture == TouchGesture::SWIPE_RIGHT)
    {
        prevScreen();
    }
    else
    {
        // Delegate other gestures (tap, swipe up/down) to active screen
        if (screens[current_index])
        {
            screens[current_index]->handleGesture(gesture);
        }
    }
}


void ScreenManager::render(const RideMetrics& metrics)
{
    if (screen_count == 0 || screens[current_index] == nullptr) return;

    screens[current_index]->render(gfx, metrics, force_redraw);
    force_redraw = false;
}
