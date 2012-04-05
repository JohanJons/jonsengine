#include "GameObjectTestManager.h"

namespace JonsEngine
{
	GameObjectTestManager::GameObjectTestManager(IGameObjectManager* mObjFact) : mGameObjManager(mObjFact) 
	{
		
	}

	/* TEST 1: Testing reference counting */
	bool GameObjectTestManager::ReferenceTest()
	{
		Engine* engine = Engine::GetEngine();
		bool res = true;

		for (int32_t i = 0; i < JONSOBJECT_ITERATIONS; i++)
		{
			res && (GameObjectRefTest1() && engine->GetMemoryManager()->GetCurrentAllocatedObjectsCount() == 0);
		}

		if (res)
			engine->GetLogger()->LogInfo() << "TEST: ReferenceTest SUCCESSFULL"  << std::endl;
		else
			engine->GetLogger()->LogError() << "TEST: ReferenceTest FAILED" << std::endl;

		return res;
	}


	bool GameObjectTestManager::GameObjectRefTest1()
	{
		Engine* engine = Engine::GetEngine();
		boost::shared_ptr<TestClass1> test1 = mGameObjManager->CreateObject<TestClass1>();
		bool ret = true;

		if (test1)
		{
			test1->TestMe();
			ret && GameObjectRefTest2(test1);
		}
		else
			goto _bail;

		if (engine->GetMemoryManager()->GetCurrentAllocatedObjectsCount() == 1)
		{
			return ret;
		}
		else
		{
			engine->GetLogger()->LogError() << "TEST: GameObjectTestManager:GameObjectRefTest1: Allocated objects: "
											<< engine->GetMemoryManager()->GetCurrentAllocatedObjectsCount()
											<< std::endl;
			goto _bail;
		}

	_bail:
		engine->GetLogger()->LogError() << "TEST: GameObjectTestManager:GameObjectRefTest1: FAILED"  << std::endl;
		return false;
	}

	bool GameObjectTestManager::GameObjectRefTest2(boost::shared_ptr<TestClass1> test1)
	{
		Engine* engine = Engine::GetEngine();
		boost::shared_ptr<TestClass1> test2 = mGameObjManager->CreateObject<TestClass1>();
		bool ret = true;

		if (test1 && test2)
		{
			test1->TestMe();
			test2->TestMe();
			ret && GameObjectRefTest3(test1,test2);
		}
		else
			goto _bail;

		if (engine->GetMemoryManager()->GetCurrentAllocatedObjectsCount() == 2)
		{
			return ret;
		}
		else
		{
			engine->GetLogger()->LogError() << "TEST: GameObjectTestManager:GameObjectRefTest2: Allocated objects: "
											<< engine->GetMemoryManager()->GetCurrentAllocatedObjectsCount()
											<< std::endl;
			goto _bail;
		}

	_bail:
		engine->GetLogger()->LogError() << "TEST: GameObjectTestManager:GameObjectRefTest2: FAILED"  << std::endl;
		return false;
	}

	bool GameObjectTestManager::GameObjectRefTest3(boost::shared_ptr<TestClass1> test1,boost::shared_ptr<TestClass1> test2)
	{
		Engine* engine = Engine::GetEngine();
		boost::shared_ptr<TestClass1> test3 = mGameObjManager->CreateObject<TestClass1>();
		bool ret = true;

		if (test1 && test2 && test3)
		{
			test1->TestMe();
			test2->TestMe();
			test3->TestMe();
		}
		else
			goto _bail;

		if (engine->GetMemoryManager()->GetCurrentAllocatedObjectsCount() == 3)
		{
			return ret;
		}
		else
		{
			engine->GetLogger()->LogError() << "TEST: GameObjectTestManager:GameObjectRefTest3: Allocated objects: "
											<< engine->GetMemoryManager()->GetCurrentAllocatedObjectsCount()
										    << std::endl;
			goto _bail;
		}

	_bail:
		engine->GetLogger()->LogError() << "TEST: GameObjectTestManager:GameObjectRefTest3: FAILED" << std::endl;
		return false;
	}

}