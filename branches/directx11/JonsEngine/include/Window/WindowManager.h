#pragma once

#include "include/Window/KeyCallback.h"
#include "include/Window/KeyDefinition.h"
#include "include/Window/MouseCallback.h"

#include <string>
#include <vector>
#include <memory>


namespace JonsEngine
{
    struct EngineSettings;
    class Logger;

    /* WindowManager definition */
    class WindowManager
    {
    public:
        WindowManager(const EngineSettings& engineSettings);
        ~WindowManager();

        /*
        * Dispatches queued input events to all registered callbacks.
        * Is called by the Engine once per frame.
        */
        void Poll();

        void SetMouseButtonCallback();
        void SetMouseButtonCallback(const MouseButtonCallback& callback);
        void SetMouseMotionCallback();
        void SetMouseMotionCallback(const MouseMotionCallback& callback);
        void SetMousePositionCallback();
        void SetMousePositionCallback(const MousePositionCallback& callback);
        void SetKeyCallback();
        void SetKeyCallback(const KeyCallback& callback);

        void ShowMouseCursor(bool show);
        void SetMousePosition(uint32_t x, uint32_t y);

        void SetFrameLimit(const uint64_t newFrameLimit);
        uint64_t GetFrameLimit() const;
        uint64_t GetCurrentFPS() const;

        void SetFullscreen(bool fullscreen);
        void SetScreenResolution(const uint32_t width, const uint32_t height);
        void SetWindowTitle(const std::string& windowTitle);
        void SetFOV(const float FOV);

        bool GetFullscreen() const;
        uint32_t GetScreenWidth() const;
        uint32_t GetScreenHeight() const;
        const std::string& GetWindowTitle() const;
        float GetFOV() const;


    private:
    };
}