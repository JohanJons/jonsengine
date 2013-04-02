#pragma once

#include "include/Input/InputBackend.h"
#include "include/Input/KeyCallback.h"
#include "include/Input/MouseCallback.h"
#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
    /* DummyInputBackend definition */
    /* Used when no proper backend specified in initialization */
    class DummyInputBackend : public InputBackend
    {
    public:
        DummyInputBackend()     { }
        ~DummyInputBackend()    { }

        void SetMouseButtonCallback(const MouseButtonCallback& onMouseButton);
        void SetMouseMotionCallback(const MouseMotionCallback& onMouseMotion);
        void SetKeyCallback(const KeyCallback& onKey);

        bool IsMouseButtonCallbackSet() const;
        bool IsMouseMotionCallbackSet() const;
        bool IsKeyCallbackSet() const;

        void ShowMouseCursor(bool show);
        void SetMousePosition(uint32_t x, uint32_t y);

        InputBackendType GetInputBackendType() const;
    };


    /* InputBackend inlines */
    inline void DummyInputBackend::SetMouseButtonCallback(const MouseButtonCallback& onMouseButton)     { }
    inline void DummyInputBackend::SetMouseMotionCallback(const MouseMotionCallback& onMouseMotion)     { }
    inline void DummyInputBackend::SetKeyCallback(const KeyCallback& onKey)                             { }

    inline bool DummyInputBackend::IsMouseButtonCallbackSet() const                                     { return false; }
    inline bool DummyInputBackend::IsMouseMotionCallbackSet() const                                     { return false; }
    inline bool DummyInputBackend::IsKeyCallbackSet() const                                             { return false; }

    inline void DummyInputBackend::ShowMouseCursor(bool show)                                           { }
    inline void DummyInputBackend::SetMousePosition(uint32_t x, uint32_t y)                             { }

    inline InputBackend::InputBackendType DummyInputBackend::GetInputBackendType() const                { return NONE; }

}