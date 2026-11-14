#pragma once

#include <Arduino.h>


/**
 * @brief Hardware-decoded touch gestures from the CST816D controller.
 */
enum class TouchGesture
{
    NONE = 0,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_UP,
    SWIPE_DOWN,
    TAP,
    DOUBLE_TAP,
    LONG_PRESS
};
