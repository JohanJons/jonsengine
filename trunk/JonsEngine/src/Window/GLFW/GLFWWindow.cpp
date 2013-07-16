#include "include/Window/GLFW/GLFWWindow.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glew.h"
#include "GL/glfw.h"
#include <exception>

namespace JonsEngine
{
    static GLFWWindow* gGLFWWindowInstance = NULL;

    GLFWWindow::GLFWWindow(const EngineSettings& engineSettings) : mLogger(Logger::GetWindowLogger()), mFrameLimit(engineSettings.mFrameLimit), mStartFrameTime(0.0f), mLastFrameTime(0.0f), mThisFrameTime(0.0f),
                                                                   mFullscreen(engineSettings.mFullscreen), mFOV(engineSettings.mFOV), mWindowTitle("Game"), mMSAA(engineSettings.mMSAA)
    {
        GLenum glfwErr = glfwInit();
        if (glfwErr != GL_TRUE)
        {
            JONS_LOG_ERROR(mLogger, "GLFWWindow::GLFWWindow(): Unable to initialize GLFW!");
            throw std::runtime_error("GLFWWindow::GLFWWindow(): Unable to initialize GLFW!");
        }
        
        // setup a forward-compatible context with openGL 3.3
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        SetupWindow(engineSettings.mWindowWidth, engineSettings.mWindowHeight);
        SetWindowTitle(engineSettings.mWindowTitle);

        // Initialize GLEW for fetching openGL
        // 'glewExperimental = GL_TRUE' needed to initialize openGL core profile; see GLEW doc
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();

        if (!GLEW_VERSION_3_3)
        {
            JONS_LOG_ERROR(mLogger, "GLFWWindow::GLFWWindow(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
            throw std::runtime_error("GLFWWindow::GLFWWindow(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
        }

        gGLFWWindowInstance = this;
        glfwSetWindowSizeCallback(glfwOnWindowChanged);
    }
        
    GLFWWindow::~GLFWWindow()
    {
        glfwCloseWindow();
        glfwTerminate();

        gGLFWWindowInstance = NULL;
    }

        
    void GLFWWindow::StartFrame()
    {
        mLastFrameTime = mThisFrameTime;

        if (mFrameLimit)
            mStartFrameTime = glfwGetTime();
    }
        
    void GLFWWindow::EndFrame()
    {
        if (mFrameLimit)
        {
            mThisFrameTime = glfwGetTime();

            const double endFrameTime = glfwGetTime() - mStartFrameTime;
            const double frameTime = 1.0f / mFrameLimit;

            if (endFrameTime < frameTime)
                glfwSleep(frameTime - endFrameTime);
        }

        glfwSwapBuffers();
    }

    
    void GLFWWindow::SetFrameLimit(const uint16_t frameLimit)
    {
        mFrameLimit = frameLimit;
    }
        
    uint16_t GLFWWindow::GetCurrentFPS() const
    {
        return static_cast<uint16_t>(1 / (mThisFrameTime - mLastFrameTime));
    }

        
    void GLFWWindow::SetFullscreen(bool fullscreen)
    {
        uint32_t screenWidth  = GetScreenWidth();
        uint32_t screenHeight = GetScreenWidth();

        if (mFullscreen != fullscreen)
            SetupWindow(screenWidth, screenHeight); // need to tear down window and reset opengl context with GLFW..

        mFullscreen = fullscreen;
    }
        
    void GLFWWindow::SetScreenResolution(const uint32_t width, const uint32_t height)
    {
        if (mFullscreen)
            return;

        uint32_t screenWidth  = GetScreenWidth();
        uint32_t screenHeight = GetScreenWidth();

        if (screenWidth != width || screenHeight != height)
            glfwSetWindowSize(width, height);
    }
        
    void GLFWWindow::SetWindowTitle(const std::string& windowTitle)
    {
        if (mWindowTitle.compare(windowTitle) != 0)
            glfwSetWindowTitle(windowTitle.c_str());

        mWindowTitle = windowTitle;
    }

    void GLFWWindow::SetFOV(const float FOV)
    {
        mFOV = FOV;
    }


    uint32_t GLFWWindow::GetScreenWidth() const
    {
        int width = 0, height = 0;
        glfwGetWindowSize(&width, &height);

        return width;
    }
        
    uint32_t GLFWWindow::GetScreenHeight() const
    {
        int width = 0, height = 0;
        glfwGetWindowSize(&width, &height);

        return height;
    }

        
    bool GLFWWindow::SetupWindow(const uint32_t width, const uint32_t height)
    {
        glfwOpenWindowHint(GLFW_FSAA_SAMPLES, mMSAA);
        if (glfwOpenWindow(width, height, 0, 0 , 0, 8, 24, 8, mFullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) != GL_TRUE)
        {
            JONS_LOG_ERROR(mLogger, "GLFWWindow::SetupWindow(): Failed to create main window!")
            return false;
        }

        SetWindowTitle(mWindowTitle);
        glViewport(0, 0, (GLsizei) width, (GLsizei)height);

        return true;
    }
        
    void GLFWWindow::glfwOnWindowChanged(int width, int height)
    {
        if (gGLFWWindowInstance)
            glViewport(0, 0, (GLsizei) width, (GLsizei)height);
    }
}