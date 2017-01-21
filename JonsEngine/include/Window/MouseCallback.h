#pragma once

#include"include/Core/Types.h"

#include <functional>

namespace JonsEngine
{
    struct MouseButtonEvent;
    struct MousePositionEvent;


    typedef void(*MouseButtonCallbackSignature)(const MouseButtonEvent&);
    typedef std::function<void(const MouseButtonEvent&)> MouseButtonCallback;

    typedef void(*MousePositionCallbackSignature)(const MousePositionEvent&);
    typedef std::function<void(const MousePositionEvent&)> MousePositionCallback;

    struct MouseButtonEvent
    {
        enum MouseButton
        {
            BUTTON_NONE = 0,
            BUTTON_LEFT,
            BUTTON_RIGHT,
            BUTTON_MIDDLE
        };

        enum MouseButtonState
        {
            STATE_UNKNOWN = 0,
            STATE_PRESSED,
            STATE_RELEASED
        };

        MouseButton mButton;
        MouseButtonState mState;


        MouseButtonEvent(MouseButton button, MouseButtonState state);
    };

    struct MousePositionEvent
    {
        int32_t mRelativePosX;
        int32_t mRelativePosY;


        MousePositionEvent(const int32_t relX, const int32_t relY);
    };


    inline MouseButtonEvent::MouseButtonEvent(const MouseButton button, const MouseButtonState state) : mButton(button), mState(state) { }
    inline MousePositionEvent::MousePositionEvent(const int32_t relX, const int32_t relY) : mRelativePosX(relX), mRelativePosY(relY) { }
}