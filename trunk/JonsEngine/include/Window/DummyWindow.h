#pragma once

#include "include/Window/IWindow.h"

#include <string>

namespace JonsEngine
{
    struct EngineSettings;

    /* DummyWindow definition */
    class DummyWindow : public IWindow
    {
    public:
        DummyWindow(const EngineSettings& engineSettings);
        ~DummyWindow();

        void StartFrame();
        void EndFrame();

        bool SetupWindow();
        void CloseWindow();
        bool IsWindowOpened() const;
        uint16_t GetCurrentFPS() const;

        void SetFullscreen(bool fullscreen);
        void SetScreenResolution(const uint16_t width, const uint16_t height);
        void SetWindowTitle(const std::string& windowTitle);

        const ScreenMode& GetScreenMode() const;
        const std::string& GetWindowTitle() const;

        WindowBackendType GetWindowBackendType() const;
    

    private:
        std::string mWindowTitle;
        ScreenMode mScreenMode;
    };


    /* DummyWindow inlines */
    inline DummyWindow::WindowBackendType DummyWindow::GetWindowBackendType() const         { return IWindow::NONE;  }

}