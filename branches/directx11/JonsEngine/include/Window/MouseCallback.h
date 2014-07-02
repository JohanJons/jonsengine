#pragma once

#include"include/Core/EngineDefs.h"

#include <functional>

namespace JonsEngine
{
    struct MouseButtonEvent;
    struct MousePositionEvent;


    /* MouseButtonCallback definition */
    typedef void(*MouseButtonCallbackSignature)(const MouseButtonEvent&);
    typedef std::function<void(const MouseButtonEvent&)> MouseButtonCallback;

    /* MousePositionCallback definition */
    typedef void(*MousePositionCallbackSignature)(const MousePositionEvent&);
    typedef std::function<void(const MousePositionEvent&)> MousePositionCallback;

    /* MouseButtonEvent definition */
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


    /* MousePositionEvent definition */
    struct MousePositionEvent
    {
        double mPreviousPosX;
        double mPreviousPosY;
        double mCurrentPosX;
        double mCurrentPosY;


        MousePositionEvent(const double prevX, const double prevY, const double currX, const double currY);
    };


    /* MouseButtonEvent inlines */
    inline MouseButtonEvent::MouseButtonEvent(const MouseButton button, const MouseButtonState state) : mButton(button), mState(state) { }

    /* MousePositionEvent inlines */
    inline MousePositionEvent::MousePositionEvent(const double prevX, const double prevY, const double currX, const double currY) : mPreviousPosX(prevX), mPreviousPosY(prevY), mCurrentPosX(currX), mCurrentPosY(currY) { }
}