#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Window/KeyDefinition.h"

#include <functional>

namespace JonsEngine
{
    struct KeyEvent;

    /* KeyCallback definition */
    typedef void (*KeyCallbackSignature)(const KeyEvent&);
    typedef std::function<void (const KeyEvent&)> KeyCallback;


    /* KeyEvent definition */
    struct KeyEvent
    {
    public:
        enum KeyState
        {
            STATE_UNKNOWN = 0,
            STATE_PRESSED,
            STATE_RELEASED,
            STATE_REPEAT
        };

        enum KeyModifier
        {
            MODIFIER_NONE = 0,
            MODIFIER_SHIFT,
            MODIFIER_CONTROL,
            MODIFIER_ALT
        };

        Key mKey;
        int mScancode;      /* System-specific scancode */
        KeyState mState;
        KeyModifier mModifier;


        KeyEvent(Key keySymbol, int scanCode, KeyState state, KeyModifier modifier);
    };


    /* KeyEvent inlines */
    inline KeyEvent::KeyEvent(Key keySymbol, int scanCode, KeyState state, KeyModifier modifier) : mKey(keySymbol), mScancode(scanCode), mState(state), mModifier(modifier) { }

}