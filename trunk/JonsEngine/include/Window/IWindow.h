#pragma once

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
    /* IWindow definition */
    class IWindow
    {
    public:
        enum WindowBackend
        {
            GLFW = 0,
            NONE
        };

        virtual ~IWindow() {}

        virtual void StartFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetFrameLimit(const uint16_t newFrameLimit) = 0;
        virtual uint16_t GetFrameLimit() const = 0;
        virtual uint16_t GetCurrentFPS() const = 0;

        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual void SetScreenResolution(const uint32_t width, const uint32_t height) = 0;
        virtual void SetWindowTitle(const std::string& windowTitle) = 0;
        virtual void SetFOV(const float FOV) = 0;

        virtual bool GetFullscreen() const = 0;
        virtual uint32_t GetScreenWidth() const = 0;
        virtual uint32_t GetScreenHeight() const = 0;
        virtual const std::string& GetWindowTitle() const = 0;
        virtual float GetFOV() const = 0;

        virtual WindowBackend GetWindowBackendType() const = 0;
    };
}