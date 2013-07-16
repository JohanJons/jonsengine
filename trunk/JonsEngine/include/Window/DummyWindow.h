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

        void SetFrameLimit(const uint16_t newFrameLimit);
        uint16_t GetFrameLimit() const;
        uint16_t GetCurrentFPS() const;

        void SetFullscreen(bool fullscreen);
        void SetScreenResolution(const uint32_t width, const uint32_t height);
        void SetWindowTitle(const std::string& windowTitle);
        void SetFOV(const float FOV);

        bool GetFullscreen() const;
        uint32_t GetScreenWidth() const;
        uint32_t GetScreenHeight() const;
        const std::string& GetWindowTitle() const;
        float GetFOV() const;

        WindowBackend GetWindowBackendType() const;
    

    private:
        std::string mWindowTitle;
    };


    /* DummyWindow inlines */
    inline DummyWindow::WindowBackend DummyWindow::GetWindowBackendType() const         { return IWindow::NONE;  }

}