#pragma once

#include"include/Core/EngineDefs.h"

#include "boost/function.hpp"

namespace JonsEngine
{
    struct MouseButtonEvent;
    struct MouseMotionEvent;


    /* MouseButtonCallback definition */
    typedef boost::function<void (const MouseButtonEvent&)> MouseButtonCallback;

    /* MouseMotionCallback definition */
    typedef boost::function<void (const MouseMotionEvent&)> MouseMotionCallback;


    /* MouseButtonEvent definition */
    struct MouseButtonEvent
    {
        enum MouseButton
        {
            MOUSEBUTTON_LEFT = 0,
            MOUSEBUTTON_RIGHT,
            MOUSEBUTTON_MIDDLE
        };

        enum MouseButtonState
        {
            PRESSED = 0,
            RELEASED
        };

        MouseButton Button;         /* Button pressed */
        MouseButtonState State;     /* State of button */

       
        MouseButtonEvent(MouseButton button, MouseButtonState state);
    };


    /* MouseMotionEvent definition */
    struct MouseMotionEvent
    {
        uint32_t PosX;      /* Mouse X-position */
        uint32_t PosY;      /* Mouse Y-position */

        
        MouseMotionEvent(uint32_t x, uint32_t y);
    };



    /* MouseButtonEvent inlines */
    inline MouseButtonEvent::MouseButtonEvent(MouseButton button, MouseButtonState state) : Button(button), State(state) { }

    /* MouseMotionEvent inlines */
    inline MouseMotionEvent::MouseMotionEvent(uint32_t x, uint32_t y) : PosX(x), PosY(y) { }
}