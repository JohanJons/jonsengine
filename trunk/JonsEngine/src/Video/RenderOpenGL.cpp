#include "include/Video/RenderOpenGL.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glew.h"
#include "GL/glfw.h"

namespace JonsEngine
{

	RenderOpenGL::RenderOpenGL() : mLogger(Globals::GetVideoLogger())
	{
	}
		
	RenderOpenGL::~RenderOpenGL()
	{
		if (mRunning)
			Destroy();
	}

		
	bool RenderOpenGL::Init(const EngineSettings& engineSettings)
	{
		bool ret = true;

		ret &= InitializeGLFW();

		ret &= SetupWindow(engineSettings.GetScreenMode());
		SetWindowTitle(engineSettings.GetWindowTitle());

		ret &= InitializeGLEW();

		if (ret)
			mRunning = true;
		else
			Destroy();

		return ret;
	}
		
	void RenderOpenGL::Destroy()
	{
		mRunning = false;

		CloseWindow();

		glfwTerminate();
	}

	bool RenderOpenGL::SetupWindow(const ScreenMode& screenMode)
	{
		if (IsWindowOpened())
		{
			JONS_LOG_ERROR(mLogger, "RenderOpenGL::SetupWindow(): Window already opened, close it first.");
			return false;
		}

		if (glfwOpenWindow(screenMode.ScreenWidth, screenMode.ScreenHeight, 0, 0 , 0, 0, 0, 0, screenMode.Fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) != GL_TRUE)
		{
			JONS_LOG_ERROR(mLogger, "RenderOpenGL::SetupWindow(): Failed to create main screen!")
			return false;
		}
		else 
		{
			mScreenMode = screenMode;
			return true;
		}
	}

	void RenderOpenGL::CloseWindow()
	{
		if (IsWindowOpened())
			glfwCloseWindow();
		else
			JONS_LOG_WARNING(mLogger, "RenderOpenGL::CloseWindow(): Window not opened.");
	}
		
	bool RenderOpenGL::IsWindowOpened() const
	{
		return (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE);
	}

	void RenderOpenGL::SetFullscreen(bool fullscreen)
	{
		if (!IsWindowOpened())
		{
			JONS_LOG_WARNING(mLogger, "RenderOpenGL::SetFullscreen(): Window not opened");
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
		
	void RenderOpenGL::SetScreenResolution(const uint16_t width, const uint16_t height)
	{
		if (!IsWindowOpened())
		{
			JONS_LOG_WARNING(mLogger, "RenderOpenGL::SetScreenResolution(): Window not opened");
			return;
		}

		if (mScreenMode.ScreenWidth != width ||
			mScreenMode.ScreenHeight != height)
		{
			glfwSetWindowSize(width, height);

			mScreenMode.ScreenWidth = width;
			mScreenMode.ScreenHeight = height;
		}
	}
		
	void RenderOpenGL::SetWindowTitle(const std::string& windowTitle)
	{
		if (!IsWindowOpened())
		{
			JONS_LOG_WARNING(mLogger, "RenderOpenGL::SetWindowTitle(): Window not opened");
			return;
		}

		if (mWindowTitle.compare(windowTitle) != 0)
		{
			glfwSetWindowTitle(windowTitle.c_str());

			mWindowTitle = windowTitle;
		}
	}

		
	void RenderOpenGL::StartFrame()
	{

	}
		
	void RenderOpenGL::EndFrame()
	{
		glfwSwapBuffers();
	}

	bool RenderOpenGL::InitializeGLFW()
	{
		// Initialize GLFW
		GLenum glfwErr = glfwInit();

		if (glfwErr != GL_TRUE)
		{
			JONS_LOG_ERROR(mLogger, "RenderOpenGL::Init(): Unable to initialize GLFW!")
			return false;
		}

		// setup a forward-compatible context with openGL 3.3
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			
		return true;
	}
		
	bool RenderOpenGL::InitializeGLEW()
	{
		// Initialize GLEW for fetching openGL
		// 'glewExperimental = GL_TRUE' needed to initialize openGL core profile; see GLEW doc
		glewExperimental = GL_TRUE;
		GLenum glewErr = glewInit();

		if (!GLEW_VERSION_3_3)
		{
			JONS_LOG_ERROR(mLogger, "RenderOpenGL::Init(): Minimum OpenGL driver (OpenGL 3.3) not supported!")
			return false;
		}
		else
			return true;
	}
}