#pragma once

#include "include/Window/IWindow.h"

#include <string>

namespace JonsEngine
{
    struct EngineSettings;
    class Logger;

    /* GLFWWindow definition */
    class GLFWWindow : public IWindow
    {
    public:
        GLFWWindow(const EngineSettings& engineSettings);
        ~GLFWWindow();

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
        void UpdateViewport();
        static void glfwOnWindowChanged(int width, int height);

        Logger& mLogger;
        std::string mWindowTitle;
        ScreenMode mScreenMode;

        // FPS
        double mStartFrameTime;
        double mLastFrameTime;
        double mThisFrameTime;

    };


    /* GLFWWindow inlines */
    inline const ScreenMode& GLFWWindow::GetScreenMode() const                            { return mScreenMode;    }
    inline const std::string& GLFWWindow::GetWindowTitle() const                          { return mWindowTitle;   }
    inline GLFWWindow::WindowBackendType GLFWWindow::GetWindowBackendType() const         { return IWindow::GLFW;  }
}