#pragma once

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
    /* ScreenMode definition */
    struct ScreenMode
    {
        uint16_t mScreenWidth;
        uint16_t mScreenHeight;
        uint16_t mFrameLimit;
        bool mFrameLimitEnabled;
        bool mFullscreen;
        float mFOV;
        float mZNear;
        float mZFar;

        ScreenMode();
    };


    /* IWindow definition */
    class IWindow
    {
    public:
        enum WindowBackendType
        {
            GLFW = 0,
            NONE
        };

        virtual ~IWindow() {}

        virtual void StartFrame() = 0;
        virtual void EndFrame() = 0;

        virtual bool SetupWindow() = 0;
        virtual void CloseWindow() = 0;
        virtual bool IsWindowOpened() const = 0;
        virtual uint16_t GetCurrentFPS() const = 0;

        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual void SetScreenResolution(const uint16_t width, const uint16_t height) = 0;
        virtual void SetWindowTitle(const std::string& windowTitle) = 0;

        virtual const ScreenMode& GetScreenMode() const = 0;
        virtual const std::string& GetWindowTitle() const = 0;

        virtual WindowBackendType GetWindowBackendType() const = 0;
    };


     /* ScreenMode inlines */
    inline ScreenMode::ScreenMode() : mScreenWidth(800), mScreenHeight(600), mFrameLimit(60), mFrameLimitEnabled(true), mFullscreen(false), mFOV(45.0f), mZNear(0.5f), mZFar(1000.0f)
    {
    }
}