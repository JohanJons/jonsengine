#pragma once

#include "include/Window/KeyCallback.h"
#include "include/Window/KeyDefinition.h"
#include "include/Window/MouseCallback.h"

#include <Windows.h>
#include <string>
#include <vector>


namespace JonsEngine
{
    struct EngineSettings;
    class Logger;

    /* WindowManagerImpl definition */
    class WindowManagerImpl
    {
    public:
        WindowManagerImpl(const EngineSettings& engineSettings, Logger& logger);
        ~WindowManagerImpl();

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
        Logger& mLogger;

        std::string mWindowTitle;
        uint32_t mScreenWidth;
        uint32_t mScreenHeight;
        bool mShowMouseCursor;
        bool mFullscreen;
        float mFOV;
        double mPreviousMouseX;
        double mPreviousMouseY;

        HINSTANCE mInstanceHandle;
        HWND mWindowHandle;
    };
}