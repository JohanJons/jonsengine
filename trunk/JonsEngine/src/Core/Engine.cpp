#include "include/Core/Engine.h"

#include "interface/Core/Logging/ILogger.h"
#include "interface/Core/Memory/IMemoryAllocator.h"

namespace JonsEngine
{

	Engine::~Engine()
	{
		Destroy();
	}

	Engine::Engine() : mRunning(false), mInitialized(false), mLog(Globals::GetDefaultLogger())
	{
		
	}

	bool Engine::Init()
	{
		bool res = true;

		res &= InitializeManagers();

		if (res)
			mInitialized = true;

		return res;
	}

	bool Engine::InitializeManagers()
	{
		bool ret = true;

		// Render

		// GameObject 

		return ret;
	}

	bool Engine::Start()
	{
		bool res = false;

		if (!mRunning)
		{
			res = true;

			if (res)
				mRunning = true;
			else
				mLog.LogError() << "Engine::Start(): Unable to start Engine!" << std::endl;
		}
		else 
			mLog.LogWarn() << "Engine::Start(): Engine already running" << std::endl;

		return res;
	}

	bool Engine::Stop()
	{
		bool res = false;

		if (mRunning)
		{
			res = true;

			if (res)
				mRunning = false;
			else
				mLog.LogError() << "Engine::Stop(): Unable to stop Engine!" << std::endl;
		}
		else 
			mLog.LogWarn() << "Engine::Stop(): Engine not running" << std::endl;

		return res;
	}

	bool Engine::Destroy()
	{
		bool res = true;

		if (mRunning)
			res &= Stop();

		res &= DestroyManagers();

		if (res)
			mInitialized = false;

		return res;
	}

	bool Engine::DestroyManagers()
	{
		bool ret = true;

		return ret;
	}

	bool Engine::isRunning()
	{
		return mRunning;
	}

	void Engine::Tick()
	{

	}

	EngineSettings& Engine::GetEngineSettings()
	{
		return mEngineSettings;
	}
}