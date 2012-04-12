#include "TestManager.h"

namespace JonsEngine
{

	TestManager::TestManager()
	{
		Init(1);
	}

	TestManager::TestManager(uint64_t RepeatTimes)
	{
		Init(RepeatTimes);
	}

	TestManager::~TestManager()
	{
	}

	void TestManager::Init(uint64_t RepeatTimes)
	{
		if (RepeatTimes>0)
			mRepeatTimes = RepeatTimes;
		else 
		{
			mRepeatTimes = 1;
		}

		mEngineSettings.SetLogToFile(true);
		mEngineSettings.SetLogToSTDOut(true);
		mEngineSettings.SetAllocatorBackEnd(DLMALLOC);
	}

	bool TestManager::InitEngine()
	{
		bool res = true;

		mEngine = new Engine();

		/* Init Engine and test mgrs */
		if(mEngine->Init(mEngineSettings))
		{
			res = true;

			mMemTestMgr = new MemoryTestManager(mEngine->GetGameObjectManager(), mEngine);
			mGameObjTestMgr = new GameObjectTestManager(mEngine->GetGameObjectManager(),mEngine);
		}
		else
			return false;

		return res;
	}
	
	bool TestManager::StartEngine()
	{
		bool res = true;

		res &= mEngine->Start();

		res &= mEngine->isRunning();

		return res;
	}
	
	bool TestManager::StopEngine()
	{
		bool res = true;

		res &= mEngine->Stop();

		res &= !mEngine->isRunning();

		return res;
	}
	
	bool TestManager::DestroyEngine()
	{
		bool res = true;

		if (mMemTestMgr)
		{
			delete mMemTestMgr;
			mMemTestMgr = NULL;
		}

		if (mGameObjTestMgr)
		{
			delete mGameObjTestMgr;
			mGameObjTestMgr = NULL;
		}

		res &= mEngine->Destroy();

		delete mEngine;

		return res;
	}


	/* Runs all testing modules */
	bool TestManager::RunAllTests()
	{
		bool res = true;

		for (int32_t i = 0; i<mRepeatTimes; i++)
		{
			/* Initialize Engine */
			res &= InitEngine();

			/* Start Engine */
			res &= StartEngine();

			/* Get logger */
			mLogger = mEngine->GetLogger();

			/* Run Memory Tests */
			if (res)
				res &= RunMemoryTests();

			/* Run Object Tests */
			if (res)
				res &= RunGameObjectTests();

			if (res)
				mLogger->LogInfo() << "TEST: TESTING SUCCESSFULL" << std::endl;
			else
				mLogger->LogError() << "TEST: TESTING FAILED" << std::endl;

			/* Stop and Destroy Engine */
			res &= StopEngine();
			res &= DestroyEngine();

			if (!res)
				break;
		}

		return res;
	}

	/* Runs all memory tests */
	bool TestManager::RunMemoryTests()
	{
		bool res = true;

		res &= mMemTestMgr->AllocateTest();

		if (res)
			mLogger->LogInfo() << "TEST: RunMemoryTests SUCCESSFULL" << std::endl;
		else
			mLogger->LogError() << "TEST: RunMemoryTests FAILED" << std::endl;

		return res;
	}

	/* Runs all object tests */
	bool TestManager::RunGameObjectTests()
	{
		bool res = true;

		res &= mGameObjTestMgr->ReferenceTest();

		if (res)
			mLogger->LogInfo() << "TEST: RunGameObjectTests SUCCESSFULL" << std::endl;
		else
			mLogger->LogError() << "TEST: RunGameObjectTests FAILED" << std::endl;

		return res;
	}

}

