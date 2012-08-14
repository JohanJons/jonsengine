#ifndef _ENGINE_H
#define _ENGINE_H

#ifdef ANDROID
#include "jni.h"
#endif

#include "../Render/RenderManagerImpl.h"
#include "Logging/LogManagerImpl.h"
#include "Memory/MemoryManagerImpl.h"
#include "Containers/Vector.h"
#include "GameObject/GameObjectManagerImpl.h"
#include "EngineSettings.h"


namespace JonsEngine
{
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
		IMemoryManager* const GetMemoryManager();
		IRenderManager* const GetRenderManager();
		IGameObjectManager* const GetGameObjectManager();
		ILogManager* const GetLogger();

		#ifdef ANDROID
			static JNIEnv* mJNIEnv;
		#endif

	private:
		bool mRunning;
		bool mInitialized;

		LogManagerImpl mLog;
		EngineSettings mEngineSettings;
		MemoryManagerImpl mMemoryManager;
		RenderManagerImpl mRenderManager;
		GameObjectManagerImpl mGameObjectManager;


	};

}

#endif
