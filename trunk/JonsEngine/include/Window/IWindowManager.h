#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Window/MouseCallback.h"
#include "include/Window/KeyCallback.h"

namespace JonsEngine
{
    /* IWindowManager definition */
    class IWindowManager
    {
    public:
        enum WindowBackend
        {
            GLFW = 0,
        };

        virtual ~IWindowManager() {}

        virtual void StartFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetMouseButtonCallback() = 0;
        virtual void SetMouseButtonCallback(const MouseButtonCallback& callback) = 0;
        virtual void SetMouseMotionCallback() = 0;
        virtual void SetMouseMotionCallback(const MouseMotionCallback& callback) = 0;
        virtual void SetKeyCallback() = 0;
        virtual void SetKeyCallback(const KeyCallback& callback) = 0;

        virtual void ShowMouseCursor(bool show) = 0;
        virtual void SetMousePosition(uint32_t x, uint32_t y) = 0;

        /*
         * Dispatches queued input events to all registered callbacks.
         * Is called by the Engine once per frame.
         */
        virtual void Poll() = 0;

        virtual void SetFrameLimit(const uint64_t newFrameLimit) = 0;
        virtual uint64_t GetFrameLimit() const = 0;
        virtual uint64_t GetCurrentFPS() const = 0;

        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual void SetScreenResolution(const uint32_t width, const uint32_t height) = 0;
        virtual void SetWindowTitle(const std::string& windowTitle) = 0;
        virtual void SetFOV(const float FOV) = 0;
        virtual void SetMSAA(const uint16_t msaa) = 0;

        virtual bool GetFullscreen() const = 0;
        virtual uint32_t GetScreenWidth() const = 0;
        virtual uint32_t GetScreenHeight() const = 0;
        virtual const std::string& GetWindowTitle() const = 0;
        virtual float GetFOV() const = 0;
        uint16_t GetMSAA() const;

        virtual WindowBackend GetWindowBackendType() const = 0;
    };
}