#pragma once

#include "include/Video/RenderBackend.h"
#include "include/Core/Utils/Types.h"

#include <string>

namespace JonsEngine
{
    /*
     * A default rendering backend that can be used when no proper backend is available during initialization
     */

    /* DummyRenderBackend definition */
    class DummyRenderBackend : public RenderBackend
    {
    public:
        DummyRenderBackend();
        ~DummyRenderBackend();

        void StartFrame();
        void EndFrame();

        bool SetupWindow();
        void CloseWindow();
        bool IsWindowOpened() const;
        uint16_t GetCurrentFPS() const;

        void SetFullscreen(bool fullscreen);
        void SetScreenResolution(const uint16_t width, const uint16_t height);
        void SetWindowTitle(const std::string& windowTitle);

        void RenderVertexArrays();
        void DrawLine(const Vec3& pointA, const Vec3& pointB);
        void DrawTriangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC);
        void DrawRectangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD);

        const ScreenMode& GetScreenMode() const;
        const std::string& GetWindowTitle() const;

        RenderBackendType GetRenderBackendType() const;


    private:
        ScreenMode mScreenMode;
        std::string mWindowTitle;
    };


    /* DummyRenderBackend inlines */
    inline DummyRenderBackend::DummyRenderBackend()                                                                                     { }
    inline DummyRenderBackend::~DummyRenderBackend()                                                                                    { }

    inline void DummyRenderBackend::StartFrame()                                                                                        { }
    inline void DummyRenderBackend::EndFrame()                                                                                          { }

    inline bool DummyRenderBackend::SetupWindow()                                                                                       { return false; }
    inline void DummyRenderBackend::CloseWindow()                                                                                       {               }
    inline bool DummyRenderBackend::IsWindowOpened() const                                                                              { return false; }
    inline uint16_t DummyRenderBackend::GetCurrentFPS() const                                                                           { return 0;     }

    inline void DummyRenderBackend::SetFullscreen(bool fullscreen)                                                                      { }
    inline void DummyRenderBackend::SetScreenResolution(const uint16_t width, const uint16_t height)                                    { }
    inline void DummyRenderBackend::SetWindowTitle(const std::string& windowTitle)                                                      { }

    inline void DummyRenderBackend::RenderVertexArrays()                                                                                { }
    inline void DummyRenderBackend::DrawLine(const Vec3& pointA, const Vec3& pointB)                                                    { }
    inline void DummyRenderBackend::DrawTriangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC)                            { }
    inline void DummyRenderBackend::DrawRectangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD)       { }

    inline const ScreenMode& DummyRenderBackend::GetScreenMode() const                                                                  { return mScreenMode;   }
    inline const std::string& DummyRenderBackend::GetWindowTitle() const                                                                { return mWindowTitle;  }

    inline RenderBackend::RenderBackendType DummyRenderBackend::GetRenderBackendType() const                                            { return NONE;  }
}