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
        bool SetupWindow(const uint32_t width, const uint32_t height);
        static void glfwOnWindowChanged(int width, int height);

        Logger& mLogger;

        // FPS
        uint16_t mFrameLimit;
        double mStartFrameTime;
        double mLastFrameTime;
        double mThisFrameTime;

        bool mFullscreen;
        float mFOV;
        std::string mWindowTitle;
        uint16_t mMSAA;
    };


    /* GLFWWindow inlines */
    inline uint16_t GLFWWindow::GetFrameLimit() const                                 { return mFrameLimit;    }

    inline bool GLFWWindow::GetFullscreen() const                                     { return mFullscreen;    }
    inline float GLFWWindow::GetFOV() const                                           { return mFOV;           }
    inline const std::string& GLFWWindow::GetWindowTitle() const                      { return mWindowTitle;   }

    inline GLFWWindow::WindowBackend GLFWWindow::GetWindowBackendType() const         { return IWindow::GLFW; }
}