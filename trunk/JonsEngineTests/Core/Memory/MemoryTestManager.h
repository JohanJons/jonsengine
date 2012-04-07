#ifndef _MEMORY_MANAGER_TEST_H
#define _MEMORY_MANAGER_TEST_H

#include "Core/Engine.h"
#include "../../TestClass1.h"
#include "Core/GameObject/GameObject.h"
#include "Core/Utils/JonsTime.h"

#define ALLOCATE_NUM_LOOPS 20
#define ALLOCATE_NUM_ALLOCATIONS 1000000

namespace JonsEngine
{
	class MemoryTestManager 
	{
	public:
		MemoryTestManager(IGameObjectManager* mObjFact,Engine* engine);

		/* Memory tests */
		bool AllocateTest();

	private:
		inline void AllocateTestAux() { mGameObjManager->CreateObject<TestClass1>(); }

		IGameObjectManager* mGameObjManager;
		Engine* mEngine;
	};

}

#endif
