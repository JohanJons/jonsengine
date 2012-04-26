#include "../../include/Core/Engine.h"

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

	Engine::Engine() : mRunning(false), mInitialized(false)
	{
		#ifdef ANDROID
			mJNIEnv = NULL;
		#endif
	}

	bool Engine::Init(EngineSettings& initSettings)
	{
		bool res = true;

		// MemoryMgr must be the first one to be initialized as all other subsystems are dependant on it for memory allocation that might occur in their initialization and onwards
		res &= mMemoryManager.Init(&mLog);

		// Only proceed if MemoryMgr is valid
		if (res)
		{
			// Log
			bool LogToFile = initSettings.GetLogToFile();
			bool LogToSTDOut = initSettings.GetLogToSTDOut();
			std::string fileLocation = initSettings.GetLogToFileLocation();

			mEngineSettings.SetLogToFile(LogToFile,fileLocation);
			mEngineSettings.SetLogToSTDOut(LogToSTDOut);

			res &= mLog.Init(LogToFile,LogToSTDOut,fileLocation,&mMemoryManager);


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

	IRenderManager* Engine::GetRenderManager()
	{
		return &mRenderManager;
	}

	IMemoryManager* Engine::GetMemoryManager()
	{
		return &mMemoryManager;
	}

	IGameObjectManager* Engine::GetGameObjectManager()
	{
		return &mGameObjectManager;
	}

	ILogManager* Engine::GetLogger()
	{
		return &mLog;
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