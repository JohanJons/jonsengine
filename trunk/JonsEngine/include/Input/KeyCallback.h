#pragma once

#include "include/Core/EngineDefs.h"

#include "boost/function.hpp"

namespace JonsEngine
{
    struct KeyEvent;

    /* KeyCallback definition */
    typedef boost::function<void (const KeyEvent&)>         KeyCallback;


    /* KeyEvent definition */
    struct KeyEvent
    {
    public:
        enum KeyState
        {
            PRESSED = 0,
            RELEASED
        };

        uint16_t KeyCode;       /* Unicode for the key pressed */
        KeyState State;         /* State of the key */


        /* Default constructor */
        KeyEvent(uint16_t keyCode, KeyState state);
    };


    /* KeyEvent inlines */
    inline KeyEvent::KeyEvent(uint16_t keyCode, KeyState state) : KeyCode(keyCode), State(state) { }

}