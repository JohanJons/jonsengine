#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Input/KeyDefinition.h"

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

        Key KeySymbol;          /* Unicode for the key pressed */
        KeyState State;         /* State of the key */


        KeyEvent(Key keySymbol, KeyState state);
    };


    /* KeyEvent inlines */
    inline KeyEvent::KeyEvent(Key keySymbol, KeyState state) : KeySymbol(keySymbol), State(state) { }

}