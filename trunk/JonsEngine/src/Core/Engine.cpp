#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Video/OpenGL/RenderOpenGL.h"
#include "include/Input/InputManager.h"

#include "GL/glfw.h"

namespace JonsEngine
{

	Engine::Engine() : mRunning(false), mLog(Globals::GetEngineLogger()), mMemoryAllocator(Globals::GetDefaultHeapAllocator()), mRenderBackend(NULL), mInputManager(NULL)
	{

	}

	Engine::~Engine()
	{
		if (mRunning)
			Destroy();
	}

	bool Engine::Init()
	{
		if (!mRunning)
		{
			bool res = true;

			JONS_LOG_INFO(mLog, "-------- INITIALIZING ENGINE --------")

			// Setup environment
			res &= SetupEnvironment();

			// intialize subsystems
			if (res)
				res &= InitializeModules();
			else
				JONS_LOG_ERROR(mLog, "Engine::Init(): Failed to setup environment")


			if (res && !mRunning)
			{
				mRunning = true;
				JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
			}

			return res;
		}
		else
		{
			JONS_LOG_WARNING(mLog, "Engine::Init(): Engine already initialized!")

			return false;
		}
	}

	bool Engine::Destroy()
	{
		if (mRunning)
		{
			bool res = true;

			JONS_LOG_INFO(mLog, "-------- SHUTTING DOWN ENGINE --------")

			res &= DestroyModules();

			if (res && mRunning)
			{
				mRunning = false;
				JONS_LOG_INFO(mLog, "-------- ENGINE DESTROYED --------")
			}

			return res;
		}
		else
		{
			JONS_LOG_WARNING(mLog, "Engine::Destroy(): Engine not initialized!")

			return false;
		}
	}

	bool Engine::isRunning()
	{
		return mRunning;
	}

	void Engine::Tick()
	{
		if (mRunning)
		{
			mRenderBackend->StartFrame();

			mRenderBackend->RenderVertexArrays();

			mRenderBackend->EndFrame();
		}
	}

	EngineSettings& Engine::GetEngineSettings()
	{
		return mEngineSettings;
	}

	RenderBase* Engine::GetRenderer() const
	{
		return mRenderBackend;
	}

	InputManager* Engine::GetInputManager() const
	{
		return mInputManager;
	}

	bool Engine::SetupEnvironment()
	{
		#if defined _WIN32 || _WIN64
			return InitializeGLFW();
		#else
			return false;
		#endif
	}

	bool Engine::InitializeGLFW()
	{
		GLenum glfwErr = glfwInit();
		if (glfwErr != GL_TRUE)
		{
			JONS_LOG_ERROR(mLog, "Engine::Init(): Unable to initialize GLFW!")
			return false;
		}
		else
			return true;
	}

	bool Engine::InitializeModules()
	{
		bool initResult = true;

		// Create and Initialize rendering backend
		if (!mRenderBackend)
		{
			if (mEngineSettings.GetRenderBackend() == OPENGL)
				mRenderBackend = mMemoryAllocator.AllocateObject<RenderOpenGL>();

			if (!mRenderBackend->Init(mEngineSettings))
			{
				JONS_LOG_ERROR(mLog, "Engine::InitializeModules(): Renderer failed to initialize!")
				return false;
			}
		}

		if (!mInputManager)
		{
			mInputManager = mMemoryAllocator.AllocateObject<InputManager>();
			if (!mInputManager->Init(mEngineSettings))
			{
				JONS_LOG_ERROR(mLog, "Engine::InitializeModules(): InputManager failed to initialize!")
				return false;
			}
		}

		return (mRenderBackend != NULL && mInputManager != NULL);
	}

	bool Engine::DestroyModules()
	{
		// Destroy rendering backend
		if (mRenderBackend)
		{
			mMemoryAllocator.DeallocateObject<RenderBase>(mRenderBackend);
			mRenderBackend = NULL;
		}

		// Destroy input manager backend
		if (mInputManager)
		{
			mMemoryAllocator.DeallocateObject<InputManager>(mInputManager);
			mInputManager = NULL;
		}

		return (!mRenderBackend && !mInputManager);
	}
}