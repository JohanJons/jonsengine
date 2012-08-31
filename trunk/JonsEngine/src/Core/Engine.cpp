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

	Engine::Engine() : mRunning(false), mInitialized(false),
						mRenderManager(mLog, mMemoryManager.GetHeapAllocator()),
						mGameObjectManager(mLog, &mMemoryManager.GetHeapAllocator()),
						mThreadingFactory(mLog, mMemoryManager.GetHeapAllocator())
	{
		
	}

	bool Engine::Init()
	{
		bool res = true;

		res &= InitializeManagers();
		res &= InitializeFactories();

		if (res)
			mInitialized = true;

		return res;
	}

	bool Engine::InitializeManagers()
	{
		bool ret = true; 

		// MemoryMgr must be the first one to be initialized as all other subsystems are dependant on it for memory allocation that might occur in their initialization and onwards
		// Likewise, last to be destroyed
		ret &= mMemoryManager.Init();

		// Log
		#ifdef ANDROID
			ret &= mLog.Init(mEngineSettings.GetLogToFile(), mEngineSettings.GetLogToSTDOut(), mEngineSettings.GetLogToFileLocation(), mJNIEnv);
		#else
			ret &= mLog.Init(mEngineSettings.GetLogToFile(), mEngineSettings.GetLogToSTDOut(), mEngineSettings.GetLogToFileLocation());
		#endif

		// Render
		ret &= mRenderManager.Init();

		// GameObject 
		ret &= mGameObjectManager.Init();

		return ret;
	}
		
	bool Engine::InitializeFactories()
	{
		bool ret = true; 

		ret &= mThreadingFactory.Init();

		return ret;
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

		res &= DestroyFactories();
		res &= DestroyManagers();

		if (res)
			mInitialized = false;

		return res;
	}

	bool Engine::DestroyFactories()
	{
		bool ret = true;

		ret &= mThreadingFactory.Destroy();

		return ret;
	}

	bool Engine::DestroyManagers()
	{
		bool ret = true;

		ret &= mRenderManager.Destroy();
		ret &= mGameObjectManager.Destroy();
		ret &= mLog.Destroy();

		// MemoryMgr must be the last one to be destroyed as it is the allocator for the other subsystems
		ret &= mMemoryManager.Destroy();

		return ret;
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

	IRenderManager& Engine::GetRenderManager()
	{
		return mRenderManager;
	}

	IMemoryManager& Engine::GetMemoryManager()
	{
		return mMemoryManager;
	}

	IGameObjectManager& Engine::GetGameObjectManager()
	{
		return mGameObjectManager;
	}

	ILogManager& Engine::GetLogger()
	{
		return mLog;
	}

	IThreadingFactory& Engine::GetThreadingFactory()
	{
		return mThreadingFactory;
	}
}