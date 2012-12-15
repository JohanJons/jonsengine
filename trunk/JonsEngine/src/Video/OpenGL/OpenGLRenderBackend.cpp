#include "include/Video/OpenGL/OpenGLRenderBackend.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glfw.h"
#include <glm/gtc/type_ptr.hpp>
#include <exception>


namespace JonsEngine
{
    OpenGLRenderBackend::OpenGLRenderBackend(const EngineSettings& engineSettings) : mLogger(Logger::GetVideoLogger()), mWindowTitle("JonsEngine Game"), mStartFrameTime(0), mLastFrameTime(0), mThisFrameTime(0)
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

        mScreenMode = engineSettings.ScreenMode;
        SetupWindow();
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

        glEnable(GL_CULL_FACE);
	    glCullFace(GL_BACK);
	    glFrontFace(GL_CCW);

        // openGL context should be ready now
        glGenBuffers(1, &mVBO_VertexShader);
        glGenBuffers(1, &mIndexBuffer);
        glGenVertexArrays(1, &mVAO);
    }
        
    OpenGLRenderBackend::~OpenGLRenderBackend()
    {
        CloseWindow();

        glfwTerminate();
    }

    void OpenGLRenderBackend::StartFrame()
    {
        mLastFrameTime = mThisFrameTime;

        if (mScreenMode.FrameLimitEnabled)
            mStartFrameTime = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

    bool OpenGLRenderBackend::SetupWindow()
    {
        if (IsWindowOpened())
            CloseWindow();

        if (!glfwOpenWindow(mScreenMode.ScreenWidth, mScreenMode.ScreenHeight, 0, 0 , 0, 0, 0, 0, mScreenMode.Fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) != GL_TRUE)
        {
            SetWindowTitle(mWindowTitle);
            UpdateViewport();

            return true;
        }
        else 
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderBackend::SetupWindow(): Failed to create main window!")
            return false;
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
        if (mScreenMode.Fullscreen != fullscreen && IsWindowOpened())
        {
            // need to tear down window and reset opengl context with GLFW..
            SetupWindow();
        }

        mScreenMode.Fullscreen = fullscreen;
    }
        
    void OpenGLRenderBackend::SetScreenResolution(const uint16_t width, const uint16_t height)
    {
        if (IsWindowOpened() && (mScreenMode.ScreenWidth != width || mScreenMode.ScreenHeight != height))
            glfwSetWindowSize(width, height);

        mScreenMode.ScreenWidth = width;
        mScreenMode.ScreenHeight = height;

        UpdateViewport();
    }
        
    void OpenGLRenderBackend::SetWindowTitle(const std::string& windowTitle)
    {
        if (IsWindowOpened() && mWindowTitle.compare(windowTitle) != 0)
            glfwSetWindowTitle(windowTitle.c_str());

        mWindowTitle = windowTitle;
    }

    void OpenGLRenderBackend::RenderVertexArrays()
    {
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }

    void OpenGLRenderBackend::DrawLine(const Vec3& pointA, const Vec3& pointB)
    {

    }
        
    void OpenGLRenderBackend::DrawTriangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC)
    {
        const float vertexPositions[] =
        {
            // front vertices
            -1.0f, -1.5f, -5.0f,
            -1.5f, -1.5f, -5.0f,
            -1.5f, -1.0f, -5.0f,
            -1.0f, -1.0f, -5.0f,

            // back vertices
            -1.0f, -1.5f, -5.5f,
            -1.5f, -1.5f, -5.5f,
            -1.5f, -1.0f, -5.5f,
            -1.0f, -1.0f, -5.5f
        };

 

        const GLshort indexData[] =
        {
            // back
            5, 4, 7,
            5, 7, 6,

            // right
            1, 5, 6,
            1, 6, 2,

            // left
            0, 4, 7,
            0, 7, 3,

            // top
            5, 4, 0,
            5, 0, 1,

            // bottom
            6, 7, 3,
            6, 3, 2,

            // front
            1, 0, 3,
            1, 3, 2
        };

        // buffer data
        glBindBuffer(GL_ARRAY_BUFFER, mVBO_VertexShader);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


        // setup VAO
        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO_VertexShader);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
        
    void OpenGLRenderBackend::DrawRectangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD)
    {
        
    }

    void OpenGLRenderBackend::UpdateViewport()
    {
        if (IsWindowOpened())
        {
            glViewport(0, 0, (GLsizei) mScreenMode.ScreenWidth, (GLsizei)mScreenMode.ScreenHeight);
        }
    }
}