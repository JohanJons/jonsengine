#pragma once

#include"include/Core/EngineDefs.h"

#include <functional>

namespace JonsEngine
{
    struct MouseButtonEvent;
    struct MouseMotionEvent;
	struct MousePositionEvent;


    /* MouseButtonCallback definition */
    typedef void (*MouseButtonCallbackSignature)(const MouseButtonEvent&);
    typedef std::function<void (const MouseButtonEvent&)> MouseButtonCallback;

    /* MouseMotionCallback definition */
    typedef void (*MouseMotionCallbackSignature)(const MouseMotionEvent&);
    typedef std::function<void (const MouseMotionEvent&)> MouseMotionCallback;

	/* MousePositionCallback definition */
    typedef void (*MousePositionCallbackSignature)(const MousePositionEvent&);
    typedef std::function<void (const MousePositionEvent&)> MousePositionCallback;

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
        float mPosX;
        float mPosY;

        
        MouseMotionEvent(const float x, const float y);
    };

	
	/* MousePositionEvent definition */
	struct MousePositionEvent
	{
		float mPreviousPosX;
		float mPreviousPosY;
		float mCurrentPosX;
		float mCurrentPosY;
		
		
		MousePositionEvent(const float prevX, const float prevY, const float currX, const float currY);
	};


    /* MouseButtonEvent inlines */
    inline MouseButtonEvent::MouseButtonEvent(MouseButton button, MouseButtonState state, MouseModifier modifier) : mButton(button), mState(state), mModifier(modifier) { }

    /* MouseMotionEvent inlines */
    inline MouseMotionEvent::MouseMotionEvent(const float x, const float y) : mPosX(x), mPosY(y) { }
	
	/* MousePositionEvent inlines */
    inline MousePositionEvent::MousePositionEvent(const float prevX, const float prevY, const float currX, const float currY) : mPreviousPosX(prevX), mPreviousPosY(prevY), mCurrentPosX(currX), mCurrentPosY(currY) { }
}