#include "include/Core/Engine.h"

#include "interface/Core/Memory/IMemoryAllocator.h"

namespace JonsEngine
{

	Engine::~Engine()
	{
		Destroy();
	}

	Engine::Engine() : mRunning(false), mInitialized(false), mLog(EngineTag)
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
				JONS_LOG_ERROR(mLog, "Engine::Start(): Unable to start Engine!");
		}
		else 
			JONS_LOG_WARNING(mLog, "Engine::Start(): Engine already running");

		JONS_LOG_INFO(mLog, "-------- STARTING ENGINE --------");

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
				JONS_LOG_ERROR(mLog, "Engine::Stop(): Unable to stop Engine!");
		}
		else 
			JONS_LOG_WARNING(mLog, "Engine::Stop(): Engine not running");

		JONS_LOG_INFO(mLog, "-------- STOPPING ENGINE --------");

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