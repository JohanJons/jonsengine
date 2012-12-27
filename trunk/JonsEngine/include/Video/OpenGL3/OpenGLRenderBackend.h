#pragma once

#include "include/Video/RenderBackend.h"
#include "include/Core/Containers/vector.h"

#include "GL/glew.h"
#include <string>

namespace JonsEngine
{
    struct EngineSettings;
    class Logger;


    /* OpenGLRenderBackend definition */
    class OpenGLRenderBackend : public RenderBackend
    {
    public:
        OpenGLRenderBackend(const EngineSettings& engineSettings);
        ~OpenGLRenderBackend();

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
        void UpdateViewport();
        static void glfwOnWindowChanged(int width, int height);

        Logger& mLogger;
        std::string mWindowTitle;
        ScreenMode mScreenMode;

        // FPS
        double mStartFrameTime;
        double mLastFrameTime;
        double mThisFrameTime;

        // rendering 
        GLuint mVBO_VertexShader;
        GLuint mIndexBuffer;
        GLuint mVAO;
        GLuint mVBO_VertexShader2;
        GLuint mIndexBuffer2;
        GLuint mVAO2;
        vector<Vec3> mVertices;
    };


    /* OpenGLRenderBackend inlines */
    inline const ScreenMode& OpenGLRenderBackend::GetScreenMode() const                             { return mScreenMode;   }
    inline const std::string& OpenGLRenderBackend::GetWindowTitle() const                           { return mWindowTitle;  }
    inline RenderBackend::RenderBackendType OpenGLRenderBackend::GetRenderBackendType() const       { return OPENGL;        }
}