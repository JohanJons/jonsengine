#include "include/Video/OpenGL/OpenGLRenderBackend.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glfw.h"
#include <glm/gtc/type_ptr.hpp>
#include <exception>


namespace JonsEngine
{

    OpenGLRenderBackend::OpenGLRenderBackend(const EngineSettings& engineSettings) : mLogger(Globals::GetVideoLogger()), mWindowTitle("JonsEngine Game"), mStartFrameTime(0), mLastFrameTime(0), mThisFrameTime(0)
    {
        GLenum glfwErr = glfwInit();
        if (glfwErr != GL_TRUE)
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderBackend::OpenGLRenderBackend(): Unable to initialize GLFW!");
            throw std::exception("OpenGLRenderBackend::OpenGLRenderBackend(): Unable to initialize GLFW!");
        }

        // setup a forward-compatible context with openGL 3.3
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        SetupWindow(engineSettings.ScreenMode);
        SetWindowTitle(engineSettings.WindowTitle);

        // Initialize GLEW for fetching openGL
        // 'glewExperimental = GL_TRUE' needed to initialize openGL core profile; see GLEW doc
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();

        if (!GLEW_VERSION_3_3)
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderBackend::OpenGLRenderBackend(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
            throw std::exception("OpenGLRenderBackend::OpenGLRenderBackend(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
        }

        // openGL context should be ready now
        glGenBuffers(1, &mVBO_VertexShader);
        glGenBuffers(1, &mVBO_FragmentShader);
        glGenVertexArrays(1, &mVAO);
        glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    }
        
    OpenGLRenderBackend::~OpenGLRenderBackend()
    {
        CloseWindow();

        glfwTerminate();
    }

    bool OpenGLRenderBackend::SetupWindow(const ScreenMode& screenMode)
    {
        if (IsWindowOpened())
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderBackend::SetupWindow(): Window already opened, close it first.");
            return false;
        }

        if (glfwOpenWindow(screenMode.ScreenWidth, screenMode.ScreenHeight, 0, 0 , 0, 0, 0, 0, screenMode.Fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) != GL_TRUE)
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderBackend::SetupWindow(): Failed to create main screen!")
            return false;
        }
        else 
        {
            mScreenMode = screenMode;
            return true;
        }
    }

    void OpenGLRenderBackend::CloseWindow()
    {
        if (IsWindowOpened())
            glfwCloseWindow();
        else
            JONS_LOG_WARNING(mLogger, "OpenGLRenderBackend::CloseWindow(): Window not opened.");
    }
        
    bool OpenGLRenderBackend::IsWindowOpened() const
    {
        return (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE);
    }

    uint16_t OpenGLRenderBackend::GetCurrentFPS() const
    {
        return static_cast<uint16_t>(1 / (mThisFrameTime - mLastFrameTime));
    }

    void OpenGLRenderBackend::SetFullscreen(bool fullscreen)
    {
        if (!IsWindowOpened())
        {
            JONS_LOG_WARNING(mLogger, "OpenGLRenderBackend::SetFullscreen(): Window not opened");
            return;
        }

        if (mScreenMode.Fullscreen != fullscreen)
        {
            // need to tear down window and reset opengl context with GLFW..
            CloseWindow();
            
            mScreenMode.Fullscreen = fullscreen;

            SetupWindow(mScreenMode);
        }
    }
        
    void OpenGLRenderBackend::SetScreenResolution(const uint16_t width, const uint16_t height)
    {
        if (!IsWindowOpened())
        {
            JONS_LOG_WARNING(mLogger, "OpenGLRenderBackend::SetScreenResolution(): Window not opened");
            return;
        }

        if (mScreenMode.ScreenWidth != width ||
            mScreenMode.ScreenHeight != height)
        {
            glfwSetWindowSize(width, height);
            glViewport(0, 0, (GLsizei) width, (GLsizei)height);

            mScreenMode.ScreenWidth = width;
            mScreenMode.ScreenHeight = height;
        }
    }
        
    void OpenGLRenderBackend::SetWindowTitle(const std::string& windowTitle)
    {
        if (!IsWindowOpened())
        {
            JONS_LOG_WARNING(mLogger, "OpenGLRenderBackend::SetWindowTitle(): Window not opened");
            return;
        }

        if (mWindowTitle.compare(windowTitle) != 0)
        {
            glfwSetWindowTitle(windowTitle.c_str());

            mWindowTitle = windowTitle;
        }
    }

    void OpenGLRenderBackend::RenderVertexArrays()
    {
        glBindVertexArray(mVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

    void OpenGLRenderBackend::DrawLine(const Vec3& pointA, const Vec3& pointB)
    {

    }
        
    void OpenGLRenderBackend::DrawTriangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC)
    {
        const float vertexPositions[] = {
            0.75f, 0.75f, 0.0f,
            0.75f, -0.75f, 0.0f,
            -0.75f, -0.75f, 0.0f,
        };


        // vertex shader
        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO_VertexShader);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(0);

        // unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        PrimitiveInfo primInfo;
        primInfo.Mode = GL_TRIANGLES;
        primInfo.Count = 3;
        mPrimitiveInfo.push_back(primInfo);
    }
        
    void OpenGLRenderBackend::DrawRectangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD)
    {
        
    }
        
    void OpenGLRenderBackend::StartFrame()
    {
        mLastFrameTime = mThisFrameTime;

        if (mScreenMode.FrameLimitEnabled)
            mStartFrameTime = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
        
    void OpenGLRenderBackend::EndFrame()
    {
        if (mScreenMode.FrameLimitEnabled)
        {
            mThisFrameTime = glfwGetTime();

            const double endFrameTime = glfwGetTime() - mStartFrameTime;
            const double frameTime = 1.0f / mScreenMode.FrameLimit;

            if (endFrameTime < frameTime)
                glfwSleep(frameTime - endFrameTime);
        }

        glfwSwapBuffers();
    }
}