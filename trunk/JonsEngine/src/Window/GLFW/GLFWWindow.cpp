#include "include/Window/GLFW/GLFWWindow.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glew.h"
#include "GL/glfw.h"
#include <exception>

namespace JonsEngine
{
    static GLFWWindow* gGLFWWindowInstance = NULL;

    GLFWWindow::GLFWWindow(const EngineSettings& engineSettings) : mLogger(Logger::GetWindowLogger())
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

        mScreenMode = engineSettings.mScreenMode;
        SetupWindow();
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
        CloseWindow();

        glfwTerminate();

        gGLFWWindowInstance = NULL;
    }

        
    void GLFWWindow::StartFrame()
    {
        mLastFrameTime = mThisFrameTime;

        if (mScreenMode.mFrameLimitEnabled)
            mStartFrameTime = glfwGetTime();
    }
        
    void GLFWWindow::EndFrame()
    {
        if (mScreenMode.mFrameLimitEnabled)
        {
            mThisFrameTime = glfwGetTime();

            const double endFrameTime = glfwGetTime() - mStartFrameTime;
            const double frameTime = 1.0f / mScreenMode.mFrameLimit;

            if (endFrameTime < frameTime)
                glfwSleep(frameTime - endFrameTime);
        }

        glfwSwapBuffers();
    }

        
    bool GLFWWindow::SetupWindow()
    {
        if (IsWindowOpened())
            CloseWindow();

        if (!glfwOpenWindow(mScreenMode.mScreenWidth, mScreenMode.mScreenHeight, 0, 0 , 0, 0, 0, 0, mScreenMode.mFullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) != GL_TRUE)
        {
            SetWindowTitle(mWindowTitle);
            UpdateViewport();

            return true;
        }
        else 
        {
            JONS_LOG_ERROR(mLogger, "GLFWWindow::SetupWindow(): Failed to create main window!")
            return false;
        }
    }
        
    void GLFWWindow::CloseWindow()
    {
         if (IsWindowOpened())
            glfwCloseWindow();
        else
            JONS_LOG_WARNING(mLogger, "GLFWWindow::CloseWindow(): Window not opened.");
    }
        
    bool GLFWWindow::IsWindowOpened() const
    {
         return (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE);
    }
        
    uint16_t GLFWWindow::GetCurrentFPS() const
    {
        return static_cast<uint16_t>(1 / (mThisFrameTime - mLastFrameTime));
    }

        
    void GLFWWindow::SetFullscreen(bool fullscreen)
    {
        if (mScreenMode.mFullscreen != fullscreen && IsWindowOpened())
        {
            // need to tear down window and reset opengl context with GLFW..
            SetupWindow();
        }

        mScreenMode.mFullscreen = fullscreen;
    }
        
    void GLFWWindow::SetScreenResolution(const uint16_t width, const uint16_t height)
    {
        if (IsWindowOpened() && (mScreenMode.mScreenWidth != width || mScreenMode.mScreenHeight != height))
            glfwSetWindowSize(width, height);

        mScreenMode.mScreenWidth = width;
        mScreenMode.mScreenHeight = height;

        UpdateViewport();
    }
        
    void GLFWWindow::SetWindowTitle(const std::string& windowTitle)
    {
        if (IsWindowOpened() && mWindowTitle.compare(windowTitle) != 0)
            glfwSetWindowTitle(windowTitle.c_str());

        mWindowTitle = windowTitle;
    }

        
    void GLFWWindow::UpdateViewport()
    {
        if (IsWindowOpened())
        {
            glViewport(0, 0, (GLsizei) mScreenMode.mScreenWidth, (GLsizei)mScreenMode.mScreenHeight);
        }
    }
        
    void GLFWWindow::glfwOnWindowChanged(int width, int height)
    {
        if (gGLFWWindowInstance)
        {
            gGLFWWindowInstance->mScreenMode.mScreenWidth = width;
            gGLFWWindowInstance->mScreenMode.mScreenHeight = height;

            gGLFWWindowInstance->UpdateViewport();
        }
    }
}