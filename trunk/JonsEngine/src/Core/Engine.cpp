#include "include/Core/Engine.h"

#ifdef ANDROID
JNIEnv* JonsEngine::Engine::mJNIEnv = NULL;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

	JonsEngine::Engine::mJNIEnv = env;
    
    return JNI_VERSION_1_6;
}
#endif

namespace JonsEngine
{

	Engine::~Engine()
	{
		Destroy();
	}

	Engine::Engine() : mRunning(false), mInitialized(false)
	{
		
	}

	bool Engine::Init()
	{
		bool res = true;

		// MemoryMgr must be the first one to be initialized as all other subsystems are dependant on it for memory allocation that might occur in their initialization and onwards
		res &= mMemoryManager.Init(&mLog);

		// Only proceed if MemoryMgr is valid
		if (res)
		{
			// Log
			#ifdef ANDROID
				res &= mLog.Init(mEngineSettings.GetLogToFile(), mEngineSettings.GetLogToSTDOut(), mEngineSettings.GetLogToFileLocation(), &mMemoryManager, mJNIEnv);
			#else
				res &= mLog.Init(mEngineSettings.GetLogToFile(), mEngineSettings.GetLogToSTDOut(), mEngineSettings.GetLogToFileLocation(), &mMemoryManager);
			#endif

			// Render
			res &= mRenderManager.Init(&mLog,&mMemoryManager);

			// GameObject 
			res &= mGameObjectManager.Init(&mLog, &mMemoryManager);

			mInitialized = true;
		}

		return res;
	}

	bool Engine::Start()
	{
		bool res = false;

		if (!mRunning)
		{
			res = true;
			res &= mLog.Start();
			res &= mRenderManager.Start();
			res &= mMemoryManager.Start();
			res &= mGameObjectManager.Start();

			if (res)
				mRunning = true;
			else
				mLog.LogError() << "Engine::Start(): Unable to start Engine!" << std::endl;
		}
		else 
			mLog.LogWarn() << "Engine::Start(): Engine already running" << std::endl;

		return res;
	}

	bool Engine::Stop()
	{
		bool res = false;

		if (mRunning)
		{
			res = true;
			res &= mRenderManager.Stop();
			res &= mMemoryManager.Stop();
			res &= mGameObjectManager.Stop();
			res &= mLog.Stop();

			if (res)
				mRunning = false;
			else
				mLog.LogError() << "Engine::Stop(): Unable to stop Engine!" << std::endl;
		}
		else 
			mLog.LogWarn() << "Engine::Stop(): Engine not running" << std::endl;

		return res;
	}

	bool Engine::Destroy()
	{
		bool res = true;

		if (mRunning)
			res &= Stop();

		res &= mRenderManager.Destroy();
		res &= mGameObjectManager.Destroy();
		res &= mLog.Destroy();

		// MemoryMgr must be the last one to be destroyed as it is the allocator for the other subsystems
		res &= mMemoryManager.Destroy();

		mInitialized = false;

		return res;
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

	IRenderManager* const Engine::GetRenderManager()
	{
		return &mRenderManager;
	}

	IMemoryManager* const Engine::GetMemoryManager()
	{
		return &mMemoryManager;
	}

	IGameObjectManager* const Engine::GetGameObjectManager()
	{
		return &mGameObjectManager;
	}

	ILogManager* const Engine::GetLogger()
	{
		return &mLog;
	}
}