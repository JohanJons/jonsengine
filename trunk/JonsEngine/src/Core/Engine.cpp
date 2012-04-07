#include "../../include/Core/Engine.h"

JonsEngine::Engine* JonsEngine::Engine::mEngine = NULL;

#ifdef ANDROID
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    JonsEngine::Engine* engine = JonsEngine::Engine::GetEngine();

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

	engine->SetJNIEnv(env);
    
    return JNI_VERSION_1_6;
}
#endif

namespace JonsEngine
{

	Engine::~Engine()
	{
		Destroy();
	}

	Engine::Engine() : mRunning(false), mRenderManager(NULL), mMemoryManager(NULL), mGameObjectManager(NULL),
						mLog(NULL)
	{
		#ifdef ANDROID
			mJNIEnv = NULL;
		#endif
	}

	Engine* Engine::GetEngine()
	{
		if (!mEngine)
		{
			mEngine = new Engine();
		}
		return mEngine;
	}

	bool Engine::Init(ILogManager* logger, IRenderManager* rendermgr, IMemoryManager* memmgr, IGameObjectManager* objmgr)
	{
		if (!mLog)
		{
			if (!logger)
				#ifdef ANDROID
					mLog = new LogManagerImpl(mJNIEnv);
				#else
					mLog = new LogManagerImpl();
				#endif
			else
				mLog = logger;
		}

		if (!mRenderManager)
		{
			if (!rendermgr)
				mRenderManager = new RenderManagerImpl(mLog);
			else
				mRenderManager = rendermgr;
		}

		if (!mMemoryManager)
		{
			if (!memmgr)
				mMemoryManager = new MemoryManagerImpl(mLog);
			else
				mMemoryManager = memmgr;
		}

		if (!mGameObjectManager)
		{
			if (!objmgr)
				mGameObjectManager = new GameObjectManagerImpl(mLog, mMemoryManager);
			else
				mGameObjectManager = objmgr;
		}

		return (mLog && mRenderManager && mMemoryManager && mGameObjectManager);
	}

	bool Engine::Start()
	{
		bool res = false;

		if (!mRunning)
		{
			res = true;
			res &= mLog->Start();
			res &= mRenderManager->Start();
			res &= mMemoryManager->Start();
			res &= mGameObjectManager->Start();

			if (res)
				mRunning = true;
			else
				mLog->LogError() << "Engine::Start(): Unable to start Engine!" << std::endl;
		}
		else 
			mLog->LogWarn() << "Engine::Start(): Engine already running" << std::endl;

		return res;
	}

	bool Engine::Stop()
	{
		bool res = false;

		if (mRunning)
		{
			res = true;
			res &= mRenderManager->Stop();
			res &= mMemoryManager->Stop();
			res &= mGameObjectManager->Stop();
			res &= mLog->Stop();

			if (res)
				mRunning = false;
			else
				mLog->LogError() << "Engine::Stop(): Unable to stop Engine!" << std::endl;
		}
		else 
			mLog->LogWarn() << "Engine::Stop(): Engine not running" << std::endl;

		return res;
	}

	bool Engine::Destroy()
	{
		bool res = true;

		if (mRunning)
			res &= Stop();

		res &= mRenderManager->Destroy();
		res &= mMemoryManager->Destroy();
		res &= mGameObjectManager->Destroy();
		res &= mLog->Destroy();

		if (mRenderManager)
		{
			delete mRenderManager;
			mRenderManager = NULL;
		}
		if (mMemoryManager)
		{
			delete mMemoryManager;
			mMemoryManager = NULL;
		}
		if (mGameObjectManager)
		{
			delete mGameObjectManager;
			mGameObjectManager = NULL;
		}
		if (mLog)
		{
			delete mLog;
			mLog = NULL;
		}

		return res;
	}

	bool Engine::isRunning()
	{
		return mRunning;
	}

	void Engine::Tick()
	{
		if (mRenderManager)
			mRenderManager->Tick();
		else
			mLog->LogError() << "Engine::Tick(): Component is NULL" << std::endl;
	}

	IRenderManager* Engine::GetRenderManager()
	{
		if (mRenderManager)
			return mRenderManager;
		else
		{
			mLog->LogError() << "Engine::GetRenderManager(): mRenderMgr is NULL!" << std::endl;
			return NULL;
		}
	}

	IMemoryManager* Engine::GetMemoryManager()
	{
		if (mMemoryManager)
			return mMemoryManager;
		else
		{
			mLog->LogError() << "Engine::GetMemoryManager(): mMemoryMgr is NULL!" << std::endl;
			return NULL;
		}
	}

	IGameObjectManager* Engine::GetGameObjectManager()
	{
		if (mGameObjectManager)
			return mGameObjectManager;
		else
		{
			mLog->LogError() << "Engine::GetGameObjectManager(): mGameObjectMgr is NULL!" << std::endl;
			return NULL;
		}
	}

	ILogManager* Engine::GetLogger()
	{
		if (mLog)
			return mLog;
		else
			return NULL;
	}


	#ifdef ANDROID
		void Engine::SetJNIEnv(JNIEnv* env)
		{
			mJNIEnv = env;
		}

		JNIEnv* Engine::GetJNIEnv()
		{
			return mJNIEnv;
		}
	#endif
}