#pragma once

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
	class ILogger;

	class Engine
	{
	public:
		Engine();
		~Engine();

		bool Init();
		bool Start();
		bool Stop();
		bool Destroy();
		bool isRunning();
		void Tick();

		EngineSettings& GetEngineSettings();

	private:
		bool InitializeManagers();
		bool DestroyManagers();

		bool mRunning;
		bool mInitialized;
		EngineSettings mEngineSettings;
		Logger mLog;
	};

}
