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
		delete mMemTestMgr;
	}

	void TestManager::Init(uint64_t RepeatTimes)
	{
		if (RepeatTimes>0)
			mRepeatTimes = RepeatTimes;
		else 
		{
			mRepeatTimes = 1;
		}

		mEngine = Engine::GetEngine();
	}

	bool TestManager::InitEngine()
	{
		bool res = true;

		/* Init Engine and test mgrs */
		if(mEngine->Init(0,0,0,0))
		{
			res = true;
			res && mEngine->GetRenderManager()->Init();
			res && mEngine->GetLogger()->Init(false,true);
			res && mEngine->GetMemoryManager()->Init(false);
			res && mEngine->GetGameObjectManager()->Init();

			mMemTestMgr = new MemoryTestManager(mEngine->GetGameObjectManager());
			mGameObjTestMgr = new GameObjectTestManager(mEngine->GetGameObjectManager());
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

		return mEngine->Destroy();
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
