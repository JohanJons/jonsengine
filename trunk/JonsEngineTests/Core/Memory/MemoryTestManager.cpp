#include "MemoryTestManager.h"

#include <iostream>

namespace JonsEngine
{
	MemoryTestManager::MemoryTestManager(IGameObjectManager* mGameObjFact,Engine* engine) : mGameObjManager(mGameObjFact), mEngine(engine)
	{
		
	}

	/* TEST 1: Test memory allocation with and without memory pool */
	bool MemoryTestManager::AllocateTest()
	{
		uint64_t remainingObjects = 0;
		uint64_t timeStart, timeStop;

		timeStart = GetTimeSinceEpoch();
		for (int32_t i = 0; i < ALLOCATE_NUM_ALLOCATIONS; i++)
		{
			AllocateTestAux();
		}
		timeStop = GetTimeSinceEpoch();

		mEngine->GetLogger()->LogInfo() << "TEST: MemoryTestManager:AllocateTest: Loop time: "
									   << timeStop - timeStart
									   << " msec"  << std::endl;


		remainingObjects = mEngine->GetMemoryManager()->GetCurrentAllocatedObjectsCount();
		if (remainingObjects != 0)
		{
			mEngine->GetLogger()->LogError() << "TEST: MemoryTestManager:AllocateTest: Objects remaining after loop: "
											<< remainingObjects  << std::endl;
			mEngine->GetLogger()->LogError() << "TEST: AllocateTest FAILED"  << std::endl;

			return false;
		}
		else
		{
			mEngine->GetLogger()->LogInfo() << "TEST: AllocateTest SUCCESSFULL"  << std::endl;

			return true;
		}


	}

	

}
