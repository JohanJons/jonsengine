#pragma once

#include "Core/Types.h"

#include <functional>

namespace JonsEngine
{
    struct MouseButtonEvent;

    typedef void(*MouseButtonCallbackSignature)(const MouseButtonEvent&);
    typedef std::function<void(const MouseButtonEvent&)> MouseButtonCallback;

	typedef MouseMovement MouseMovementEvent;
    typedef void(*MouseMovementCallbackSignature)(const MouseMovementEvent&);
    typedef std::function<void(const MouseMovementEvent&)> MouseMovementCallback;

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


    inline MouseButtonEvent::MouseButtonEvent(const MouseButton button, const MouseButtonState state) : mButton(button), mState(state) { }
}