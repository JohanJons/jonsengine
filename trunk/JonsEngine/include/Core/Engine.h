#pragma once

#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
	class IMemoryAllocator;
	class Logger;
	class RenderBase;

	class Engine
	{
	public:
		Engine();
		~Engine();

		bool Init();
		bool Destroy();
		bool isRunning();
		void Tick();

		EngineSettings& GetEngineSettings();

	private:
		bool InitializeModules();
		bool DestroyModules();

		bool mRunning;
		EngineSettings mEngineSettings;
		Logger& mLog;
		IMemoryAllocator& mMemoryAllocator;

		/* Modules */
		RenderBase* mRenderBackend;
	};

}
