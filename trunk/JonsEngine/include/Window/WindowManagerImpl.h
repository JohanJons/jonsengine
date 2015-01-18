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
        void SetMousePositionCallback();
        void SetMousePositionCallback(const MousePositionCallback& callback);
        void SetKeyCallback();
        void SetKeyCallback(const KeyCallback& callback);

		void ShowMouseCursor(const bool show);
		void SetMousePosition(const uint32_t x, const uint32_t y);

		void SetFullscreen(const bool fullscreen);
        void SetScreenResolution(const uint32_t width, const uint32_t height);
        void SetWindowTitle(const std::string& windowTitle);

        bool GetFullscreen() const;
        uint16_t GetScreenWidth() const;
        uint16_t GetScreenHeight() const;
        const std::string& GetWindowTitle() const;
        bool GetShowMouseCursor() const;


    private:
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        void ProcessKeyboardInput(const RAWKEYBOARD& keyInput);
        void ProcessMouseInput(const RAWMOUSE& mouseInput);

        Logger& mLogger;

        std::string mWindowTitle;
        uint16_t mScreenWidth;
        uint16_t mScreenHeight;
        bool mShowMouseCursor;
        bool mFullscreen;
        int32_t mRelativePosX;
        int32_t mRelativePosY;

        MouseButtonCallback mMouseButtonCallback;
        MousePositionCallback mMousePositionCallback;
        KeyCallback mKeyCallback;

        std::vector<MouseButtonEvent> mMouseButtonEvents;
        std::vector<KeyEvent> mKeyEvents;

        HINSTANCE mInstanceHandle;
        HWND mWindowHandle;
    };
}