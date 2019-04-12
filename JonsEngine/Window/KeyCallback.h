#pragma once

#include "Types.h"
#include "KeyDefinition.h"

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
        enum class KeyState
        {
            STATE_PRESSED,
            STATE_RELEASED,
            STATE_REPEAT
        };

        enum class KeyModifier
        {
            MODIFIER_SHIFT,
            MODIFIER_CONTROL,
            MODIFIER_ALT
        };

        const Key mKey;
        const uint16_t mScancode;      /* System-specific scancode */
        const KeyState mState;

        const bool mControlPressed;
        const bool mAltPressed;
        const bool mShiftPressed;


        KeyEvent(const Key keySymbol, const uint16_t scanCode, const KeyState state, const bool controlPressed, const bool altPressed, const bool shiftPressed);
    };


    /* KeyEvent inlines */
    inline KeyEvent::KeyEvent(const Key keySymbol, const uint16_t scanCode, const KeyState state, const bool controlPressed, const bool altPressed, const bool shiftPressed)
        : 
        mKey(keySymbol), mScancode(scanCode), mState(state), mControlPressed(controlPressed), mAltPressed(altPressed), mShiftPressed(shiftPressed)
    {}

}