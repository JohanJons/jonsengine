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

		bool Init(EngineSettings& initSettings);
		bool Start();
		bool Stop();
		bool Destroy();
		bool isRunning();
		void Tick();

		EngineSettings& GetEngineSettings();
		IMemoryManager* GetMemoryManager();
		IRenderManager* GetRenderManager();
		IGameObjectManager* GetGameObjectManager();
		ILogManager* GetLogger();

		#ifdef ANDROID
			void SetJNIEnv(JNIEnv* env);
			JNIEnv* GetJNIEnv();
		#endif

	private:
		bool mRunning;
		bool mInitialized;
		#ifdef ANDROID
			JNIEnv* mJNIEnv;
		#endif

		EngineSettings mEngineSettings;
		MemoryManagerImpl mMemoryManager;
		LogManagerImpl mLog;
		RenderManagerImpl mRenderManager;
		GameObjectManagerImpl mGameObjectManager;


	};

}

#endif
