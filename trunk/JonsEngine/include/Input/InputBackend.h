#pragma once

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

        virtual InputBackendType GetInputBackendType() const;
    };


    /* InputBackend inlines */
    inline InputBackend::InputBackendType InputBackend::GetInputBackendType() const         { return NONE; }
}