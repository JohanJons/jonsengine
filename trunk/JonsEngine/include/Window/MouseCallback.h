#pragma once

#include"include/Core/EngineDefs.h"

#include "boost/function.hpp"

namespace JonsEngine
{
    struct MouseButtonEvent;
    struct MouseMotionEvent;


    /* MouseButtonCallback definition */
    typedef void (*MouseButtonCallbackSignature)(const MouseButtonEvent&);
    typedef boost::function<void (const MouseButtonEvent&)> MouseButtonCallback;

    /* MouseMotionCallback definition */
    typedef void (*MouseMotionCallbackSignature)(const MouseMotionEvent&);
    typedef boost::function<void (const MouseMotionEvent&)> MouseMotionCallback;


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

        enum MouseModifier
        {
            MODIFIER_NONE = 0,
            MODIFIER_SHIFT,
            MODIFIER_CONTROL,
            MODIFIER_ALT
        };

        MouseButton mButton;
        MouseButtonState mState;
        MouseModifier mModifier;

       
        MouseButtonEvent(MouseButton button, MouseButtonState state, MouseModifier modifier);
    };


    /* MouseMotionEvent definition */
    struct MouseMotionEvent
    {
        double mPosX;
        double mPosY;

        
        MouseMotionEvent(double x, double y);
    };



    /* MouseButtonEvent inlines */
    inline MouseButtonEvent::MouseButtonEvent(MouseButton button, MouseButtonState state, MouseModifier modifier) : mButton(button), mState(state), mModifier(modifier) { }

    /* MouseMotionEvent inlines */
    inline MouseMotionEvent::MouseMotionEvent(double x, double y) : mPosX(x), mPosY(y) { }
}