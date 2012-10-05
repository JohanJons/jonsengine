#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Video/RenderOpenGL.h"

namespace JonsEngine
{

	Engine::Engine() : mRunning(false), mLog(Globals::GetEngineLogger()), mRenderBackend(NULL), mMemoryAllocator(Globals::GetDefaultHeapAllocator())
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

			res &= InitializeModules();

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

	bool Engine::InitializeModules()
	{
		bool initResult = true;

		// Create and Initialize rendering backend
		if (!mRenderBackend)
		{
			if (mEngineSettings.GetRenderBackend() == OPENGL)
				mRenderBackend = mMemoryAllocator.AllocateObject<RenderOpenGL>();

			if (mRenderBackend->Init( mEngineSettings ))
				JONS_LOG_INFO(mLog, "Engine::InitializeModules(): Renderer initialized.")
			else
				JONS_LOG_ERROR(mLog, "Engine::InitializeModules(): Renderer failed to initialize!")
		}

		return (mRenderBackend != NULL);
	}

	bool Engine::DestroyModules()
	{
		// Destroy rendering backend
		if (mRenderBackend)
		{
			mMemoryAllocator.DeallocateObject<RenderBase>(mRenderBackend);
			mRenderBackend = NULL;

			JONS_LOG_INFO(mLog, "Engine::DestroyModules(): Destroyed Renderer.")
		}

		return (!mRenderBackend);
	}
}