#pragma once

#include "include/Input/InputBackend.h"

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

        InputBackendType GetInputBackendType() const;
    };

    /* InputBackend inlines */
    inline void DummyInputBackend::SetMouseButtonCallback(const MouseButtonCallback& onMouseButton)     { }
    inline void DummyInputBackend::SetMouseMotionCallback(const MouseMotionCallback& onMouseMotion)     { }
    inline void DummyInputBackend::SetKeyCallback(const KeyCallback& onKey)                             { }

    inline bool DummyInputBackend::IsMouseButtonCallbackSet() const                                     { return false; }
    inline bool DummyInputBackend::IsMouseMotionCallbackSet() const                                     { return false; }
    inline bool DummyInputBackend::IsKeyCallbackSet() const                                             { return false; }

    inline InputBackend::InputBackendType DummyInputBackend::GetInputBackendType() const                { return NONE; }

}