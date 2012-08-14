#ifndef _ENGINE_H
#define _ENGINE_H

#ifdef ANDROID
#include "jni.h"
#endif

#include "include/Render/RenderManagerImpl.h"
#include "include/Core/Logging/LogManagerImpl.h"
#include "include/Core/Memory/MemoryManagerImpl.h"
#include "include/Core/Containers/Vector.h"
#include "include/Core/GameObject/GameObjectManagerImpl.h"
#include "include/Core/EngineSettings.h"


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
