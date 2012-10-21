#pragma once

#include"include/Core/EngineDefs.h"

namespace JonsEngine
{
    struct MouseButtonEvent
    {
        enum MouseButton
        {
            MOUSEBUTTON_LEFT = 0,
            MOUSEBUTTON_RIGHT,
            MOUSEBUTTON_MIDDLE
        };

        MouseButtonEvent(MouseButton button);

        /* Button pressed */
        MouseButton Button;
    };

    struct MouseMotionEvent
    {
        MouseMotionEvent();



    };

}