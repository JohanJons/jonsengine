#pragma once

#include "include/Input/KeyCallback.h"
#include "include/Input/MouseCallback.h"

namespace JonsEngine
{
    /* InputBackend definition */
    class InputBackend
    {
    public:
        enum InputBackendType
        {
            GLFW = 0,
            NONE
        }; 

        virtual ~InputBackend() { }

        virtual void SetMouseButtonCallback(const MouseButtonCallback& onMouseButton) = 0;
        virtual void SetMouseMotionCallback(const MouseMotionCallback& onMouseMotion) = 0;
        virtual void SetKeyCallback(const KeyCallback& onKey) = 0;

        virtual bool IsMouseButtonCallbackSet() const = 0;
        virtual bool IsMouseMotionCallbackSet() const = 0;
        virtual bool IsKeyCallbackSet() const = 0;

        virtual void ShowMouseCursor(bool show) = 0;
        virtual void SetMousePosition(uint32_t x, uint32_t y) = 0;

        virtual InputBackendType GetInputBackendType() const = 0;
    };
}