#pragma once

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
    /* KeyEvent definition */
    struct KeyEvent
    {
    public:
        enum KeyState
        {
            PRESSED = 0,
            RELEASED
        };

        KeyEvent(uint16_t keyCode, KeyState state);

        /* Unicode for the key pressed */
        uint16_t KeyCode;
        
        /* State of the key */
        KeyState State;
    };


    /* KeyEvent inlines */
    inline KeyEvent::KeyEvent(uint16_t keyCode, KeyState state) : KeyCode(keyCode), State(state) { }

}