#ifndef _ENGINE_H
#define _ENGINE_H

#ifdef ANDROID
#include "jni.h"
#endif

#include "include/Core/EngineSettings.h"
#include "include/Core/EngineCore.h"

#include "include/Render/RenderManagerImpl.h"
#include "include/Core/Logging/LogManagerImpl.h"
#include "include/Core/Memory/MemoryManagerImpl.h"
#include "include/Core/GameObject/GameObjectManagerImpl.h"

#include "include/Core/Threading/ThreadingFactory.h"


namespace JonsEngine
{
	class Engine : EngineCore
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

		/* Managers */
		EngineSettings& GetEngineSettings();
		IMemoryManager& GetMemoryManager();
		IRenderManager& GetRenderManager();
		IGameObjectManager& GetGameObjectManager();
		ILogManager& GetLogger();

		/* Factories */
		IThreadingFactory& GetThreadingFactory();


		#ifdef ANDROID
			static JNIEnv* mJNIEnv;
		#endif

	private:
		bool InitializeManagers();
		bool InitializeFactories();
		bool DestroyManagers();
		bool DestroyFactories();

		bool mRunning;
		bool mInitialized;
		EngineSettings mEngineSettings;

		/* Managers */
		RenderManagerImpl mRenderManager;
		GameObjectManagerImpl mGameObjectManager;

		/* Factories */
		ThreadingFactory mThreadingFactory;

	};

}

#endif
