#include "MemoryTestManager.h"

#include <iostream>

namespace JonsEngine
{
	MemoryTestManager::MemoryTestManager(IGameObjectManager* mGameObjFact) : mGameObjManager(mGameObjFact)
	{
		
	}

	/* TEST 1: Test memory allocation with and without memory pool */
	bool MemoryTestManager::AllocateTest()
	{
		Engine* engine = Engine::GetEngine();
		uint64_t remainingObjects = 0;
		uint64_t timeStart, timeStop;

		timeStart = GetTimeSinceEpoch();
		for (int32_t i = 0; i < ALLOCATE_NUM_ALLOCATIONS; i++)
		{
			AllocateTestAux();
		}
		timeStop = GetTimeSinceEpoch();

		engine->GetLogger()->LogInfo() << "TEST: MemoryTestManager:AllocateTest: Loop time: "
									   << timeStop - timeStart
									   << " msec"  << std::endl;


		remainingObjects = engine->GetMemoryManager()->GetCurrentAllocatedObjectsCount();
		if (remainingObjects != 0)
		{
			engine->GetLogger()->LogError() << "TEST: MemoryTestManager:AllocateTest: Objects remaining after loop: "
											<< remainingObjects  << std::endl;
			engine->GetLogger()->LogError() << "TEST: AllocateTest FAILED"  << std::endl;

			return false;
		}
		else
		{
			engine->GetLogger()->LogInfo() << "TEST: AllocateTest SUCCESSFULL"  << std::endl;

			return true;
		}


	}

	

}
