#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Utils/Types.h"

namespace JonsEngine
{
    struct EngineSettings;
    struct ScreenMode;
    class IMemoryAllocator;
    class RenderBackend;

    class Renderer
    {
    public:
        Renderer(const EngineSettings& engineSettings);
        ~Renderer();

        void StartFrame();
        void EndFrame();

        bool SetupWindow(const ScreenMode& screenMode);
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


    private:
        RenderBackend* CreateBackend(const EngineSettings& engineSettings);

        IMemoryAllocator& mMemoryAllocator;
        RenderBackend* mRenderBackend;
    };
}