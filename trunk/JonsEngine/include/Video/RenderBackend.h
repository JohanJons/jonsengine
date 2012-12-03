#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Utils/Types.h"

namespace JonsEngine
{
    /* ScreenMode definition */
    struct ScreenMode
    {
        uint16_t ScreenWidth;
        uint16_t ScreenHeight;
        uint16_t FrameLimit;
        bool FrameLimitEnabled;
        bool Fullscreen;


        ScreenMode();
    };


    /* RenderBackend definition */
    class RenderBackend
    {
    public:
        enum RenderBackendType
        {
            OPENGL = 0,
            NONE
        };

        virtual ~RenderBackend() { }

        virtual void StartFrame() = 0;
        virtual void EndFrame() = 0;

        virtual bool SetupWindow() = 0;
        virtual void CloseWindow() = 0;
        virtual bool IsWindowOpened() const = 0;
        virtual uint16_t GetCurrentFPS() const = 0;

        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual void SetScreenResolution(const uint16_t width, const uint16_t height) = 0;
        virtual void SetWindowTitle(const std::string& windowTitle) = 0;

        virtual void RenderVertexArrays() = 0;
        virtual void DrawLine(const Vec3& pointA, const Vec3& pointB) = 0;
        virtual void DrawTriangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC) = 0;
        virtual void DrawRectangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD) = 0;

        virtual const ScreenMode& GetScreenMode() const = 0;
        virtual const std::string& GetWindowTitle() const = 0;

        virtual RenderBackendType GetRenderBackendType() const = 0;
    };



    /* ScreenMode inlines */
    inline ScreenMode::ScreenMode() : ScreenWidth(800), ScreenHeight(600), FrameLimit(60), FrameLimitEnabled(true), Fullscreen(false)
    {
    }
}