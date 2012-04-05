#ifndef _ENGINE_H
#define _ENGINE_H

#ifdef ANDROID
#include "jni.h"
#endif

#include "../Render/RenderManagerImpl.h"
#include "Logging/LogManagerImpl.h"
#include "Memory/MemoryManagerImpl.h"
#include "GameObject/GameObjectManagerImpl.h"

namespace JonsEngine
{
	class Engine
	{
	public:
		static Engine* GetEngine();
		~Engine();

		bool Init(ILogManager* logger, IRenderManager* rendermgr, IMemoryManager* memmgr, IGameObjectManager* objfact);
		bool Start();
		bool Stop();
		bool Destroy();
		bool isRunning();
		void Tick();

		IRenderManager* GetRenderManager();
		IMemoryManager* GetMemoryManager();
		IGameObjectManager* GetGameObjectManager();
		ILogManager* GetLogger();

		#ifdef ANDROID
			void SetJNIEnv(JNIEnv* env);
			JNIEnv* GetJNIEnv();
		#endif

	private:
		Engine();
		static Engine* mEngine;

		bool mRunning;
		#ifdef ANDROID
			JNIEnv* mJNIEnv;
		#endif

		ILogManager* mLog;
		IRenderManager* mRenderManager;
		IMemoryManager* mMemoryManager;
		IGameObjectManager* mGameObjectManager;

	};

}

#endif
