#pragma once

#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
	class IMemoryAllocator;
	class Logger;
	class RenderBase;
	class InputManager;

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

		RenderBase* GetRenderer() const;
		InputManager* GetInputManager() const;

	private:
		bool InitializeModules();
		bool DestroyModules();

		bool mRunning;
		EngineSettings mEngineSettings;
		Logger& mLog;
		IMemoryAllocator& mMemoryAllocator;

		/* Modules */
		RenderBase* mRenderBackend;
		InputManager* mInputManager;
	};

}
