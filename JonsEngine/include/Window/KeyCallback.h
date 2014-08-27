#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Window/KeyDefinition.h"

#include <functional>
#include <bitset>

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
        typedef std::bitset<sizeof(uint32_t)> KeyModifiers;

        enum KeyState
        {
            STATE_UNKNOWN = 0,
            STATE_PRESSED,
            STATE_RELEASED,
            STATE_REPEAT
        };

        enum KeyModifier
        {
            MODIFIER_SHIFT = 0,
            MODIFIER_CONTROL,
            MODIFIER_ALT
        };

        Key mKey;
        uint16_t mScancode;      /* System-specific scancode */
        KeyState mState;
        KeyModifiers mModifiers;


        KeyEvent(Key keySymbol, uint16_t scanCode, KeyState state, KeyModifiers modifiers);
    };


    /* KeyEvent inlines */
    inline KeyEvent::KeyEvent(Key keySymbol, uint16_t scanCode, KeyState state, KeyModifiers modifiers) : mKey(keySymbol), mScancode(scanCode), mState(state), mModifiers(modifiers) { }

}